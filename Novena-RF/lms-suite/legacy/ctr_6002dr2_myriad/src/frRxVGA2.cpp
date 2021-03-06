// -----------------------------------------------------------------------------
// FILE: 		    frRxVGA2.cpp
// DESCRIPTION:	RxVGA2 control user interface
// DATE:
// AUTHOR(s):	  Lime Microsystems
// REVISIONS:
// -----------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "frRxVGA2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//TfrmRxVGA2 *frmRxVGA2;


//0x0
#define             DC_REGVAL_r00_b50() ((m_psD.cDataR[0x00] & 0x3F) >> 0)
//0x1
#define               DC_LOCK_r01_b42() ((m_psD.cDataR[0x01] & 0x1C) >> 2)
#define          DC_CLBR_DONE_r01_b11() ((m_psD.cDataR[0x01] & 0x02) >> 1)
#define                 DC_UD_r01_b00() ((m_psD.cDataR[0x01] & 0x01) >> 0)
//0x2
#define             DC_CNTVAL_r02_b50() ((m_psD.cDataR[0x02] & 0x3F) >> 0)
//0x3
#define         DC_START_CLBR_r03_b55() ((m_psD.cDataR[0x03] & 0x20) >> 5)
#define               DC_LOAD_r03_b44() ((m_psD.cDataR[0x03] & 0x10) >> 4)
#define             DC_SRESET_r03_b33() ((m_psD.cDataR[0x03] & 0x08) >> 3)
#define               DC_ADDR_r03_b20() ((m_psD.cDataR[0x03] & 0x07) >> 0)
//0x4
#define                   VCM_r04_b52() ((m_psD.cDataR[0x04] & 0x3C) >> 2)
#define                    EN_r04_b11() ((m_psD.cDataR[0x04] & 0x02) >> 1)
#define                DECODE_r04_b00() ((m_psD.cDataR[0x04] & 0x01) >> 0)
//0x5
#define              VGA2GAIN_r05_b40() ((m_psD.cDataR[0x05] & 0x1F) >> 0)
//0x6
#define                   PD9_r06_b55() ((m_psD.cDataR[0x06] & 0x20) >> 5)
#define                   PD8_r06_b44() ((m_psD.cDataR[0x06] & 0x10) >> 4)
//#define                   PD7_r06_b33() ((m_psD.cDataR[0x06] & 0x08) >> 3)
#define                   PD7_r06_b33_v0() ((m_psD.cDataR[0x06] & 0x08) >> 3) //When Ver=2, Rev=2, Mask=0
#define                   PD7_r0E_b77_v1() ((m_psD.cDataR[0x0E] & 0x80) >> 7) //When Ver=2, Rev=2, Mask=1
#define                   PD6_r06_b22() ((m_psD.cDataR[0x06] & 0x04) >> 2)
//#define                   PD5_r06_b11() ((m_psD.cDataR[0x06] & 0x02) >> 1)
#define                   PD5_r06_b11_v0() ((m_psD.cDataR[0x06] & 0x02) >> 1) //When Ver=2, Rev=2, Mask=0
#define                   PD5_r0E_b66_v1() ((m_psD.cDataR[0x0E] & 0x40) >> 6) //When Ver=2, Rev=2, Mask=1
#define                   PD4_r06_b00() ((m_psD.cDataR[0x06] & 0x01) >> 0)
//0x7
#define                   PD3_r07_b33() ((m_psD.cDataR[0x07] & 0x08) >> 3)
#define                   PD2_r07_b22() ((m_psD.cDataR[0x07] & 0x04) >> 2)
#define                   PD1_r07_b11() ((m_psD.cDataR[0x07] & 0x02) >> 1)
#define                   PD0_r07_b00() ((m_psD.cDataR[0x07] & 0x01) >> 0)
//0x8
#define             VGA2GAINB_r08_b74() ((m_psD.cDataR[0x08] & 0xF0) >> 4)
#define             VGA2GAINA_r08_b30() ((m_psD.cDataR[0x08] & 0x0F) >> 0)

#define FRIF_DIR_W "rfIf Direct=16,"
//#define FRIF_SPI_A ",20"

//---------------------------------------------------------------------------
__fastcall TfrmRxVGA2::TfrmRxVGA2(TComponent* Owner)
	: TFrame(Owner)
{
  m_cVer = 2;
  m_cRev = 2;
  m_cMask = 0;
}
//---------------------------------------------------------------------------
__fastcall TfrmRxVGA2::~TfrmRxVGA2(void)
{
	delete[] m_psD.cCmdW;
	delete[] m_psD.cDataW;
	delete[] m_psD.cCmdR;
	delete[] m_psD.cDataR;
	delete[] m_psD.iRInd;
	delete[] m_psD.iWInd;
  delete[] m_psD.cTestMap; //RT//
}

//---------------------------------------------------------------------------
void TfrmRxVGA2::Initialize(char MAddr)
{
	m_cMAddr = MAddr;

	m_psD.iLen = RxVGA2Len;
	m_psD.cCmdW = new char[m_psD.iLen];
	m_psD.cDataW = new char[m_psD.iLen];
	m_psD.cCmdR = new char[m_psD.iLen];
	m_psD.cDataR = new char[m_psD.iLen];
	m_psD.iRInd = new int[m_psD.iLen];
	m_psD.iWInd = new int[m_psD.iLen];
  m_psD.cTestMap = new char[m_psD.iLen];  //RT//
	memset(m_psD.cCmdW, 0, m_psD.iLen);
	memset(m_psD.cDataW, 0, m_psD.iLen);
	memset(m_psD.cCmdR, 0, m_psD.iLen);
	memset(m_psD.cDataR, 0, m_psD.iLen);
	memset(m_psD.iRInd, 0, m_psD.iLen*sizeof(int));
	memset(m_psD.iWInd, 0, m_psD.iLen*sizeof(int));
  memset(m_psD.cTestMap, 0, m_psD.iLen);  //RT//
	m_psD.iToR = m_psD.iLen;
	m_psD.iToW = 0;
	for(int i=0; i<m_psD.iLen; i++)
	{
		//Write Command and Addresses
		m_psD.cCmdW[i] = 1;
		m_psD.cCmdW[i] = m_psD.cCmdW[i] << 3;
		m_psD.cCmdW[i] |= m_cMAddr;
		m_psD.cCmdW[i] = m_psD.cCmdW[i] << 4;
		m_psD.cCmdW[i] |= i;
		//Read Command and Addresses
		m_psD.cCmdR[i] = 0;
		m_psD.cCmdR[i] = m_psD.cCmdR[i] << 3;
		m_psD.cCmdR[i] |= m_cMAddr;
		m_psD.cCmdR[i] = m_psD.cCmdR[i] << 4;
		m_psD.cCmdR[i] |= i;

		m_psD.iRInd[i] = i;
	};

  //Register test mask //RT//
	for(int i=0; i<m_psD.iLen; i++)
	{
    m_psD.cTestMap[i] = 0x00;
  };
  m_psD.cTestMap[0x0] = 0x00;
  m_psD.cTestMap[0x1] = 0x00;
  m_psD.cTestMap[0x2] = 0x3F;
  m_psD.cTestMap[0x3] = 0x3F;
  m_psD.cTestMap[0x4] = 0x3F;
  m_psD.cTestMap[0x5] = 0x1F;
  m_psD.cTestMap[0x6] = 0x3F;
  m_psD.cTestMap[0x7] = 0x0F;
  m_psD.cTestMap[0x8] = 0xFF;
  m_psD.cTestMap[0xE] = 0xC0;
  //m_psD.cTestMap[0x9] = 0x00;

  SetGuiDefaults();

};

//---------------------------------------------------------------------------
void TfrmRxVGA2::SetGuiDefaults()
{
	m_bAllowSend = false;
  int itmp;
  //VCM related
  double V1 = 0.9;
  double VLSB = 0.04;
  double VCMV;

	//VCM[3:0]
	cmbVCM->Clear();
  for(int i=0; i<=7; i++)
  {
    VCMV = V1 - (i - 7)*VLSB;
    cmbVCM->Items->Add(VCMV);
  };
  for(int i=14; i>=8; i--)
  {
    VCMV = V1 + (i - 15)*VLSB;
    cmbVCM->Items->Add(VCMV);
  };
/*  for(int i=0; i<15; i++)
  {
  	cmbVCM->Items->Add(i);
  };
*/
  cmbVCM->ItemIndex = 7;

  //EN
  chbPwrVGA2Mods->Checked = true;
	//DECODE
  rgrDecode->ItemIndex = 0;

  //VGA2GAIN[4:0]
  cmbVga2G_u->Clear();
  itmp = 0;
  for(int i=0; i<11; i++)
  {
	  cmbVga2G_u->Items->Add(itmp);
    itmp += 3;
  };
  cmbVga2G_u->ItemIndex = 1;

  //
  chbPwrDCCurrR->Checked = true;
  chbPwrDCDACB->Checked = true;
  chbPwrDCCmpB->Checked = true;
  chbPwrDCDACA->Checked = true;
  chbPwrDCCmpA->Checked = true;
  chbPwrBG->Checked = true;
  //
  chbPwrBypAB->Checked = true;
  chbPwrBypB->Checked = true;
  chbPwrBypA->Checked = true;
  chbPwrCurrRef->Checked = true;

  //VGA2GAINB[3:0]
  cmbVga2GB_t->Clear();
  cmbVga2GB_t->Items->Add("0");
  cmbVga2GB_t->Items->Add("3"); //Add("-0.4");
  cmbVga2GB_t->Items->Add("6"); //Add("2.5");
  cmbVga2GB_t->Items->Add("9"); //Add("5.5");
  cmbVga2GB_t->Items->Add("12"); //Add("8.5");
  cmbVga2GB_t->Items->Add("15"); //Add("11.5");
  cmbVga2GB_t->Items->Add("18"); //Add("14.2");
  cmbVga2GB_t->Items->Add("21"); //Add("16.7");
  cmbVga2GB_t->Items->Add("24"); //Add("19.5");
  cmbVga2GB_t->Items->Add("27"); //Add("21.5");
  cmbVga2GB_t->Items->Add("30"); //Add("24.0");
  cmbVga2GB_t->ItemIndex = 0;

  //VGA2GAINA[3:0]
  cmbVga2GA_t->Clear();
  cmbVga2GA_t->Items->Add("0");
  cmbVga2GA_t->Items->Add("3"); //Add("-0.4");
  cmbVga2GA_t->Items->Add("6"); //Add("2.5");
  cmbVga2GA_t->Items->Add("9"); //Add("5.5");
  cmbVga2GA_t->Items->Add("12"); //Add("8.5");
  cmbVga2GA_t->Items->Add("15"); //Add("11.5");
  cmbVga2GA_t->Items->Add("18"); //Add("14.2");
  cmbVga2GA_t->Items->Add("21"); //Add("16.7");
  cmbVga2GA_t->Items->Add("24"); //Add("19.5");
  cmbVga2GA_t->Items->Add("27"); //Add("21.5");
  cmbVga2GA_t->Items->Add("30"); //Add("24.0");
  cmbVga2GA_t->ItemIndex = 1;



	//DC_ADDR[3:0]
	cmbDCCalAddr->Clear();
  for(int i=0; i<5; i++)
  {
  	cmbDCCalAddr->Items->Add(i);
  };
  cmbDCCalAddr->ItemIndex = 0;

	//DC_CNTVAL[5:0]
	cmbCalVal->Clear();
  for(int i=0; i<64; i++)
  {
  	cmbCalVal->Items->Add(i);
  };
  cmbCalVal->ItemIndex = 31;

  m_DCClbrV[0] = 31;
  m_DCClbrV[1] = 31;
  m_DCClbrV[2] = 31;
  m_DCClbrV[3] = 31;
  m_DCClbrV[4] = 31;

  SetGuiDecode();

  m_bAllowSend = true;
};

//---------------------------------------------------------------------------
void TfrmRxVGA2::SetGuiDecode()
{
	if(rgrDecode->ItemIndex == 0)
  {
  	//Decode
  	chbPwrVGA2Mods->Enabled = true;
    cmbVga2G_u->Enabled = true;

    chbPwrDCCurrR->Enabled = false;
    chbPwrDCDACB->Enabled = false;
    chbPwrDCDACA->Enabled = false;

    //To resolove Ver=2 Rev=2 Mask=1 version
    if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
    {
      chbPwrDCCmpB->Enabled = true;
      chbPwrDCCmpA->Enabled = true;
    }
    else
    {
      chbPwrDCCmpB->Enabled = false;
      chbPwrDCCmpA->Enabled = false;
    };

    chbPwrBG->Enabled = false;
    chbPwrBypAB->Enabled = false;
    chbPwrBypB->Enabled = false;
    chbPwrBypA->Enabled = false;
    chbPwrCurrRef->Enabled = false;
    cmbVga2GB_t->Enabled = false;
    cmbVga2GA_t->Enabled = false;
  }
  else
  {
  	//Direct control
  	chbPwrVGA2Mods->Enabled = false;
    cmbVga2G_u->Enabled = false;

    chbPwrDCCurrR->Enabled = true;
    chbPwrDCDACB->Enabled = true;
    chbPwrDCCmpB->Enabled = true;
    chbPwrDCDACA->Enabled = true;
    chbPwrDCCmpA->Enabled = true;
    chbPwrBG->Enabled = true;
    chbPwrBypAB->Enabled = true;
    chbPwrBypB->Enabled = true;
    chbPwrBypA->Enabled = true;
    chbPwrCurrRef->Enabled = true;
    cmbVga2GB_t->Enabled = true;
    cmbVga2GA_t->Enabled = true;
  };
};

//----------------------------------------------------------------------------
// Set GUI according to the chip version, revision and mask
//----------------------------------------------------------------------------
void TfrmRxVGA2::UpdateVerRevMask(char Ver, char Rev, char Mask)
{
  m_cVer = Ver;
  m_cRev = Rev;
  m_cMask = Mask;

  m_bAllowSend = false;
  SetGuiDecode();
  m_bAllowSend = true;


  //Edit Register Test masks
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    m_psD.cTestMap[0xE] += 0x80;  //DC Offset comparator address is different when Mask=1
    m_psD.cTestMap[0xE] += 0x40;
    m_psD.cTestMap[0x6] -= 0x02;
    m_psD.cTestMap[0x6] -= 0x08;
  };

};

//----------------------------------------------------------------------------
// Make data to download
//----------------------------------------------------------------------------
void TfrmRxVGA2::MakeData(int rAddr)
{
	char btmp;

	//======= register addr 0x00 =======
	m_psD.cDataW[0x00] = 0x00;

	//======= register addr 0x01 =======
	m_psD.cDataW[0x01] = 0x00;

  //======= register addr 0x02 =======
  m_psD.cDataW[0x02] = 0x00;
  btmp = (char)cmbCalVal->ItemIndex;
  m_psD.cDataW[0x02] |= btmp;

	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x08;  //Not to reset calibration modules
	//DC_ADDR
  btmp = (char)cmbDCCalAddr->ItemIndex;
  m_psD.cDataW[0x03] |= btmp;


	//======= register addr 0x04 =======
	m_psD.cDataW[0x04] = 0x00;
  //VCM[3:0]
	//btmp = (char)cmbVCM->ItemIndex;
  btmp = (char)CMB2VCM(cmbVCM->ItemIndex);
	btmp = btmp << 2; m_psD.cDataW[0x04] |= btmp;
	//EN
	btmp = chbPwrVGA2Mods->Checked == true ? 1 : 0;
	btmp = btmp << 1; m_psD.cDataW[0x04] |= btmp;
	//DECODE
	btmp = (char)rgrDecode->ItemIndex;
	btmp = btmp << 0; m_psD.cDataW[0x04] |= btmp;

	//======= register addr 0x05 =======
	m_psD.cDataW[0x05] = 0x00;
	//VGA2GAIN[4:0]
	btmp = (char)cmbVga2G_u->ItemIndex;
	btmp = btmp << 0; m_psD.cDataW[0x05] |= btmp;

	//======= register addr 0x06 =======
	m_psD.cDataW[0x06] = 0x00;
	//PD[9]
	btmp = chbPwrDCCurrR->Checked == true ? 0 : 1;
	btmp = btmp << 5; m_psD.cDataW[0x06] |= btmp;
	//PD[8]
	btmp = chbPwrDCDACB->Checked == true ? 0 : 1;
	btmp = btmp << 4; m_psD.cDataW[0x06] |= btmp;
	//PD[7]
	btmp = chbPwrDCCmpB->Checked == true ? 0 : 1;
	btmp = btmp << 3; m_psD.cDataW[0x06] |= btmp;
	//PD[6]
	btmp = chbPwrDCDACA->Checked == true ? 0 : 1;
	btmp = btmp << 2; m_psD.cDataW[0x06] |= btmp;
	//PD[5]
	btmp = chbPwrDCCmpA->Checked == true ? 0 : 1;
	btmp = btmp << 1; m_psD.cDataW[0x06] |= btmp;
	//PD[4]
	btmp = chbPwrBG->Checked == true ? 0 : 1;
	btmp = btmp << 0; m_psD.cDataW[0x06] |= btmp;

	//======= register addr 0x07 =======
	m_psD.cDataW[0x07] = 0x00;
	//PD[3]
	btmp = chbPwrBypAB->Checked == true ? 0 : 1;
	btmp = btmp << 3; m_psD.cDataW[0x07] |= btmp;
	//PD[2]
	btmp = chbPwrBypB->Checked == true ? 0 : 1;
	btmp = btmp << 2; m_psD.cDataW[0x07] |= btmp;
	//PD[1]
	btmp = chbPwrBypA->Checked == true ? 0 : 1;
	btmp = btmp << 1; m_psD.cDataW[0x07] |= btmp;
	//PD[0]
	btmp = chbPwrCurrRef->Checked == true ? 0 : 1;
	btmp = btmp << 0; m_psD.cDataW[0x07] |= btmp;

	//======= register addr 0x08 =======
	m_psD.cDataW[0x08] = 0x00;
	//VGA2GAINB[3:0]
	btmp = (char)cmbVga2GB_t->ItemIndex;
	btmp = btmp << 4; m_psD.cDataW[0x08] |= btmp;
	//VGA2GAINA[3:0]
	btmp = (char)cmbVga2GA_t->ItemIndex;
	btmp = btmp << 0; m_psD.cDataW[0x08] |= btmp; 

	//======= register addr 0x0E =======
	m_psD.cDataW[0x0E] = 0x00;
	//PD[7]
	btmp = chbPwrDCCmpB->Checked == true ? 0 : 1;
	btmp = btmp << 7; m_psD.cDataW[0x0E] |= btmp;
	//PD[5]
	btmp = chbPwrDCCmpA->Checked == true ? 0 : 1;
	btmp = btmp << 6; m_psD.cDataW[0x0E] |= btmp;


  

	memset(m_psD.iWInd, 0, m_psD.iLen*sizeof(int));
	switch(rAddr)
	{
  	//DC Calibration
    case 0xF0:
			m_psD.iToW = 1;
			m_psD.iWInd[0] = 0x03;
		break;

    case 0xF1:
			m_psD.iToW = 1;
			m_psD.iWInd[0] = 0x02;
		break;

		//User mode
		case 0x00:
		case 0x01:
    case 0x0F:
			m_psD.iToW = 1;
			m_psD.iWInd[0] = 0x04;
		break;
    
		case 0x02:
			m_psD.iToW = 1;
			m_psD.iWInd[0] = 0x05;
		break;

    //Test mode
		case 0x03:
		case 0x04:
    case 0x05:
		case 0x06:
    case 0x07:
    case 0x08:
			m_psD.iToW = 2;
			m_psD.iWInd[0] = 0x04;
			m_psD.iWInd[1] = 0x06;
		break;

    case 0x051:
    case 0x071:
			m_psD.iToW = 1;
			m_psD.iWInd[0] = 0x0E;
		break;

		case 0x09:
		case 0x0A:
    case 0x0B:
		case 0x0C:
			m_psD.iToW = 2;
			m_psD.iWInd[0] = 0x04;
			m_psD.iWInd[1] = 0x07;
		break;

    case 0x0D:
		case 0x0E:
			m_psD.iToW = 2;
			m_psD.iWInd[0] = 0x04;
			m_psD.iWInd[1] = 0x08;
		break;
    
		default:
			m_psD.iToW = 0;
	};
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrVGA2ModsClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x00); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::rgrDecodeClick(TObject *Sender)
{
	SetGuiDecode();
  if(m_bAllowSend){ MakeData(0x01); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::cmbVga2G_uChange(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x02); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrDCCurrRClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x03); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrDCDACBClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x04); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrDCCmpBClick(TObject *Sender)
{
//	if(m_bAllowSend){ MakeData(0x05); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
  //To resolove Ver=2 Rev=2 Mask=1 version
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    if(m_bAllowSend){ MakeData(0x051); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
  }
  else
  {
    if(m_bAllowSend){ MakeData(0x05); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
  };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrDCDACAClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x06); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrDCCmpAClick(TObject *Sender)
{
//	if(m_bAllowSend){ MakeData(0x07); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
  //To resolove Ver=2 Rev=2 Mask=1 version
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    if(m_bAllowSend){ MakeData(0x071); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
  }
  else
  {
    if(m_bAllowSend){ MakeData(0x07); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
  };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrBGClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x08); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrBypABClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x09); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrBypBClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x0A); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrBypAClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x0B); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::chbPwrCurrRefClick(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x0C); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::cmbVga2GB_tChange(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x0D); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::cmbVga2GA_tChange(TObject *Sender)
{
	if(m_bAllowSend){ MakeData(0x0E); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::cmbVCMChange(TObject *Sender)
{
  if(m_bAllowSend){ MakeData(0x0F); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::cmbDCCalAddrChange(TObject *Sender)
{
  //Enable DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKONMASK, 0, 0x10); };

	if(m_bAllowSend){ MakeData(0xF0); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };

  //Restore DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKRESTORE, 0, 0); };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::cmbCalValChange(TObject *Sender)
{
//	if(m_bAllowSend){ MakeData(0xF1); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };	
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::btnDCRstClbrClick(TObject *Sender)
{
	char btmp;

  //Enable DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKONMASK, 0, 0x10); };


	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_LOAD
  btmp = 0x00;	//Set Load Value Inactive
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x00;	//Set Reset Active
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)cmbDCCalAddr->ItemIndex;
  m_psD.cDataW[0x03] |= btmp;

	m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_LOAD
  btmp = 0x00;	//Set Load Value Inactive
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)cmbDCCalAddr->ItemIndex;
  m_psD.cDataW[0x03] |= btmp;

  m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

  m_DCClbrV[0] = 12;
  m_DCClbrV[1] = 31;
  m_DCClbrV[2] = 31;
  m_DCClbrV[3] = 31;
  m_DCClbrV[4] = 31;

  //Restore DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKRESTORE, 0, 0); };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::btnDCLoadValClick(TObject *Sender)
{
	char btmp;

  //Enable DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKONMASK, 0, 0x10); };

	if(m_bAllowSend){ MakeData(0xF0); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };
  if(m_bAllowSend){ MakeData(0xF1); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); };


	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_LOAD
  btmp = 0x01;	//Load Value
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)cmbDCCalAddr->ItemIndex;
  m_psD.cDataW[0x03] |= btmp;

	m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);



	//======= register addr 0x03 =======
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
	m_psD.cDataW[0x03] = 0x00;
  //DC_LOAD
  btmp = 0x00;	//Deactivate Load Value
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)cmbDCCalAddr->ItemIndex;
  m_psD.cDataW[0x03] |= btmp;

  m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

  m_DCClbrV[cmbDCCalAddr->ItemIndex] = cmbCalVal->ItemIndex;

  //Restore DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKRESTORE, 0, 0); };
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::btnDCStartClbrClick(TObject *Sender)
{
  StartCalibration(cmbDCCalAddr->ItemIndex);
}
//---------------------------------------------------------------------------
void __fastcall TfrmRxVGA2::btnDCReadValClick(TObject *Sender)
{
  //Enable DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKONMASK, 0, 0x10); };

  MakeData(0xF0); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);
	SendMessage(G_hwWindow, CM_READ, 1, m_cMAddr);

  //Restore DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKRESTORE, 0, 0); };
}
//---------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Make all data to write
//----------------------------------------------------------------------------
void TfrmRxVGA2::MakeWriteAll()
{
	MakeData(0xFFFF);
	m_psD.iToW = m_psD.iLen;
	for(int i=0; i<m_psD.iLen; i++)
	{
		m_psD.iWInd[i] = i;
	};
};

//----------------------------------------------------------------------------
// Save the chip configuration to the file
//----------------------------------------------------------------------------
bool TfrmRxVGA2::ChipVsGui(ofstream &out, String Caption, bool bWrAll)
{
  bool bRez = true;
	char str[64] = "";
  int itmp;

  out << left;
	out << Caption.c_str() << endl;
	out << endl;
	out << "-----------------------------------------------------------------------------------------------------------------------------------------" << endl;
	out.width(32); out << "| Register"; out.width(2); out << "|";
	out.width(50); out << "Value from PC"; out.width(2); out << "|";
	out.width(50); out << "Value from Chip"; out.width(1); out << "|";
	out << endl;
	out << "--------------------------------+---------------------------------------------------+----------------------------------------------------" << endl;

  if(bWrAll)
  {
  	//DC_REGVAL[5:0]
    memset(str, '\0', sizeof(str));
  	out.width(32); out << "| DC_REGVAL[5:0]"; out.width(2); out << "|";	out.width(40);
    strcpy(str, "Read Only");
  	out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
    itmp = DC_REGVAL_r00_b50();
  	out << itmp;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;

    //DC_LOCK[2:0]
    memset(str, '\0', sizeof(str));
  	out.width(32); out << "| DC_LOCK[2:0]"; out.width(2); out << "|";	out.width(40);
    strcpy(str, "Read Only");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
	  itmp = DC_LOCK_r01_b42(); out << hex << showbase << uppercase;
    out << itmp;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;

    //DC_CLBR_DONE
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| DC_CLBR_DONE"; out.width(2); out << "|";	out.width(40);
    strcpy(str, "Read Only");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	if(DC_CLBR_DONE_r01_b11()) strcpy(str, "Calibration in Progress"); else strcpy(str, "Calibration Done");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;

    //DC_UD
    memset(str, '\0', sizeof(str));
  	out.width(32); out << "| DC_UD"; out.width(2); out << "|";	out.width(40);
    strcpy(str, "Read Only");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
	  if(DC_UD_r01_b00()) strcpy(str, "Counting Up"); else strcpy(str, "Counting Down");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;

    //DC_CNTVAL[5:0]
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| DC_CNTVAL[5:0]"; out.width(2); out << "|";	out.width(40);
    strcpy(str, "Read Only");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	itmp = DC_CNTVAL_r02_b50(); out << dec;// << showbase << uppercase;
    out << itmp;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;

    //DC_START_CLBR
    memset(str, '\0', sizeof(str));
  	out.width(32); out << "| DC_START_CLBR"; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
	  if(DC_START_CLBR_r03_b55()) strcpy(str, "Start Active"); else strcpy(str, "Start Not Active");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;

    //DC_LOAD
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| DC_LOAD"; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	if(DC_LOAD_r03_b44()) strcpy(str, "Load Active"); else strcpy(str, "Load Not Active");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;

    //DC_SRESET
    memset(str, '\0', sizeof(str));
  	out.width(32); out << "| DC_SRESET"; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
	  if(DC_SRESET_r03_b33()) strcpy(str, "Reset Not Active"); else strcpy(str, "Reset Active");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;

    //DC_ADDR[2:0]
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| DC_ADDR[2:0]"; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	itmp = DC_ADDR_r03_b20(); out << hex << showbase << uppercase;
    out << itmp;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
  };

  
  //VCM[3:0]
  if((CMB2VCM(cmbVCM->ItemIndex) != VCM_r04_b52()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| VCM[3:0]"; out.width(2); out << "|";	out.width(40);
    strcpy(str, cmbVCM->Items->Strings[CMB2VCM(cmbVCM->ItemIndex)].c_str());
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
	  itmp = VCM_r04_b52();
    strcpy(str, cmbVCM->Items->Strings[VCM2CMB(itmp)].c_str());
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //EN
  if((chbPwrVGA2Mods->Checked != (bool)EN_r04_b11()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| EN"; out.width(2); out << "|";	out.width(40);
    if(chbPwrVGA2Mods->Checked) strcpy(str, "Enabled"); else strcpy(str, "Powered Down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	memset(str, '\0', sizeof(str));
	  if(EN_r04_b11()) strcpy(str, "Enabled"); else strcpy(str, "Powered Down");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //DECODE
  if((rgrDecode->ItemIndex != DECODE_r04_b00()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| DECODE"; out.width(2); out << "|";	out.width(40);
    if(rgrDecode->ItemIndex == 1) strcpy(str, "Test Mode"); else strcpy(str, "User Mode");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
	  if(DECODE_r04_b00()) strcpy(str, "Test Mode"); else strcpy(str, "User Mode");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //VGA2GAIN[4:0]
  if((cmbVga2G_u->ItemIndex != VGA2GAIN_r05_b40()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| MIXVGA2GAIN[4:0]"; out.width(2); out << "|";	out.width(40);
    strcpy(str, cmbVga2G_u->Items->Strings[cmbVga2G_u->ItemIndex].c_str());
    out << str;	out.width(10); out << "dB"; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
	  itmp = VGA2GAIN_r05_b40();
    if(itmp < 11) strcpy(str, cmbVga2G_u->Items->Strings[itmp].c_str());
    else strcpy(str, "Wrong Data");
    out << str;	out.width(10); out << "dB"; out.width(1); out << "|"; out << endl;
    bRez = false;
  };


  if((!bRez) || bWrAll)
  {
    out << "| - - - - - - - - - - - - - - - + - - - - - - - - - - - - - - - - - - - - - - - - - + - - - - - - - - - - - - - - - - - - - - - - - - - |" << endl;
  };

  //PD[9]
  if((chbPwrDCCurrR->Checked == (bool)PD9_r06_b55()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| PD[9]"; out.width(2); out << "|";	out.width(40);
    if(chbPwrDCCurrR->Checked) strcpy(str, "DC Current Regulator powered up"); else strcpy(str, "DC Current Regulator powered down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	memset(str, '\0', sizeof(str));
	  if(PD9_r06_b55()) strcpy(str, "DC Current Regulator powered down"); else strcpy(str, "DC Current Regulator powered up");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //PD[8]
  if((chbPwrDCDACB->Checked == (bool)PD8_r06_b44()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| PD[8]"; out.width(2); out << "|";	out.width(40);
    if(chbPwrDCDACB->Checked) strcpy(str, "VGA2B DC Cal. DAC powered up"); else strcpy(str, "VGA2B DC Cal. DAC powered down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	memset(str, '\0', sizeof(str));
	  if(PD8_r06_b44()) strcpy(str, "VGA2B DC Cal. DAC powered down"); else strcpy(str, "VGA2B DC Cal. DAC powered up");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //PD[7]
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    if((chbPwrDCCmpB->Checked == (bool)PD7_r0E_b77_v1()) || bWrAll)
    {
      memset(str, '\0', sizeof(str));
  	  out.width(32); out << "| PD[7]"; out.width(2); out << "|";	out.width(40);
      if(chbPwrDCCmpB->Checked) strcpy(str, "VGA2B DC Cal. Comp. powered up"); else strcpy(str, "VGA2B DC Cal. Comp. powered down");
      out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
    	memset(str, '\0', sizeof(str));
	    if(PD7_r0E_b77_v1()) strcpy(str, "VGA2B DC Cal. Comp. powered down"); else strcpy(str, "VGA2B DC Cal. Comp. powered up");
      out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
      bRez = false;
    };
  }
  else
  {
    if((chbPwrDCCmpB->Checked == (bool)PD7_r06_b33_v0()) || bWrAll)
    {
      memset(str, '\0', sizeof(str));
  	  out.width(32); out << "| PD[7]"; out.width(2); out << "|";	out.width(40);
      if(chbPwrDCCmpB->Checked) strcpy(str, "VGA2B DC Cal. Comp. powered up"); else strcpy(str, "VGA2B DC Cal. Comp. powered down");
      out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
    	memset(str, '\0', sizeof(str));
	    if(PD7_r06_b33_v0()) strcpy(str, "VGA2B DC Cal. Comp. powered down"); else strcpy(str, "VGA2B DC Cal. Comp. powered up");
      out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
      bRez = false;
    };
  };

  //PD[6]
  if((chbPwrDCDACA->Checked == (bool)PD6_r06_b22()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| PD[6]"; out.width(2); out << "|";	out.width(40);
    if(chbPwrDCDACA->Checked) strcpy(str, "VGA2A DC Cal. DAC powered up"); else strcpy(str, "VGA2A DC Cal. DAC powered down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
	  memset(str, '\0', sizeof(str));
	  if(PD6_r06_b22()) strcpy(str, "VGA2A DC Cal. DAC powered down"); else strcpy(str, "VGA2A DC Cal. DAC powered up");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //PD[5]
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    if((chbPwrDCCmpA->Checked == (bool)PD5_r0E_b66_v1()) || bWrAll)
    {
      memset(str, '\0', sizeof(str));
	    out.width(32); out << "| PD[5]"; out.width(2); out << "|";	out.width(40);
      if(chbPwrDCCmpA->Checked) strcpy(str, "VGA2A DC Cal. Comp. powered up"); else strcpy(str, "VGA2A DC Cal. Comp. powered down");
      out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
    	memset(str, '\0', sizeof(str));
  	  if(PD5_r0E_b66_v1()) strcpy(str, "VGA2A DC Cal. Comp. powered down"); else strcpy(str, "VGA2A DC Cal. Comp. powered up");
      out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
      bRez = false;
    };
  }
  else
  {
    if((chbPwrDCCmpA->Checked == (bool)PD5_r06_b11_v0()) || bWrAll)
    {
      memset(str, '\0', sizeof(str));
	    out.width(32); out << "| PD[5]"; out.width(2); out << "|";	out.width(40);
      if(chbPwrDCCmpA->Checked) strcpy(str, "VGA2A DC Cal. Comp. powered up"); else strcpy(str, "VGA2A DC Cal. Comp. powered down");
      out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
    	memset(str, '\0', sizeof(str));
  	  if(PD5_r06_b11_v0()) strcpy(str, "VGA2A DC Cal. Comp. powered down"); else strcpy(str, "VGA2A DC Cal. Comp. powered up");
      out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
      bRez = false;
    };
  };

  //PD[4]
  if((chbPwrBG->Checked == (bool)PD4_r06_b00()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| PD[4]"; out.width(2); out << "|";	out.width(40);
    if(chbPwrBG->Checked) strcpy(str, "Band Gap powered up"); else strcpy(str, "Band Gap powered down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	memset(str, '\0', sizeof(str));
	  if(PD4_r06_b00()) strcpy(str, "Band Gap powered down"); else strcpy(str, "Band Gap powered up");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //PD[3]
  if((chbPwrBypAB->Checked == (bool)PD3_r07_b33()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| PD[3]"; out.width(2); out << "|";	out.width(40);
    if(chbPwrBypAB->Checked) strcpy(str, "Bypass Both VGAs powered up"); else strcpy(str, "Bypass Both VGAs powered down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	memset(str, '\0', sizeof(str));
	  if(PD3_r07_b33()) strcpy(str, "Bypass Both VGAs powered down"); else strcpy(str, "Bypass Both VGAs powered up");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //PD[2]
  if((chbPwrBypB->Checked == (bool)PD2_r07_b22()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| PD[2]"; out.width(2); out << "|";	out.width(40);
    if(chbPwrBypB->Checked) strcpy(str, "Bypass VGA2B powered up"); else strcpy(str, "Bypass VGA2B powered down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	memset(str, '\0', sizeof(str));
	  if(PD2_r07_b22()) strcpy(str, "Bypass VGA2B powered down"); else strcpy(str, "Bypass VGA2B powered up");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //PD[1]
  if((chbPwrBypA->Checked == (bool)PD1_r07_b11()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| PD[1]"; out.width(2); out << "|";	out.width(40);
    if(chbPwrBypA->Checked) strcpy(str, "Bypass VGA2A powered up"); else strcpy(str, "Bypass VGA2A powered down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	memset(str, '\0', sizeof(str));
	  if(PD1_r07_b11()) strcpy(str, "Bypass VGA2A powered down"); else strcpy(str, "Bypass VGA2A powered up");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //PD[0]
  if((chbPwrCurrRef->Checked == (bool)PD0_r07_b00()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| PD[0]"; out.width(2); out << "|";	out.width(40);
    if(chbPwrCurrRef->Checked) strcpy(str, "Current Reference powered up"); else strcpy(str, "Current Reference powered down");
    out << str;	out.width(10); out << ""; out.width(2); out << "|";	out.width(40);
  	memset(str, '\0', sizeof(str));
	  if(PD0_r07_b00()) strcpy(str, "Current Reference powered down"); else strcpy(str, "Current Reference powered up");
    out << str;	out.width(10); out << ""; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //VGA2GAINB[3:0]
  if((cmbVga2GB_t->ItemIndex != VGA2GAINB_r08_b74()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| VGA2GAINB[3:0]"; out.width(2); out << "|";	out.width(40);
    strcpy(str, cmbVga2GB_t->Items->Strings[cmbVga2GB_t->ItemIndex].c_str());
    out << str;	out.width(10); out << "dB"; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
	  itmp = VGA2GAINB_r08_b74();
    if(itmp < 11) strcpy(str, cmbVga2GB_t->Items->Strings[itmp].c_str());
    else strcpy(str, "Wrong Data");
    out << str;	out.width(10); out << "dB"; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  //VGA2GAINA[3:0]
  if((cmbVga2GA_t->ItemIndex != VGA2GAINA_r08_b30()) || bWrAll)
  {
    memset(str, '\0', sizeof(str));
	  out.width(32); out << "| VGA2GAINA[3:0]"; out.width(2); out << "|";	out.width(40);
    strcpy(str, cmbVga2GA_t->Items->Strings[cmbVga2GA_t->ItemIndex].c_str());
    out << str;	out.width(10); out << "dB"; out.width(2); out << "|";	out.width(40);
    memset(str, '\0', sizeof(str));
	  itmp = VGA2GAINA_r08_b30();
    if(itmp < 11) strcpy(str, cmbVga2GA_t->Items->Strings[itmp].c_str());
    else strcpy(str, "Wrong Data");
    out << str;	out.width(10); out << "dB"; out.width(1); out << "|"; out << endl;
    bRez = false;
  };

  if(bRez && (!bWrAll))
  {
  	out.width(136); out << "| CHIP Configuration EQUALS GUI Configuration"; out.width(2); out << "|";
	  out << endl;
  };
	out << "-----------------------------------------------------------------------------------------------------------------------------------------" << endl;
	out << "" << endl;
	out << "" << endl;

  return bRez;
};

//----------------------------------------------------------------------------
// Writes Register - Value format to the file.
//----------------------------------------------------------------------------
void TfrmRxVGA2::MakeRVFFile(ofstream &out)
{
  int iRval;
  int iAddr = (((1 << 3) | m_cMAddr) << 4) << 8;

  //*** To create file from GUI data instead of chip registers data. Start ***//
  MakeData(0xFF);
  memcpy(m_psD.cDataR, m_psD.cDataW, m_psD.iLen);
  //***  To create file from GUI data instead of chip registers data. End  ***//

  //0x00 - Read Only

  //0x01 - Read Only

  //0x02
  iRval = (iAddr | (0x02 << 8)) | (m_psD.cDataR[0x02] & 0x3F);
  out << iRval; out << endl;

  //0x03
  iRval = (iAddr | (0x03 << 8)) | (m_psD.cDataR[0x03] & 0x3F);
  out << iRval; out << endl;

  //0x04
  iRval = (iAddr | (0x04 << 8)) | (m_psD.cDataR[0x04] & 0x3F);
  out << iRval; out << endl;

  //0x05
  iRval = (iAddr | (0x05 << 8)) | (m_psD.cDataR[0x05] & 0x1F);
  out << iRval; out << endl;

  //0x06
  //To resolove Ver=2 Rev=2 Mask=1 version
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    iRval = (iAddr | (0x06 << 8)) | (m_psD.cDataR[0x06] & 0x35);
  }
  else
  {
    iRval = (iAddr | (0x06 << 8)) | (m_psD.cDataR[0x06] & 0x3F);
  };
  out << iRval; out << endl;
  
  //0x07
  iRval = (iAddr | (0x07 << 8)) | (m_psD.cDataR[0x07] & 0x0F);
  out << iRval; out << endl;

  //0x08
  iRval = (iAddr | (0x08 << 8)) | (m_psD.cDataR[0x08] & 0xFF);
  out << iRval; out << endl;

  //0x0E
  //To resolove Ver=2 Rev=2 Mask=1 version
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    iRval = (iAddr | (0x0E << 8)) | (m_psD.cDataR[0x0E] & 0xC0);
    out << iRval; out << endl;
  }
};

//----------------------------------------------------------------------------
// Saves chip configuration using FRIF format
//----------------------------------------------------------------------------
void TfrmRxVGA2::MakeRFIFFile(ofstream &out, char *FRIF_SPI_A)
{
  int iRval;
  int iAddr = (((1 << 3) | m_cMAddr) << 4) << 8;

  //*** To create file from GUI data instead of chip registers data. Start ***//
  MakeData(0xFF);
  memcpy(m_psD.cDataR, m_psD.cDataW, m_psD.iLen);
  //***  To create file from GUI data instead of chip registers data. End  ***//


  out << "echo \"Now programming the Lime  Rx VGA2\"" << endl << endl;

  //0x00 - Read Only

  //0x01 - Read Only

  //0x02
  //out << "# Register(s): DC_CNTVAL[5:0]" << endl;
  iRval = (iAddr | (0x02 << 8)) | (m_psD.cDataR[0x02] & 0x3F);
  out << FRIF_DIR_W;
  //out << hex << uppercase << showbase << iRval;
  out << "0x" << hex << uppercase << noshowbase << iRval;
  out << FRIF_SPI_A; out << endl;

  //0x03
  //out << "# Register(s): DC_START_CLBR, DC_ADDR, DC_LOAD, DC_SRESET, DC_ADDR[2:0]" << endl;
  iRval = (iAddr | (0x03 << 8)) | (m_psD.cDataR[0x03] & 0x3F);
  out << FRIF_DIR_W;
  //out << hex << uppercase << showbase << iRval;
  out << "0x" << hex << uppercase << noshowbase << iRval;
  out << FRIF_SPI_A; out << endl;

  //0x04
  //out << "# Register(s): VCM[3:0], EN, DECODE" << endl;
  iRval = (iAddr | (0x04 << 8)) | (m_psD.cDataR[0x04] & 0x3F);
  out << FRIF_DIR_W;
  //out << hex << uppercase << showbase << iRval;
  out << "0x" << hex << uppercase << noshowbase << iRval;
  out << FRIF_SPI_A; out << endl;

  //0x05
  //out << "# Register(s): VGA2GAIN[4:0]" << endl;
  iRval = (iAddr | (0x05 << 8)) | (m_psD.cDataR[0x05] & 0x1F);
  out << FRIF_DIR_W;
  //out << hex << uppercase << showbase << iRval;
  out << "0x" << hex << uppercase << noshowbase << iRval;
  out << FRIF_SPI_A; out << endl;

  //0x06
  //out << "# Register(s): PD[9:4]" << endl;
  //To resolove Ver=2 Rev=2 Mask=1 version
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    iRval = (iAddr | (0x06 << 8)) | (m_psD.cDataR[0x06] & 0x35);
  }
  else
  {
    iRval = (iAddr | (0x06 << 8)) | (m_psD.cDataR[0x06] & 0x3F);
  };
  out << FRIF_DIR_W;
  //out << hex << uppercase << showbase << iRval;
  out << "0x" << hex << uppercase << noshowbase << iRval;
  out << FRIF_SPI_A; out << endl;

  //0x07
  //out << "# Register(s): PD[3:0]" << endl;
  iRval = (iAddr | (0x07 << 8)) | (m_psD.cDataR[0x07] & 0x0F);
  out << FRIF_DIR_W;
  //out << hex << uppercase << showbase << iRval;
  out << "0x" << hex << uppercase << noshowbase << iRval;
  out << FRIF_SPI_A; out << endl;

  //0x08
  //out << "# Register(s): VGA2GAINB, VGA2GAINA" << endl;
  iRval = (iAddr | (0x08 << 8)) | (m_psD.cDataR[0x08] & 0xFF);
  out << FRIF_DIR_W;
  //out << hex << uppercase << showbase << iRval;
  out << "0x" << hex << uppercase << noshowbase << iRval;
  out << FRIF_SPI_A; out << endl;

  //To resolove Ver=2 Rev=2 Mask=1 version
  //out << "# Register(s): PD[7], PD[5]" << endl;
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    iRval = (iAddr | (0x0E << 8)) | (m_psD.cDataR[0x0E] & 0xC0);
  }
  out << FRIF_DIR_W;
  //out << hex << uppercase << showbase << iRval;
  out << "0x" << hex << uppercase << noshowbase << iRval;
  out << FRIF_SPI_A; out << endl;

  out << endl << "echo \"Rx VGA2 programming complete\"" << endl << endl;
};



//---------------------------------------------------------------------------
// Sets GUI under data from chip
//---------------------------------------------------------------------------
bool TfrmRxVGA2::SetGUIUnderReadback()
{
  int itmp;
  bool bRez = true;

  m_bAllowSend = false;

  //DC_REGVAL[5:0]
  itmp = DC_REGVAL_r00_b50();
  lbDC_REGVAL->Caption = itmp;

  //DC_LOCK[2:0]
  itmp = DC_LOCK_r01_b42();
  lblDC_LOCK->Caption = itmp;

  //DC_CLBR_DONE
  itmp = DC_CLBR_DONE_r01_b11();
  lblDC_CLBR_DONE->Caption = itmp;

  //DC_UD
  itmp = DC_UD_r01_b00();
  lblDC_UD->Caption = itmp;

  //DC_CNTVAL[5:0]
  itmp = DC_CNTVAL_r02_b50();
  cmbCalVal->ItemIndex = itmp;

  //DC_START_CLBR
  itmp = DC_START_CLBR_r03_b55();

  //DC_LOAD
  itmp = DC_LOAD_r03_b44();

  //DC_SRESET
  itmp = DC_SRESET_r03_b33();

  //DC_ADDR[2:0]
  itmp = DC_ADDR_r03_b20();
  if(itmp > 4) {itmp = 4; bRez = false;}
  cmbDCCalAddr->ItemIndex = itmp;

  //VCM[3:0]
  itmp = VCM_r04_b52();
  cmbVCM->ItemIndex = CMB2VCM(itmp);

  //EN
  itmp = EN_r04_b11();
  chbPwrVGA2Mods->Checked = itmp;

  //DECODE
  itmp = DECODE_r04_b00();
  rgrDecode->ItemIndex = itmp;

  //VGA2GAIN[4:0]
  itmp = VGA2GAIN_r05_b40();
  if(itmp > 20) { itmp = 20; bRez = false;}
  cmbVga2G_u->ItemIndex = itmp;

  //PD[9]
  itmp = PD9_r06_b55();
  chbPwrDCCurrR->Checked = itmp;
  chbPwrDCCurrR->Checked = !chbPwrDCCurrR->Checked;

  //PD[8]
  itmp = PD8_r06_b44();
  chbPwrDCDACB->Checked = itmp;
  chbPwrDCDACB->Checked = !chbPwrDCDACB->Checked;

  //PD[7]
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    itmp = PD7_r0E_b77_v1();
  }
  else
  {
    itmp = PD7_r06_b33_v0();
  };
  chbPwrDCCmpB->Checked = itmp;
  chbPwrDCCmpB->Checked = !chbPwrDCCmpB->Checked;

  //PD[6]
  itmp = PD6_r06_b22();
  chbPwrDCDACA->Checked = itmp;
  chbPwrDCDACA->Checked = !chbPwrDCDACA->Checked;

  //PD[5]
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    itmp = PD5_r0E_b66_v1();
  }
  else
  {
    itmp = PD5_r06_b11_v0();
  };
  chbPwrDCCmpA->Checked = itmp;
  chbPwrDCCmpA->Checked = !chbPwrDCCmpA->Checked;

  //PD[4]
  itmp = PD4_r06_b00();
  chbPwrBG->Checked = itmp;
  chbPwrBG->Checked = !chbPwrBG->Checked;

  //PD[3]
  itmp =PD3_r07_b33();
  chbPwrBypAB->Checked = itmp;
  chbPwrBypAB->Checked = !chbPwrBypAB->Checked;

  //PD[2]
  itmp = PD2_r07_b22();
  chbPwrBypB->Checked = itmp;
  chbPwrBypB->Checked = !chbPwrBypB->Checked;

  //PD[1]
  itmp = PD1_r07_b11();
  chbPwrBypA->Checked = itmp;
  chbPwrBypA->Checked = !chbPwrBypA->Checked;

  //PD[0]
  itmp = PD0_r07_b00();
  chbPwrCurrRef->Checked = itmp;
  chbPwrCurrRef->Checked = !chbPwrCurrRef->Checked;

  //VGA2GAINB[3:0]
  itmp = VGA2GAINB_r08_b74();
  if(itmp > 10) { itmp = 10; bRez = false;}
  cmbVga2GB_t->ItemIndex = itmp;

  //VGA2GAINA[3:0]
  itmp = VGA2GAINA_r08_b30();
  if(itmp > 10) { itmp = 10; bRez = false;}
  cmbVga2GA_t->ItemIndex = itmp;

  m_bAllowSend = true;
  return bRez;
};

//----------------------------------------------------------------------------
// //Writes register map to the file. According to the customer wish.
//----------------------------------------------------------------------------
void TfrmRxVGA2::MakeRegisterFile(ofstream &out)
{
  int itmp;

  //*** To create file from GUI data instead of chip registers data. Start ***//
  MakeData(0xFF);
  memcpy(m_psD.cDataR, m_psD.cDataW, m_psD.iLen);
  //***  To create file from GUI data instead of chip registers data. End  ***//


	//DC_REGVAL[5:0]
  out.width(24); out << left; out << "REG 0x60 MASK 05 : 00  =";
  itmp = DC_REGVAL_r00_b50();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_REGVAL"; out << endl;

  //DC_LOCK[2:0]
  out.width(24); out << left; out << "REG 0x61 MASK 04 : 02  =";
  itmp = DC_LOCK_r01_b42();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_LOCK"; out << endl;

  //DC_CLBR_DONE
  out.width(24); out << left; out << "REG 0x61 MASK 01 : 01  =";
  itmp = DC_CLBR_DONE_r01_b11();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_CLB_DONE"; out << endl;

  //DC_UD
  out.width(24); out << left; out << "REG 0x61 MASK 00 : 00  =";
  itmp = DC_UD_r01_b00();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_UD"; out << endl;

  //DC_CNTVAL[5:0]
  out.width(24); out << left; out << "REG 0x62 MASK 05 : 00  =";
  itmp = DC_CNTVAL_r02_b50();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_CNTVAL"; out << endl;

  //DC_START_CLBR
  out.width(24); out << left; out << "REG 0x63 MASK 05 : 05  =";
  itmp = DC_START_CLBR_r03_b55();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_START_CLBR"; out << endl;

  //DC_LOAD
  out.width(24); out << left; out << "REG 0x63 MASK 04 : 04  =";
  itmp = DC_LOAD_r03_b44();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_LOAD"; out << endl;

  //DC_SRESET
  out.width(24); out << left; out << "REG 0x63 MASK 03 : 03  =";
  itmp = DC_SRESET_r03_b33();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_SRESET"; out << endl;

  //DC_ADDR[2:0]
  out.width(24); out << left; out << "REG 0x63 MASK 02 : 00  =";
  itmp = DC_ADDR_r03_b20();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DC_ADDR"; out << endl;

  //VCM[3:0]
  out.width(24); out << left; out << "REG 0x64 MASK 05 : 02  =";
  itmp = VCM_r04_b52();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_VCM"; out << endl;

  //EN
  out.width(24); out << left; out << "REG 0x64 MASK 01 : 01  =";
  itmp = EN_r04_b11();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_EN_RXVGA2"; out << endl;

  //DECODE
  out.width(24); out << left; out << "REG 0x64 MASK 00 : 00  =";
  itmp = DECODE_r04_b00();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_DECODE"; out << endl;

  //VGA2GAIN[4:0]
  out.width(24); out << left; out << "REG 0x65 MASK 04 : 00  =";
  itmp = VGA2GAIN_r05_b40();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_VGA2GAIN"; out << endl;

  //PD[9]
  out.width(24); out << left; out << "REG 0x66 MASK 05 : 05  =";
  itmp = PD9_r06_b55();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD9_DC_CURR_REG"; out << endl;

  //PD[8]
  out.width(24); out << left; out << "REG 0x66 MASK 04 : 04  =";
  itmp = PD8_r06_b44();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD8_VGA2B_DC_CAL_DAC"; out << endl;

  //PD[7]
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    out.width(24); out << left; out << "REG 0x6E MASK 07 : 07  =";
    itmp = PD7_r0E_b77_v1();
  }
  else
  {
    out.width(24); out << left; out << "REG 0x66 MASK 03 : 03  =";
    itmp = PD7_r06_b33_v0();
  };
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD7_VGA2B_DC_CAL_COMP"; out << endl;

  //PD[6]
  out.width(24); out << left; out << "REG 0x66 MASK 02 : 02  =";
  itmp = PD6_r06_b22();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD6_VGA2A_DC_CAL_DAC"; out << endl;

  //PD[5]
  if((m_cVer == 2) && (m_cRev == 2) && (m_cMask == 1))
  {
    out.width(24); out << left; out << "REG 0x6E MASK 06 : 06  =";
    itmp = PD5_r0E_b66_v1();
  }
  else
  {
    out.width(24); out << left; out << "REG 0x66 MASK 01 : 01  =";
    itmp = PD5_r06_b11_v0();
  };
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD5_VGA2A_DC_CAL_COMP"; out << endl;

  //PD[4]
  out.width(24); out << left; out << "REG 0x66 MASK 00 : 00  =";
  itmp = PD4_r06_b00();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD4_BANDGAP"; out << endl;

  //PD[3]
  out.width(24); out << left; out << "REG 0x67 MASK 03 : 03  =";
  itmp = PD3_r07_b33();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD3_RXVGA_BYP"; out << endl;

  //PD[2]
  out.width(24); out << left; out << "REG 0x67 MASK 02 : 02  =";
  itmp = PD2_r07_b22();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD2_RXVGA2B_BYP"; out << endl;

  //PD[1]
  out.width(24); out << left; out << "REG 0x67 MASK 01 : 01  =";
  itmp = PD1_r07_b11();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD1_RXVGA2A_BYP"; out << endl;

  //PD[0]
  out.width(24); out << left; out << "REG 0x67 MASK 00 : 00  =";
  itmp = PD0_r07_b00();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_PD0_CURR_REF"; out << endl;

  //VGA2GAINB[3:0]
  out.width(24); out << left; out << "REG 0x68 MASK 07 : 04  =";
  itmp = VGA2GAINB_r08_b74();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_VGA2GAINB"; out << endl;

  //VGA2GAINA[3:0]
  out.width(24); out << left; out << "REG 0x68 MASK 03 : 00  =";
  itmp = VGA2GAINA_r08_b30();
  out.width(6); out << right; out << itmp;
  out << "  RXVGA2_VGA2GAINA"; out << endl;
}

//---------------------------------------------------------------------------
void TfrmRxVGA2::UpdateFormData()
{
	int itmp;
  String str;
  
	//DC_REGVAL[5:0]
  itmp = DC_REGVAL_r00_b50();
  lbDC_REGVAL->Caption = itmp;

  //DC_LOCK[2:0]
	itmp = DC_LOCK_r01_b42();
  lblDC_LOCK->Caption = itmp;

  //DC_CLBR_DONE
	if(DC_CLBR_DONE_r01_b11()) str = "In Progress"; else str = "Done";
  lblDC_CLBR_DONE->Caption = str;

  //DC_UD
	if(DC_UD_r01_b00()) str = "Up"; else str = "Down";
  lblDC_UD->Caption = str;
};

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void TfrmRxVGA2::SaveConf(TIniFile *pini)
{
	String Sect = (int)m_cMAddr;
	pini->WriteInteger(Sect, "cmbDCCalAddr", cmbDCCalAddr->ItemIndex);
  pini->WriteInteger(Sect, "cmbCalVal", cmbCalVal->ItemIndex);
  pini->WriteInteger(Sect, "dc_ref", GetCalVal(0x00));
  pini->WriteInteger(Sect, "dc2a_I", GetCalVal(0x01));
  pini->WriteInteger(Sect, "dc2a_Q", GetCalVal(0x02));
  pini->WriteInteger(Sect, "dc2b_I", GetCalVal(0x03));
  pini->WriteInteger(Sect, "dc2b_Q", GetCalVal(0x04));
  

  pini->WriteInteger(Sect, "cmbVCM", cmbVCM->ItemIndex);
  pini->WriteBool(Sect, "chbPwrVGA2Mods", chbPwrVGA2Mods->Checked);
  pini->WriteInteger(Sect, "rgrDecode", rgrDecode->ItemIndex);

  pini->WriteInteger(Sect, "cmbVga2G_u", cmbVga2G_u->ItemIndex);

  pini->WriteBool(Sect, "chbPwrDCCurrR", chbPwrDCCurrR->Checked);
  pini->WriteBool(Sect, "chbPwrDCDACB", chbPwrDCDACB->Checked);
  pini->WriteBool(Sect, "chbPwrDCCmpB", chbPwrDCCmpB->Checked);
  pini->WriteBool(Sect, "chbPwrDCDACA", chbPwrDCDACA->Checked);
  pini->WriteBool(Sect, "chbPwrDCCmpA", chbPwrDCCmpA->Checked);
  pini->WriteBool(Sect, "chbPwrBG", chbPwrBG->Checked);
  pini->WriteBool(Sect, "chbPwrBypAB", chbPwrBypAB->Checked);
  pini->WriteBool(Sect, "chbPwrBypB", chbPwrBypB->Checked);
  pini->WriteBool(Sect, "chbPwrBypA", chbPwrBypA->Checked);
  pini->WriteBool(Sect, "chbPwrCurrRef", chbPwrCurrRef->Checked);

  pini->WriteInteger(Sect, "cmbVga2GB_t", cmbVga2GB_t->ItemIndex);
  pini->WriteInteger(Sect, "cmbVga2GA_t", cmbVga2GA_t->ItemIndex);
};

//---------------------------------------------------------------------------
void TfrmRxVGA2::ReadConf(TIniFile *pini)
{
	String Sect = (int)m_cMAddr;
  int iTmp;

  m_bAllowSend = false;

	cmbDCCalAddr->ItemIndex = pini->ReadInteger(Sect, "cmbDCCalAddr", 0);
  cmbCalVal->ItemIndex = pini->ReadInteger(Sect, "cmbCalVal", 31);
  m_DCClbrV[0] = pini->ReadInteger(Sect, "dc_ref", 12);
  m_DCClbrV[1] = pini->ReadInteger(Sect, "dc2a_I", 31);
  m_DCClbrV[2] = pini->ReadInteger(Sect, "dc2a_Q", 31);
  m_DCClbrV[3] = pini->ReadInteger(Sect, "dc2b_I", 31);
  m_DCClbrV[4] = pini->ReadInteger(Sect, "dc2b_Q", 31);


  cmbVCM->ItemIndex = pini->ReadInteger(Sect, "cmbVCM", 7);
  chbPwrVGA2Mods->Checked = pini->ReadBool(Sect, "chbPwrVGA2Mods", 1);
  rgrDecode->ItemIndex = pini->ReadInteger(Sect, "rgrDecode", 0);

  cmbVga2G_u->ItemIndex = pini->ReadInteger(Sect, "cmbVga2G_u", 1);

  chbPwrDCCurrR->Checked = pini->ReadBool(Sect, "chbPwrDCCurrR", 1);
  chbPwrDCDACB->Checked = pini->ReadBool(Sect, "chbPwrDCDACB", 1);
  chbPwrDCCmpB->Checked = pini->ReadBool(Sect, "chbPwrDCCmpB", 1);
  chbPwrDCDACA->Checked = pini->ReadBool(Sect, "chbPwrDCDACA", 1);
  chbPwrDCCmpA->Checked = pini->ReadBool(Sect, "chbPwrDCCmpA", 1);
  chbPwrBG->Checked = pini->ReadBool(Sect, "chbPwrBG", 1);
  chbPwrBypAB->Checked = pini->ReadBool(Sect, "chbPwrBypAB", 1);
  chbPwrBypB->Checked = pini->ReadBool(Sect, "chbPwrBypB", 1);
  chbPwrBypA->Checked = pini->ReadBool(Sect, "chbPwrBypA", 1);
  chbPwrCurrRef->Checked = pini->ReadBool(Sect, "chbPwrCurrRef", 1);
  
  cmbVga2GB_t->ItemIndex = pini->ReadInteger(Sect, "cmbVga2GB_t", 0);
  cmbVga2GA_t->ItemIndex = pini->ReadInteger(Sect, "cmbVga2GA_t", 1);

  m_bAllowSend = true;
};

//---------------------------------------------------------------------------
// Read DC Calibration value
//---------------------------------------------------------------------------
int TfrmRxVGA2::GetCalVal(int Addr)
{
	char btmp;

  // Change address to Addr
	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_LOAD
  btmp = 0x00;	//Set Load Value Inactive
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)Addr;
  m_psD.cDataW[0x03] |= btmp;

	m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

  //Read SPI configuration
  SendMessage(G_hwWindow, CM_READ, 0, m_cMAddr);

  //Return value
	//DC_REGVAL[5:0]
  return (m_psD.cDataR[0x00] & 0x3F);
};

//---------------------------------------------------------------------------
// Write DC Calibration value
//---------------------------------------------------------------------------
void TfrmRxVGA2::SetCalVal(int Addr, int Val)
{
  char btmp;
  
  // Change address to Addr
	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_START_CLBR
  btmp = 0x00;	//Set Load Value Inactive
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)Addr;
  m_psD.cDataW[0x03] |= btmp;

	m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

  //Write Value to DC_CNTVAL[5:0] register
  //======= register addr 0x02 =======
  m_psD.cDataW[0x02] = 0x00;
  btmp = (char)Val;
  m_psD.cDataW[0x02] |= btmp;

	m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x02;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

  //Perform Load Operation
	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_LOAD
  btmp = 0x01;	//Load Value Active
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)Addr;
  m_psD.cDataW[0x03] |= btmp;

	m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr); 

	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_LOAD
  btmp = 0x00;	//Load Value Inactive
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)Addr;
  m_psD.cDataW[0x03] |= btmp;

  m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

};

//---------------------------------------------------------------------------
// Set calibration chain address
//---------------------------------------------------------------------------
void TfrmRxVGA2::SetCalAddr(int Addr)
{
  char btmp;
  
  // Change address to Addr
	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_START_CLBR
  btmp = 0x00;	//Set Start Calibration Inactive
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_START_CLBR
  btmp = 0x00;	//Set Load Value Inactive
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  btmp = (char)Addr;
  m_psD.cDataW[0x03] |= btmp;

	m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);
};

//---------------------------------------------------------------------------
// Activate start calibration command and the deactivate.
//---------------------------------------------------------------------------
void TfrmRxVGA2::StartCalibration(int Addr)
{
  //Enable DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKONMASK, 0, 0x10); };


	char btmp;
	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_LOAD
  btmp = 0x01;	//Start Calibration
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_LOAD
  btmp = 0x00;	//Load Value Inactive
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  //btmp = (char)cmbDCCalAddr->ItemIndex;
  btmp = (char)Addr;
  m_psD.cDataW[0x03] |= btmp;

	m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

	//======= register addr 0x03 =======
	m_psD.cDataW[0x03] = 0x00;
  //DC_LOAD
  btmp = 0x00;	//Deactivate Start Calibration
  btmp = btmp << 5; m_psD.cDataW[0x03] |= btmp;
  //DC_LOAD
  btmp = 0x00;	//Load Value Inactive
  btmp = btmp << 4; m_psD.cDataW[0x03] |= btmp;
  //DC_SRESET
  btmp = 0x01;	//Set Reset Inactive
  btmp = btmp << 3; m_psD.cDataW[0x03] |= btmp;
	//DC_ADDR
  //btmp = (char)cmbDCCalAddr->ItemIndex;
  btmp = (char)Addr;
  m_psD.cDataW[0x03] |= btmp;

  m_psD.iToW = 1;
  m_psD.iWInd[0] = 0x03;
  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);

  //m_DCClbrV[cmbDCCalAddr->ItemIndex] = GetCalVal(cmbDCCalAddr->ItemIndex);
  m_DCClbrV[Addr] = GetCalVal(Addr);

  //Restore DC CAL clock
  //if(m_bAllowSend){ SendMessage(G_hwWindow, CM_SPICLKRESTORE, 0, 0); };
};

//---------------------------------------------------------------------------
// Returns DC Calibration Chain status (address 0x1)
// Address of DC Calibration Chain should be settled before.
//---------------------------------------------------------------------------
char TfrmRxVGA2::GetCalStatus()
{
  //Read SPI configuration
  SendMessage(G_hwWindow, CM_READ, 0, m_cMAddr);

  //Return value
	//DC_REGVAL[5:0]
  return m_psD.cDataR[0x01];
};

//---------------------------------------------------------------------------
// Set DC Calibration values in the Class to defaults
//---------------------------------------------------------------------------
void TfrmRxVGA2::SetCalDefaults()
{
  m_DCClbrV[0] = 31;
  m_DCClbrV[1] = 31;
  m_DCClbrV[2] = 31;
  m_DCClbrV[3] = 31;
  m_DCClbrV[4] = 31;
};

//---------------------------------------------------------------------------
// Write DC Calibration value
//---------------------------------------------------------------------------
void TfrmRxVGA2::DownloadAllDCCalValues()
{
  SetCalVal(0x00, m_DCClbrV[0]);
  SetCalVal(0x01, m_DCClbrV[1]);
  SetCalVal(0x02, m_DCClbrV[2]);
  SetCalVal(0x03, m_DCClbrV[3]);
  SetCalVal(0x04, m_DCClbrV[4]);
};


//---------------------------------------------------------------------------
// Functions for customer mode interface
//---------------------------------------------------------------------------
void TfrmRxVGA2::CustSet_RxVGA2PowerON()
{
  m_bAllowSend = false;
  chbPwrVGA2Mods->Checked = true;
  rgrDecode->ItemIndex = 0;
  m_bAllowSend = true;

  MakeData(0x01); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);
};

void TfrmRxVGA2::CustSet_RxVGA2PowerOFF()
{
  m_bAllowSend = false;
  chbPwrVGA2Mods->Checked = false;
  rgrDecode->ItemIndex = 0;
  m_bAllowSend = true;

  MakeData(0x01); SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);
};

void TfrmRxVGA2::CustSet_cmbVCM(int Ind)
{
  //cmbVCM->ItemIndex = Ind;
  cmbVCM->ItemIndex = VCM2CMB(Ind);
  cmbVCMChange(Application);
};

//VCM code to combobox index recalculation
int TfrmRxVGA2::VCM2CMB(int code)
{
  if(code <= 7)
  {
    return code;
  }
  else
  {
    return 22 - code;
  };
};

//
int TfrmRxVGA2::CMB2VCM(int cmb)
{
  return VCM2CMB(cmb);
};


//---------------------------------------------------------------------------
// Functions for register testing
//---------------------------------------------------------------------------
void TfrmRxVGA2::RT_SetTestMask(char Mask)
{
  MakeWriteAll();

	for(int i=0; i<m_psD.iToW; i++)
	{
    m_psD.cDataW[m_psD.iWInd[i]] = Mask & m_psD.cTestMap[m_psD.iWInd[i]];
  };

  SendMessage(G_hwWindow, CM_AUTODOWNLOAD, 0, m_cMAddr);
};


//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
int TfrmRxVGA2::GetRXVGA2Gain()
{
  return cmbVga2G_u->ItemIndex;
};

void TfrmRxVGA2::SetRXVGA2Gain(int Ind)
{
  cmbVga2G_u->ItemIndex = Ind;
};
