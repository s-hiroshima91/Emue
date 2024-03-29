#ifndef CPUPARAM_H
#define CPUPARAM_H

static int cpuCycles[256] = {
 /*0x00*/ 7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
  /*0x10*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
  /*0x20*/ 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
  /*0x30*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
  /*0x40*/ 6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
  /*0x50*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
  /*0x60*/ 6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
  /*0x70*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 6, 7,
  /*0x80*/ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
  /*0x90*/ 2, 6, 2, 6, 4, 4, 4, 4, 2, 4, 2, 5, 5, 4, 5, 5,
  /*0xA0*/ 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
  /*0xB0*/ 2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
  /*0xC0*/ 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
  /*0xD0*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
  /*0xE0*/ 2, 6, 3, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
  /*0xF0*/ 2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};
/*
enum opeCode{
	BrkImp,
	OraXInd,
	OraZpg = 0x05,
	AslZpg,
	PhpImp =0x08,
	OraIm,
	AslAA,
	OraAbs = 0x0d,
	AslAbs,
	BplRel = 0x10,
	OraIndY,
	OraZpgX = 0x15,
	AslZpgX,
	ClcImp = 0x18,
	OraAbsY,
	OraAbsX = 0x1d,
	AslAbsX,
	JsrAbs = 0x20,
	AndXInd,
	BitZpg = 0x24,
	AndZpg,
	RolZpg,
	PlpImp = 0x28,
	AndIm,
	RolAA,
	BitAbs =0x2c,
	AndAbs,
	RolAbs,
	BmiRel = 0x30,
	AndIndY,
	AndZpgX = 0x35,
	RolZpgX,
	SecImp = 0x38,
	AndAbsY,
	AndAbsX = 0x3d,
	RolAbsX,
	RtiImp  = 0x40,
	EorXInd,
	EorZpg = 0x45,
	LsrZpg,
	PhaImp = 0x48,
	EorIm,
	LsrAA,
	JmpAbs = 0x4c,
	EorAbs,
	LsrAbs,
	BvcRel = 0x50,
	EorIndY,
	EorZpgX = 0x55,
	LsrZpgX,
	CliImp = 0x58,
	EorAbsY,
	EorAbsX = 0x5d,
	LsrAbsX,
	RtsImp = 0x60,
	AdcXInd,
	AdcZpg = 0x65,
	RorZpg,
	PlaImp = 0x68,
	AdcIm,
	RorAA,
	JmpInd = 0x6c,
	AdcAbs,
	RorAbs,	
	BvsRel = 0x70,
	AdcIndY,
	AdcZpgX = 0x75,
	RorZpgX,
	SeiImp = 0x78,
	AdcAbsY,
	AdcAbsX = 0x7d,
	RorAbsX,	
	StaXInd = 0x81,
	StyZpg = 0x84,
	StaZpg,
	StxZpg,
	DeYImp = 0x88,
	TxaImp = 0x8a,
	StyAbs = 0x8c,
	StaAbs,
	StxAbs,
	BccRel = 0x90,
	StaIndY,
	StyZpgX = 0x94,
	StaZpgX,
	StxZpgY,
	TyaImp = 0x98,
	StaAbsY,
	TxsImp,
	StaAbsX = 0x9d,
	LdyIm = 0xa0,
	LdaXInd,
	LdxIm,
	LdyZpg = 0xa4,
	LdaZpg,
	LdxZpg,
	TayImp = 0xa8,
	LdaIm,
	TaxImp,
	LdyAbs = 0xac,
	LdaAbs,
	LdxAbs,
	BcsRel = 0xb0,
	LdaIndY,
	LdyZpgX = 0xb4,
	LdaZpgX,
	LdxZpgY,
	ClvImp = 0xb8,
	LdaAbsY,
	TsxImp,
	LdyAbsX = 0xbc,
	LdaAbsX,
	LdxAbsY,
	CpYIm = 0xc0,
	CmpXInd,
	CpYZpg = 0xc4,
	CmpZpg,
	DecZpg,
	InYImp = 0xc8,
	CmpIm,
	DeXImp,
	CpYAbs = 0xcc,
	CmpAbs,
	DecAbs,
	BneRel = 0xd0,
	CmpIndY,
	CmpZpgX = 0xd5,
	DecZpgX,
	CldImp = 0xd8,
	CmpAbsY,
	CmpAbsX = 0xdd,
	DecAbsX,
	CpXIm = 0xe0,
	SbcXInd,
	CpXZpg = 0xe4,
	SbcZpg,
	IncZpg,
	InXImp = 0xe8,
	SbcIm,
	NopImp,
	CpXAbs = 0xec,
	SbcAbs ,
	IncAbs,
	BeqRel = 0xf0,
	SbcIndY,
	SbcZpgX = 0xf5,
	IncZpgX,
	SedImp  = 0xf8,
	SbcAbsY,
	SbcAbsX = 0xfd,
	IncAbsX,	
};
*/


#endif