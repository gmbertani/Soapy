// -----------------------------------------------------------------------------
// FILE: 		common.h
// DESCRIPTION:	Header for common.cpp
// DATE:		2013-05-06
// AUTHOR(s):	Lime Microsystems
// REVISIONS:
// -----------------------------------------------------------------------------
#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include <stdio.h>
#include "assert.h"
#include "iniParser.h"
using namespace std;

#define RegistryString "Software\\ctr_6002d\\Settings"

//--- Lengths should be = Last Used Address + 1;
#define TopLen    0x0C
#define TxPllLen  0x10
#define RxPllLen  0x10
#define TxLpfLen  0x10
#define RxLpfLen  0x10
#define TxRFLen   0x0F
#define RxVGA2Len 0x0F
#define RxFELen   0x0E
#define ADFLen    0x0C

#define TopAddr		0x00
#define TxPllAddr 	0x01
#define RxPllAddr 	0x02
#define TxLpfAddr 	0x03
#define TxRFAddr	0x04
#define RxLpfAddr 	0x05
#define RxVGA2Addr	0x06
#define RxFEAddr	0x07
#define ADFAddr 	0x10

#define DCClbrCnt	0x05

typedef INI <string, string, string> ini_t;

struct sPrData
{
	int iLen;		//Number of registers in the hardware SPI
	unsigned char *cCmdW;	//Write command bytes
	unsigned char *cDataW;	//Write data bytes
	unsigned char *cCmdR;	//Read command bytes
	unsigned char *cDataR;	//Read data bytes

	int iToR;		//How much bytes we need to read from Hardware
	int iToW;		//How much bytes we need to write to Hardware
	int *iRInd;		//Indexes of cDataR array, required to raed
	int *iWInd;		//Indexes of cDataW array, required to write

	//Registers testing
	char *cTestMap;
};

struct sVcoVsCap
{
	int iRef;		//Number of reference points
	double *dFreq;
	double *iCap;
};

int QuadraticInterpol(int iRefVCnt, double *dFVco, double *dCVco, double dRealFvco);

struct SelectionContainer
{
	int itemIndex; //currently selected item
	vector<string> sItems; //item names
	SelectionContainer();
	void clear();
	void add(const string& str);
	void add(const char* ch);
	void add(const int integer);
	int indexOf(const string &str);
	int size();
};

struct LabelString
{
	string caption;
	LabelString();
	void set (const int& integer);
	void set (const string& str);
	void set (const double& dbl);
	double ToDouble();
	int ToInt();
};

struct BoolCheckBox
{
	bool checked;
	BoolCheckBox();
};

string Int2Hex(int integer, int symCount);

#endif
