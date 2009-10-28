//==================================================================
/// RSLC_Functions_DiscoverFunctions.cpp
///
/// Created by Davide Pasca - 2009/9/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Functions.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

#include "RSLC_Functions.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void removeCommasFromParams( TokNode *pNode )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TokNode	*pChild = pNode->mpChilds[i];
		if ( pChild->mpToken->id == T_OP_COMMA )
		{
			if ( pChild->mpChilds.size() != 0 )
			{
				throw Exception( "Broken function definition",  pNode );
			}

			pChild->UnlinkFromParent();
			DSAFE_DELETE( pChild );
			--i;
		}
	}
}

//==================================================================
static void discoverFuncsDeclarations( TokNode *pRoot )
{
	DVec<Function>	&funcs = pRoot->GetFuncs();

	for (size_t i=0; i < pRoot->mpChilds.size(); ++i)
	{
		TokNode	*pNode = pRoot->mpChilds[i];

		if NOT( pNode->IsCodeBlock() )
			continue;

		TokNode	*pParamsBlk	= NULL;
		TokNode	*pFuncName	= NULL;
		TokNode	*pRetType	= NULL;

		if ( !(pParamsBlk	= pRoot->GetChildTry(i-2)) ||
			 !(pFuncName	= pRoot->GetChildTry(i-3)) ||
			 !(pRetType		= pRoot->GetChildTry(i-4)) )
		{
			throw Exception( "Broken function definition",  pNode->mpToken );
		}

		Function	*pFunc = funcs.grow();

		removeCommasFromParams( pParamsBlk );

		pFunc->mpParamsNode		= pParamsBlk;
		pFunc->mpCodeBlkNode	= pNode;
		pFunc->mpNameNode		= pFuncName;
		pFunc->mpRetTypeTok		= pRetType->mpToken;

		// params block becomes children of the function body
		pFunc->mpCodeBlkNode->Reparent( pFunc->mpParamsNode );
		pFunc->mpParamsNode->AddChild( pFunc->mpCodeBlkNode );

		i -= 1;

		pFuncName->mNodeType = TokNode::TYPE_FUNCDEF;	// mark the node as a function definition

		if ( pRetType->mpToken->id == T_DT___funcop )
		{
			pFuncName->mIsFuncOp = true;
		}

		for (size_t j=0; j < pFunc->mpParamsNode->mpChilds.size(); ++j)
		{
			const TokNode	*pParamNode = pFunc->mpParamsNode->mpChilds[j];
			const Token		*pParamTok = pParamNode->mpToken;

			if ( pParamTok->idType == T_TYPE_NONTERM )
			{
				DASSERT( pParamNode->mVarLink.IsValid() );

				pFunc->mParamsVarTypes.push_back( pParamNode->GetVarType() );
			}
		}
	}
}

//==================================================================
static void discoverFuncsUsageSub( TokNode *pFuncCallNode, int &out_parentIdx )
{
	// constructor ?
	bool	isDataType = pFuncCallNode->IsDataType();

	TokNode *pRightNode = pFuncCallNode->GetRight();

	if ( pRightNode && pRightNode->mpToken->id == T_VL_STRING )
	{
		if ( isDataType )
		{
			// ok, it's just a space cast..

			// ..next should be a bracket
			TokNode *pParamsList = pRightNode->GetRight();

			pRightNode->Reparent( pParamsList );
			pParamsList->mpChilds.push_front( pRightNode );

			pRightNode = pParamsList;
/*
			// ..reparent the cast string as child of the function call node
			out_parentIdx -= 1;

			pRightNode->Reparent( pFuncCallNode );
			pFuncCallNode->mpChilds.push_back( pRightNode );
*/
		}
		else
		{
			throw Exception( "Unknown usage, why a string ?",  pFuncCallNode );
		}
	}

	if ( pRightNode && pRightNode->mpToken->id == T_OP_LFT_BRACKET )
	{
		// set the block type as a function call
		pRightNode->UpdateBlockTypeToFuncCall();

		/*
			b
			(
			)

			..becomes..

			b
				(
				)

			..the parent loop will skip one, so we need to backpedal..
		*/

		out_parentIdx -= 1;

		pRightNode->Reparent( pFuncCallNode );
		pFuncCallNode->mpChilds.push_back( pRightNode );

		removeCommasFromParams( pRightNode );

		pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;
	}
}

//==================================================================
static void discoverFuncsUsage( TokNode *pNode, const DVec<Function> &funcs, int &out_parentIdx )
{
	bool	isDataType = pNode->IsDataType();

	if ( !pNode->IsParentRoot() &&  // parent cannot be root !!
		 (pNode->IsNonTerminal() ||
		  pNode->IsStdFunction() ||
		  isDataType) &&
			pNode->mNodeType == TokNode::TYPE_STANDARD )
	{
		discoverFuncsUsageSub( pNode, out_parentIdx );
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		discoverFuncsUsage( pNode->mpChilds[i], funcs, i );
	}
}

//==================================================================
void DiscoverFunctions( TokNode *pRoot )
{
	discoverFuncsDeclarations( pRoot );

	int	idx = 0;
	discoverFuncsUsage( pRoot, pRoot->GetFuncs(), idx );
}

#if 0
//==================================================================
//==================================================================
//==================================================================
static void discoverFuncopsUsage_sub( TokNode *pNode, int &out_parentIdx )
{
	if (!pNode->IsParentRoot() &&  // parent cannot be root !!
			pNode->IsFuncOp() &&
				pNode->mNodeType == TokNode::TYPE_STANDARD )
	{
		TokNode *pRightNode = pNode->GetRight();

		if ( pRightNode && pRightNode->mpToken->id == T_OP_LFT_BRACKET )
		{
			// set the block type as a function call
			pRightNode->UpdateBlockTypeToFuncOpExpression();

			/*
				b
				(

				..becomes..

				b
					(

				..the parent loop will skip one, so we need to backpedal..
			*/

			out_parentIdx -= 1;

			// set the expression block as parent of the node with our funcop
			pRightNode->Reparent( pNode );
			pNode->mpChilds.push_back( pRightNode );

			// remove separating commas as we don't need them anymore
			removeCommasFromParams( pRightNode );

			pNode->mNodeType = TokNode::TYPE_FUNCOP;
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		discoverFuncopsUsage_sub( pNode->mpChilds[i], out_parentIdx );
	}
}

//==================================================================
void DiscoverFuncopsUsage( TokNode *pRoot )
{
	int	idx = 0;
	discoverFuncopsUsage_sub( pRoot, idx );
}

//==================================================================
static void reparentFuncopsStatements_sub( TokNode *pNode, int &out_parentIdx )
{
	//if ( pNode->GetBlockType() == BLKT_FUNCOPEXPR )
	if ( pNode->mIsFuncOp )
	{
		/*
			b
				(
			statement

			..becomes..

			b
				(
					statement

			..the parent loop will skip one, so we need to backpedal..
		*/

		//out_parentIdx -= 1;

		TokNode *pParamsBracket = pNode->GetRight();

		if NOT( pParamsBracket )
			throw Exception( "Missing statement ?",  pNode );

		TokNode *pStmtNode = pParamsBracket->GetRight();
		
		TokNode *pNewParent = pParamsBracket->mpChilds.back();

		if ( !pStmtNode || !pNewParent )
			throw Exception( "Missing statement ?",  pNode );

		// set statement as child of the expression block
		pStmtNode->Reparent( pNewParent );
		pNewParent->mpChilds.push_back( pStmtNode );
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		reparentFuncopsStatements_sub( pNode->mpChilds[i], out_parentIdx );
	}
}

//==================================================================
void ReparentFuncopsStatements( TokNode *pRoot )
{
	int	idx = 0;
	reparentFuncopsStatements_sub( pRoot, idx );
}
#endif

//==================================================================
}
