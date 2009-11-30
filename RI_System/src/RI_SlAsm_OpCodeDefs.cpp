//==================================================================
/// RI_SlAsm_OpCodeDefs.cpp
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SlAsm_OpCodeDefs.h"

//==================================================================
namespace RI
{

//==================================================================
// NOTE: this table must match the order of callbacks in sInstructionTable in RI_SlShader.cpp
SlAsmOpCodeDef	_gSlAsmOpCodeDefs[] =
{
	"ret"			,	0,			0,	OPRTYPE_NA,	OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,

	"mov.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"mov.vs"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"mov.vv"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"mov.xx"		,	2,			0,	OPRTYPE_STR,OPRTYPE_STR,OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"abs.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"abs.vs"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"abs.vv"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"sign.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"add.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"add.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"add.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
	"add.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"sub.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"sub.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"sub.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
	"sub.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"mul.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"mul.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"mul.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
	"mul.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
																					   
	"div.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"div.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"div.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,
	"div.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,

	"pow.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,

	"mov.vs3"		,	4,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,

	"dot.svv"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,

	"min.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"min.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,

	"max.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"max.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,

	"ld.s"			,	2,			OPC_FLG_RIGHTISIMM,		OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"ld.v"			,	4,			OPC_FLG_RIGHTISIMM,		OPRTYPE_F3,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"cmplt.ssl"		,	3,			OPC_FLG_UNIFORMOPERS,	OPRTYPE_F1,	OPRTYPE_F1,OPRTYPE_ADDR,OPRTYPE_NA,OPRTYPE_NA,

	"noise.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"noise.sv2"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F2, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"noise.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"noise.vs"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"noise.vv2"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F2, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"noise.vv"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"xcomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"ycomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"zcomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"setxcomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"setycomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"setzcomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"pxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,
	"vxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,
	"nxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,
	"cxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,

	"normalize"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"faceforward"	,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,
	"diffuse"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,
	"ambient"		,	1,			0,	OPRTYPE_F3,	OPRTYPE_NA, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"calculatenormal",	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	"solarbegin",		0,			OPC_FLG_DIRPOSLIGHT_INSTR | OPC_FLG_FUNCOP_BEGIN, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"solarbegin.vs",	2,			OPC_FLG_DIRPOSLIGHT_INSTR | OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, OPRTYPE_F1, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"illuminance.v",	1,			OPC_FLG_DIRPOSLIGHT_INSTR | OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, OPRTYPE_NA, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,
	"illuminance.vvs",	3,			OPC_FLG_DIRPOSLIGHT_INSTR | OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, OPRTYPE_F3, OPRTYPE_F1,	OPRTYPE_NA,	OPRTYPE_NA,
	"funcopend"		,	0,			OPC_FLG_FUNCOP_END,	  OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA,	OPRTYPE_NA,	OPRTYPE_NA,

	NULL			,	0,			0,	OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA, OPRTYPE_NA
};

//==================================================================
}
