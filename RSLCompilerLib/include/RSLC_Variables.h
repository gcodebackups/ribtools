//==================================================================
/// RSLC_Variables.h
///
/// Created by Davide Pasca - 2009/5/26
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_VARIABLES_H
#define RSLC_VARIABLES_H

#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{

//==================================================================
class TokNode;

//==================================================================
class Variable
{
public:
	Token		*mpDTypeTok;
	Token		*mpDetailTok;
	Token		*mpSpaceCastTok;
	Token		*mpNameTok;
	std::string	mInternalName;

	Variable() :
		mpDTypeTok(NULL),
		mpDetailTok(NULL),
		mpSpaceCastTok(NULL),
		mpNameTok(NULL)
	{
	}

	~Variable()
	{
	}
};

//==================================================================
void DiscoverVariables( TokNode *pNode );

//==================================================================
}

#endif
