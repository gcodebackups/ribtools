//==================================================================
/// RSLCompiler.cpp
///
/// Created by Davide Pasca - 2009/5/21
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <time.h>
#include "DSystem/include/DSafeCrt.h"
#include "DSystem/include/DUtils.h"
#include "DSystem/include/DContainers.h"
#include "DSystem/include/DUtils_Files.h"
#include "RSLC_Token.h"
#include "RSLC_Tree.h"
#include "RSLC_Variables.h"
#include "RSLC_Functions.h"
#include "RSLC_Operators.h"
#include "RSLC_Registers.h"
#include "RSLC_Constants.h"
#include "RSLC_Expressions.h"
#include "RSLCompiler.h"
#include "RSLC_RRASMOut.h"
#include "RSLC_Prepro.h"
#include "RSLC_Arrays.h"

//==================================================================
using namespace	RSLC;

//==================================================================
const char	*RSLCompiler::mpsVersionString = "0.4a";

//==================================================================
RSLCompiler::RSLCompiler(
		const char *pSLFName,
		const char *pSource,
		size_t sourceSize,
		const char *pBaseInclude,
		const Params &params )
{
	FatBase		fatBase;
	DVec<Fat8>	source;
	fatBase.AppendNewFile( source, pSLFName, (const U8 *)pSource, sourceSize );
	DVec<Fat8>	processedSource;

	DStr	curShaderDir = DUT::GetDirNameFromFPathName( pSLFName );

	PREPRO::Prepro
				prepro(
					*params.mpFileManager,
					fatBase,
					source,
					pBaseInclude,
					curShaderDir.c_str(),
					processedSource );

	Tokenizer( mTokens, fatBase, processedSource );

#if 0	// useful to debug the tokenizer
	for (size_t i=0; i < mTokens.size(); ++i)
	{
		printf( "%3i) %i - %-12s - %s\n",
				i,
				mTokens[i].isPrecededByWS,
				GetTokenTypeStr( mTokens[i].idType ),
				mTokens[i].str.c_str() );
	}
#endif

	mpRoot = DNEW TokNode( (Token *)NULL );

	// make the basic tree with nested blocks based on brackets
	u_int	blockCnt = 0;
	MakeTree( mpRoot, mTokens, blockCnt );

	// discover variables declarations
	DiscoverVariablesDeclarations( mpRoot );

	// discover functions declarations and usage
	DiscoverFunctions( mpRoot, blockCnt );

	// remove semicolons as they serve no additional purpose
	RemoveSemicolons( mpRoot );

	// develop the tree based on operators with the proper precedence
	ReparentOperators( mpRoot );

	RemoveOpeningExprBrackets( mpRoot );

	// simplify assign-with-arithmetic operators
	// example: a += b  ..becomes..  a = a + b
	ExpandAssingOperators( mpRoot );

	OptimizeConstantExpressions( mpRoot );

	RealizeArraysSizes( mpRoot );

	GenerateArrayAssignOperators( mpRoot );

	// discover variables usage
	DiscoverVariablesUsage( mpRoot );

	RealizeConstants( mpRoot );

	SolveExpressions( mpRoot, false );

	ResolveFunctionCalls( mpRoot );

	MarkUsedVariables( mpRoot );

	SolveGlobalConstants( mpRoot );

	SolveVariablesDetail( mpRoot );

	AssignRegisters( mpRoot, 0 );

	CloseFuncOps( mpRoot );

	// produce some debug info in the output file
	if ( params.mDbgOutputTree )
		TraverseTree( mpRoot, 0 );
}

//==================================================================
RSLCompiler::~RSLCompiler()
{
	DSAFE_DELETE( mpRoot );
}

//==================================================================
void RSLCompiler::SaveASM( const char *pFName, const char *pRefSourceName )
{
	// return;

	FILE	*pFile;

	if ( fopen_s( &pFile, pFName, "wb" ) )
	{
		DASSTHROW( 0, ("Failed to save %s", pFName) );
	}

	char dateStr[256];
	char timeStr[256];
	numstrdate( dateStr);
	_strtime_s( timeStr );

	fprintf_s( pFile, ";==========================================================\n" );
	fprintf_s( pFile, ";= %s\n", pFName );
	fprintf_s( pFile, ";= Source file %s\n", pRefSourceName );
	fprintf_s( pFile, ";= Creation date %s - %s\n", dateStr, timeStr );
	fprintf_s( pFile, ";= File automatically generated by RSLCompilerCmd %s\n", mpsVersionString );
	fprintf_s( pFile, ";=========================================================\n\n" );

	fprintf_s( pFile, "\n.data\n" );

	RRASMOut::WriteVariables( pFile, mpRoot );

	fprintf_s( pFile, "\n.code\n" );

	RRASMOut::WriteFunctions( pFile, mpRoot );

	fclose( pFile );
}
