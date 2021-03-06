// -----------------------------------------------------------------------------
// FILE: 		    dlgSerPort.cpp
// DESCRIPTION:	Serial port control routines
// DATE:
// AUTHOR(s):	  Lime Microsystems
// REVISIONS:
// -----------------------------------------------------------------------------

//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "dlgSerPort.h"
#include "common.h"
#include <Registry.hpp>

//Comands
//I2C FPGA
#define CFG_ADDR 0xAA
#define CMD_CFG_I2C_WR 0x14
#define CMD_CFG_I2C_RD 0x15

//LMS6002D
#define CMD_LMS_WR 0x16
#define CMD_LMS_RD 0x17

//I2C Si5356A
#define FSN_ADDR 0xE0
#define CMD_FSN_I2C_WR 0x24
#define CMD_FSN_I2C_RD 0x25

#define USB_BUFFER_LENGTH 64

//--------------------------------------------------------------------- 
#pragma resource "*.dfm"
TdSerPort *dSerPort;
//---------------------------------------------------------------------
__fastcall TdSerPort::TdSerPort(TComponent* AOwner)
	: TForm(AOwner)
{


  hComm = INVALID_HANDLE_VALUE;
  m_sComName = "COM1";
  m_iBaud = 0;


   //=====USB=======
  iPortSelected = 0; //default - COM Port

  USBDevice = new CCyUSBDevice(NULL);
  if(USBDevice->DeviceCount() > 0)
  {
        rgrPortSelection->ItemIndex = 1;
        iPortSelected = 1;
  }

  findUSBDevices();  
  //================


  LoadFromIni();
  OpenComPort(m_sComName, Code2Baud(m_iBaud));
}

//---------------------------------------------------------------------
void __fastcall TdSerPort::FormDestroy(TObject *Sender)
{
  //SaveToIni();
  CloseUSBDevice();
  CloseComPort();
}

//---------------------------------------------------------------------------
void __fastcall TdSerPort::CloseComPort()
{
  if(hComm != INVALID_HANDLE_VALUE)
  {
    SetCommTimeouts(hComm, &m_ctmoOld);
    CloseHandle(hComm);
  };
  hComm = INVALID_HANDLE_VALUE;
};

//---------------------------------------------------------------------------
bool __fastcall TdSerPort::OpenComPort(String sPortName, int iBaudRate)
{
  int erreur = 0; 

  //Close port if open
  CloseComPort();

  //Initialize Overlap structures
  m_osROverlap.Internal = 0;
  m_osROverlap.InternalHigh = 0;
  m_osROverlap.Offset = 0;
  m_osROverlap.OffsetHigh = 0;
  m_osROverlap.hEvent = CreateEvent(NULL, false, false, NULL);

  m_osWOverlap.Internal = 0;
  m_osWOverlap.InternalHigh = 0;
  m_osWOverlap.Offset = 0;
  m_osWOverlap.OffsetHigh = 0;
  m_osWOverlap.hEvent = CreateEvent(NULL, false, false, NULL);

  //Initialize DSB structure
  memset(&m_dcbCommPort, 0, sizeof(m_dcbCommPort));
  m_dcbCommPort.BaudRate = iBaudRate;
  m_dcbCommPort.fBinary = 1;
  m_dcbCommPort.fParity = 0;
  m_dcbCommPort.fOutxCtsFlow = 0;
  m_dcbCommPort.fOutxDsrFlow = 0;
  m_dcbCommPort.fDtrControl = 1;
  m_dcbCommPort.fDsrSensitivity = 0;
  m_dcbCommPort.fTXContinueOnXoff = 0;
  m_dcbCommPort.fOutX = 0;
  m_dcbCommPort.fInX = 0;
  m_dcbCommPort.fErrorChar = 0;
  m_dcbCommPort.fNull = 0;
  m_dcbCommPort.fRtsControl = 0;
  m_dcbCommPort.fAbortOnError = 0;
  m_dcbCommPort.fDummy2 = 0;
  //m_dcbCommPort.wReserved = 0;
  m_dcbCommPort.XonLim = 512;
  m_dcbCommPort.XoffLim = 512;
  m_dcbCommPort.ByteSize = 8;
  m_dcbCommPort.Parity = 0;
  m_dcbCommPort.StopBits = 0;
  m_dcbCommPort.XonChar = 17;
  m_dcbCommPort.XoffChar = 19;
  m_dcbCommPort.ErrorChar = 0;
  m_dcbCommPort.EofChar = 26;
  m_dcbCommPort.EvtChar = 0;
  m_dcbCommPort.wReserved1 = 0;
  m_dcbCommPort.DCBlength = sizeof(DCB);

  //Initialize Timeout's
  m_ctmoNew.ReadIntervalTimeout = 20;
  m_ctmoNew.ReadTotalTimeoutMultiplier = 0;
  m_ctmoNew.ReadTotalTimeoutConstant = 0;//1;
  m_ctmoNew.WriteTotalTimeoutMultiplier = 0;
  m_ctmoNew.WriteTotalTimeoutConstant = 0;


  //Open COM port
  String stmp;
  stmp = "\\\\.\\" + sPortName; 

  hComm = CreateFile(stmp.c_str(),
                      GENERIC_READ | GENERIC_WRITE,
                      0,
                      0,
                      OPEN_EXISTING,
                      FILE_FLAG_OVERLAPPED,
                      0);

  if(hComm != INVALID_HANDLE_VALUE)
  {
    //Set Events
    if(!SetCommMask(hComm, 0))
      erreur = 1;

    //Set Timeouts
    GetCommTimeouts(hComm, &m_ctmoOld);
    if(!SetCommTimeouts(hComm, &m_ctmoNew))
      erreur = 2;

    //Set DCB
    if(!SetCommState(hComm, &m_dcbCommPort))
      erreur = 4;
  }
  else
  {
    erreur = 8;
  };

  //Check the results
  if(erreur != 0)
  {
    CloseHandle(hComm);
    hComm = INVALID_HANDLE_VALUE;
    return false;
  }
  else
  {
    PurgeComm(hComm, PURGE_TXCLEAR|PURGE_RXCLEAR);
    return true;
  };
};

//---------------------------------------------------------------------------
void __fastcall TdSerPort::FormClose(TObject *Sender, TCloseAction &Action)
{
  if(ModalResult == mrOk)
  {
    m_sComName = cmbComPorts->Items->Strings[cmbComPorts->ItemIndex];
    m_iBaud = rgrBaud->ItemIndex;
    SaveToIni();

    OpenComPort(m_sComName, Code2Baud(m_iBaud));
  };
}

//---------------------------------------------------------------------------
void TdSerPort::LoadFromIni()
{
  TIniFile *ini;
  String Str, Name;

  Str = ChangeFileExt(Application->ExeName, ".cps");
  Name = "SER";

  ini = new TIniFile(Str);
  m_sComName = ini->ReadString(Name, "Port", "COM1");
  m_iBaud = ini->ReadInteger(Name, "BaudRate", 0);
  delete ini;
};

//---------------------------------------------------------------------------
void TdSerPort::SaveToIni()
{
  TIniFile *ini;
  String Str, Name;

  Str = ChangeFileExt(Application->ExeName, ".cps");
  Name = "SER";

  ini = new TIniFile(Str);

  ini->WriteString(Name, "Port", m_sComName);
  ini->WriteInteger(Name, "BaudRate", m_iBaud);

  delete ini;
};

//---------------------------------------------------------------------------
int TdSerPort::Code2Baud(int Code)
{
  int baud;
  switch(Code)
  {
    case 1:
      baud = 14400;
    break;
    case 2:
      baud = 19200;
    break;
    case 3:
      baud = 38400;
    break;
    case 4:
      baud = 57600;
    break;
    case 5:
      baud = 115200;
    break;
    default:
      baud = 9600;
  };
  return baud;
};

//---------------------------------------------------------------------------
void __fastcall TdSerPort::FormShow(TObject *Sender)
{
  //findUSBDevices();

  int iCind;
  TRegistry *reg;

  cmbComPorts->Items->Clear();

  reg = new TRegistry();
  reg->RootKey = HKEY_LOCAL_MACHINE;
  reg->OpenKey("hardware\\devicemap\\serialcomm", false);
  reg->GetValueNames(cmbComPorts->Items);
  for(int i=0; i<cmbComPorts->Items->Count; i++)
  {
    cmbComPorts->Items->Strings[i] = reg->ReadString(cmbComPorts->Items->Strings[i]);
  };
  reg->CloseKey();
  delete reg;



  if(m_iBaud <= 5)
  {
    rgrBaud->ItemIndex = m_iBaud;
  }
  else
  {
    rgrBaud->ItemIndex = 0;
    m_iBaud = 0;
  };


  iCind = cmbComPorts->Items->IndexOf(m_sComName);
  if(iCind != -1)
  {
    cmbComPorts->ItemIndex = iCind;
  }
  else
  {
    cmbComPorts->ItemIndex = 0;
    m_sComName = cmbComPorts->Items->Strings[0];
  };
}

//---------------------------------------------------------------------------
bool TdSerPort::PortOpened()
{
  if( (hComm != INVALID_HANDLE_VALUE) || USBDevice->IsOpen() ) return true;
  else return false;
};



//---------------------------------------------------------------------------
bool __fastcall TdSerPort::WritePort(unsigned char *Buffer, int iSize)
{
  DWORD dwWriten = 0;
  bool bWRes = false;

  {
        //PurgeComm(hComm, PURGE_TXCLEAR|PURGE_RXCLEAR);

          if(!WriteFile(hComm, Buffer, iSize, &dwWriten, &m_osWOverlap))
          {
            if(GetLastError() == ERROR_IO_PENDING)
            {
              if(WaitForSingleObjectEx(m_osWOverlap.hEvent, (int) 5, true) == WAIT_TIMEOUT)
              {
                bWRes = false;
              }
              else
              {
                bWRes = true;
              };
            }
            else
            {
              bWRes = false;
            };
          }
          else
          {
            bWRes = true;
          };
  }
  return bWRes;
};

//---------------------------------------------------------------------------
bool __fastcall TdSerPort::ReadPort(LPVOID Buffer, int *iSize, int iToRead)
{
  DWORD dwReaded = 0;
  bool bRRes = false;
  BYTE cRawData[cuiBuffL];

  setmem(cRawData, sizeof(cRawData[0])*cuiBuffL, '\0');
  bRRes = false;


  //Porto nuskaitymas
  if(!ReadFile(hComm, cRawData, iToRead, &dwReaded, &m_osROverlap))
  {
    if(GetLastError() == ERROR_IO_PENDING)
    {
      //if(WaitForSingleObjectEx(m_osROverlap.hEvent, (int)50, false) == WAIT_TIMEOUT)
      if(WaitForSingleObject(m_osROverlap.hEvent, (int)20) == WAIT_TIMEOUT)
      {
        bRRes = false;
        *iSize = 0;
      }
      else
      {
        bRRes = true;
        *iSize = m_osROverlap.InternalHigh;
        memcpy(Buffer, cRawData, cuiBuffL);
      };
    }
    else
    {
      bRRes = false;
      *iSize = 0;
    };
  }
  else
  {
    bRRes = true;
    *iSize = m_osROverlap.InternalHigh;
    memcpy(Buffer, cRawData, cuiBuffL);
  };


  return bRRes;
};

//---------------------------------------------------------------------------
bool TdSerPort::WriteUSBPort(BYTE *Buffer, int iSize, char I2Ccmd)
{
  if(Buffer == NULL || iSize == 0 || iSize > 64)
    return false;
        
	DWORD dwWriten = 0;
	bool bWRes = false;

	BYTE modBuf[64];
  for(int i=0; i<64; i++)
    modBuf[i] = 0;
	
	modBuf[0] = I2Ccmd;
	modBuf[1] = CFG_ADDR;

	if(I2Ccmd == CMD_LMS_WR)
	{
		modBuf[2] = iSize/2;
	}
	else
	{
		modBuf[2] = iSize;
	}
	modBuf[3] = 0;
        
	for(int i=0; i<iSize; i++)
	{
			modBuf[i+4] = Buffer[i];
	}

	LONG iBufSize = USB_BUFFER_LENGTH;
        bWRes = OutCtrEndPt->XferData(modBuf, iBufSize);

        if( I2Ccmd == CMD_LMS_WR )  // after writing, chip sends back empty buffer
        {
                char tempBuf[USB_BUFFER_LENGTH];
                ReadUSBPort(tempBuf);
        }

	return bWRes;
}

//---------------------------------------------------------------------------
bool TdSerPort::ReadUSBPort(LPVOID Buffer)
{
	DWORD dwReaded = 0;
	bool bRRes = false;
	BYTE cRawData[64];

	memset(cRawData, '\0', sizeof(cRawData[0])*64);
	bRRes = false;

	long int iBufSize;
	iBufSize = USB_BUFFER_LENGTH;

	bRRes = InCtrEndPt->XferData(cRawData, iBufSize);
	if(bRRes)
	{
	        memcpy(Buffer, cRawData, sizeof(BYTE)*USB_BUFFER_LENGTH);
	}
	else
	{
	        bRRes = false;
	}
	return bRRes;
}

//---------------------------------------------------------------------------
void TdSerPort::SPI_Rst()
{
  char buf = 0x10;
  bool rez;
	unsigned char Buf[USB_BUFFER_LENGTH];
  memset(Buf, 0, USB_BUFFER_LENGTH);

  if(iPortSelected) //if usb selected
  {
        //rez = WriteUSBPort(&buf, 1, CMD_LMS_WR);

    //Reset of LMS6002D is controlled via FPGA
  	Buf[0] = CMD_CFG_I2C_WR;
	  Buf[1] = CFG_ADDR;
    Buf[3] = 2;
    Buf[4] = 0x05;  //Address in FPGA
    Buf[5] = 0x00;  //LMS Reset is Active
    Buf[6] = 0x05;  //Address in FPGA
    Buf[7] = 0x00;  //LMS Reset is Inactive

    LONG length = USB_BUFFER_LENGTH;
    rez = OutCtrEndPt->XferData(Buf, length);
    rez = InCtrEndPt->XferData(Buf, length);
  }
  else
  {
        rez = WritePort(&buf, 1);
        rez = !rez;
  }
};

//---------------------------------------------------------------------------
void TdSerPort::SPI_RstAct()
{
  char buf = 0x11;
  bool rez;
	BYTE Buf[USB_BUFFER_LENGTH];
  memset(Buf, 0, USB_BUFFER_LENGTH);

  if(iPortSelected) //if usb selected
  {
        //rez = WriteUSBPort(&buf, 1, CMD_LMS_WR);
    //Reset of LMS6002D is controlled via FPGA
  	Buf[0] = CMD_CFG_I2C_WR;
	  Buf[1] = CFG_ADDR;
    Buf[3] = 1;
    Buf[4] = 0x05;  //Address in FPGA
    Buf[5] = 0x00;  //LMS Reset is Active
    LONG length = USB_BUFFER_LENGTH;
    rez = OutCtrEndPt->XferData(Buf, length);
    rez = InCtrEndPt->XferData(Buf, length);
  }
  else
  {
        rez = WritePort(&buf, 1);
        rez = !rez;
  }
};

//---------------------------------------------------------------------------
void TdSerPort::SPI_RstInact()
{
  char buf = 0x12;
  bool rez;
	BYTE Buf[USB_BUFFER_LENGTH];
  memset(Buf, 0, USB_BUFFER_LENGTH);
  
  if(iPortSelected) //if usb selected
  {
        //rez = WriteUSBPort(&buf, 1, CMD_LMS_WR);
    //Reset of LMS6002D is controlled via FPGA
  	Buf[0] = CMD_CFG_I2C_WR;
	  Buf[1] = CFG_ADDR;
    Buf[3] = 1;
    Buf[4] = 0x05;  //Address in FPGA
    Buf[5] = 0x01;  //LMS Reset is Inactive
    LONG length = USB_BUFFER_LENGTH;
    rez = OutCtrEndPt->XferData(Buf, length);
    rez = InCtrEndPt->XferData(Buf, length);
  }
  else
  {
        rez = WritePort(&buf, 1);
        rez = !rez;
  }
};

//---------------------------------------------------------------------------
void TdSerPort::SetBrdPA(char Code)
{
	BYTE Buf[USB_BUFFER_LENGTH];
  bool rez;
  memset(Buf, 0, USB_BUFFER_LENGTH);
	
  if(iPortSelected)   //if USB selected
  {
  	Buf[0] = CMD_CFG_I2C_WR;
	  Buf[1] = CFG_ADDR;
    Buf[3] = 1;
    Buf[4] = 0x07;  //Address in FPGA
    Buf[5] = Code;
    LONG length = USB_BUFFER_LENGTH;
    rez = OutCtrEndPt->XferData(Buf, length);
    rez = InCtrEndPt->XferData(Buf, length);
  }
};

//---------------------------------------------------------------------------
void TdSerPort::SetBrdLNA(char Code)
{
	BYTE Buf[USB_BUFFER_LENGTH];
  bool rez;
  memset(Buf, 0, USB_BUFFER_LENGTH);

  if(iPortSelected)   //if USB selected
  {
  	Buf[0] = CMD_CFG_I2C_WR;
	  Buf[1] = CFG_ADDR;
    Buf[3] = 1;
    Buf[4] = 0x08;  //Address in FPGA
    Buf[5] = Code;
    LONG length = USB_BUFFER_LENGTH;
    rez = OutCtrEndPt->XferData(Buf, length);
    rez = InCtrEndPt->XferData(Buf, length);
  }
};

//---------------------------------------------------------------------------
void TdSerPort::SPI_Rd(sPrData *pPD)
{
  //char *buf = new char[pPD->iToR+1];
  char *buf = new char[cuiBuffL];
  //BYTE Bbuf[cuiBuffLen];

  bool rez;
  int itmp = 0;
  setmem(buf, cuiBuffL, 0x0);

  if(iPortSelected)    //if USB selected
  {
        //Construct write (uC->Chip) commands
        //buf[0] = 0x20;
	for(int j=0; j<pPD->iToR; j++)
       	{
            buf[itmp] = pPD->cCmdR[pPD->iRInd[j]];
            itmp++;
        };

          //Write commands
          rez = WriteUSBPort(buf, pPD->iToR, CMD_LMS_RD);

          //Read Data from Chip
          setmem(buf, cuiBuffL, 0x0);
          ReadUSBPort(buf);
          for(int j=0; j<pPD->iToR; j++)
	  {
                pPD->cDataR[pPD->iRInd[j]] = buf[j];
          };
  }
  else
  {
  //Construct write (uC->Chip) commands
  buf[0] = 0x20;
	for(int j=0; j<pPD->iToR; j++)
        {
         itmp++;
         buf[itmp] = pPD->cCmdR[pPD->iRInd[j]];
        };

  //Write commands
  PurgeComm(hComm, PURGE_RXCLEAR);
  rez = WritePort(buf, pPD->iToR+1);

  //Read Data from Chip
  setmem(buf, cuiBuffL, 0x0);
  ReadPort(buf, &itmp, pPD->iToR);
	for(int j=0; j<pPD->iToR; j++)
	{
    pPD->cDataR[pPD->iRInd[j]] = buf[j];
  };
  }
  delete[] buf;
};

char TdSerPort::SPI_Rd_OneByte(char Command)
{
  char *buf = new char[cuiBuffL];
  bool rez;
  char cRez;
  setmem(buf, cuiBuffL, 0x0);

  if(iPortSelected)   //if USB selected
  {
          int itmp = 0;
          //Construct write (uC->Chip) commands
          buf[0] = Command;
          //Write command

          rez = WriteUSBPort(buf, 1, CMD_LMS_RD);

          //Read Data from Chip
          setmem(buf, cuiBuffL, 0x0);
          ReadUSBPort(buf);
          cRez = buf[0];
  }
  else
  {
          int itmp = 0;    
          //Construct write (uC->Chip) commands
          buf[0] = 0x20;
          buf[1] = Command;

          //Write command
          PurgeComm(hComm, PURGE_RXCLEAR);
          rez = WritePort(buf, 2);

          //Read Data from Chip
          setmem(buf, cuiBuffL, 0x0);
          ReadPort(buf, &itmp, 1);
          cRez = buf[0];
  }
  delete[] buf;
  return cRez;

};

//---------------------------------------------------------------------------
void TdSerPort::SPI_Wr(sPrData *pPD)
{
  char *buf = new char[2*pPD->iToW+1];
  bool rez;
  int ind = 0;

  if(iPortSelected)   //if USB selected
  {
	for(int j=0; j<pPD->iToW; j++)
	{
            buf[ind] = pPD->cCmdW[pPD->iWInd[j]];
            ind++;
            buf[ind] = pPD->cDataW[pPD->iWInd[j]];
            ind++;
        };

        rez = WriteUSBPort(buf, 2*pPD->iToW, CMD_LMS_WR);
        rez = !rez;
  }
  else
  {
  buf[0] = 0x30;
	for(int j=0; j<pPD->iToW; j++)
	{
    ind++;
    buf[ind] = pPD->cCmdW[pPD->iWInd[j]];
    ind++;
    buf[ind] = pPD->cDataW[pPD->iWInd[j]];
  };

  rez = WritePort(buf, 2*pPD->iToW+1);
  rez = !rez;
  }

  delete[] buf;
};

//---------------------------------------------------------------------------
void TdSerPort::SPI_Wr_ADF(sPrData *pPD)
{
  char *buf = new char[2*pPD->iToW+1];
  bool rez;
  int ind = 0;

  if(iPortSelected) //if usb selected
  {
        for(int j=0; j<pPD->iToW; j++)
	{
                ind++;
                buf[ind] = pPD->cDataW[pPD->iWInd[j]];
        };
        rez = WriteUSBPort(buf, pPD->iToW, CMD_LMS_WR);
  }
  else
  {
        buf[0] = 0xAD;
	for(int j=0; j<pPD->iToW; j++)
	{
            ind++;
            buf[ind] = pPD->cDataW[pPD->iWInd[j]];
        };
        rez = WritePort(buf, pPD->iToW+1);
        rez = !rez;
  }
  delete[] buf;
};

//---------------------------------------------------------------------------
void TdSerPort::SPI_Wr_Xil(sPrData *pPD)
{
  char *buf = new char[2*pPD->iToW+1];
  bool rez;
  int ind = 0;

  if(iPortSelected)
  {
        for(int j=0; j<pPD->iToW; j++)
	{
            ind++;
            buf[ind] = pPD->cCmdW[pPD->iWInd[j]];
            ind++;
            buf[ind] = pPD->cDataW[pPD->iWInd[j]];
        };

        rez = WriteUSBPort(buf, 2*pPD->iToW, CMD_LMS_WR);
        rez = !rez;
  }
  else
  {
        buf[0] = 0xA0;
	for(int j=0; j<pPD->iToW; j++)
	{
            ind++;
            buf[ind] = pPD->cCmdW[pPD->iWInd[j]];
            ind++;
            buf[ind] = pPD->cDataW[pPD->iWInd[j]];
        };

        rez = WritePort(buf, 2*pPD->iToW+1);
        rez = !rez;

  }
  delete[] buf;
  
};

//---------------------------------------------------------------------------
void TdSerPort::SPI_Rd_Xil(sPrData *pPD)
{
  //char *buf = new char[pPD->iToR+1];
  char *buf = new char[cuiBuffL];
  //BYTE Bbuf[cuiBuffLen];

  bool rez;
  int itmp = 0;

  //Construct write (uC->Chip) commands
  setmem(buf, cuiBuffL, 0x0);

  if(iPortSelected)
  {
        for(int j=0; j<pPD->iToR; j++)
        {
            itmp++;
            buf[itmp] = pPD->cCmdR[pPD->iRInd[j]];
        };

        //Write commands
        rez = WriteUSBPort(buf, pPD->iToR, CMD_LMS_RD);

        //Read Data from Chip
        setmem(buf, cuiBuffL, 0x0);
        ReadUSBPort(buf);
  }
  else
  {
        buf[0] = 0xA1;
        for(int j=0; j<pPD->iToR; j++)
        {
            itmp++;
            buf[itmp] = pPD->cCmdR[pPD->iRInd[j]];
        };

        //Write commands
        PurgeComm(hComm, PURGE_RXCLEAR);
        rez = WritePort(buf, pPD->iToR+1);

        //Read Data from Chip
        setmem(buf, cuiBuffL, 0x0);
        ReadPort(buf, &itmp, pPD->iToR);
  }

  for(int j=0; j<pPD->iToR; j++)
  {
    pPD->cDataR[pPD->iRInd[j]] = buf[j];
  };
  delete[] buf;
}

//---------------------------------------------------------------------------
void TdSerPort::SPI_Rd_AD7691(sPrData *pPD)
{
  //char *buf = new char[pPD->iToR+1];
  char *buf = new char[cuiBuffL];
  //BYTE Bbuf[cuiBuffLen];

  bool rez;
  int itmp = 0;

  //Construct write (uC->Chip) commands
  setmem(buf, cuiBuffL, 0x0);

  if(iPortSelected)
  {
        buf[0] = 0xA6;

        //Write commands
        rez = WriteUSBPort(buf, pPD->iToW, CMD_LMS_RD);

        //Read Data from Chip
        setmem(buf, cuiBuffL, 0x0);
        ReadUSBPort(buf);
  }
  else
  {
  buf[0] = 0xA6;

  //Write commands
  PurgeComm(hComm, PURGE_RXCLEAR);
  rez = WritePort(buf, pPD->iToW);

  //Read Data from Chip
  setmem(buf, cuiBuffL, 0x0);
  ReadPort(buf, &itmp, pPD->iToR);

  }

  for(int j=0; j<pPD->iToR; j++)
  {
    pPD->cDataR[j] = buf[j];
  };
  delete[] buf;
}

//---------------------------------------------------------------------------
void TdSerPort::SPI_Wr_AD9552(sPrData *pPD)
{
  char *buf = new char[pPD->iToW+1];
  bool rez;
  int ind = 0;

  if(iPortSelected)
  {
        for(int j=0; j<pPD->iToW; j++)
	{
            ind++;
            buf[ind] = pPD->cCmdW[j];
        };

        rez = WriteUSBPort(buf, pPD->iToW, CMD_LMS_WR);
        rez = !rez;
  }
  else
  {
        buf[0] = 0xA3;
	for(int j=0; j<pPD->iToW; j++)
	{
            ind++;
            buf[ind] = pPD->cCmdW[j];
        };

        rez = WritePort(buf, pPD->iToW+1);
        rez = !rez;
  }
  delete[] buf;
};

//---------------------------------------------------------------------------

void TdSerPort::findUSBDevices()
{
    cmbUSBDevices->Items->Clear();
    int iUSBcount = USBDevice->DeviceCount();
    if(iUSBcount == 0)
        return;

    cmbInEndPt->Enabled = false;
    cmbOutEndPt->Enabled = false;
    cmbInEndPt->Visible = false;
    cmbOutEndPt->Visible = false;
    if(iUSBcount)
    {
        //cmbInEndPt->Visible = true;
        //cmbOutEndPt->Visible = true;
        //cmbInEndPt->Enabled = true;
        //cmbOutEndPt->Enabled = true;

        for(int i=0; i<1; i++)
        {
                CCyUSBEndPoint *endpt;
                USBDevice->Open(i);
                for (int i=0; i<USBDevice->EndPointCount(); i++)  // skip 0-control endpoint
                {
                        endpt = USBDevice->EndPoints[i];
                        if(USBDevice->EndPoints[i]->Address == 0x86)
                        {
                                InEndPt = USBDevice->EndPoints[i];
                        }

                        if(USBDevice->EndPoints[i]->Address == 0x02)
                        {
                                OutEndPt = USBDevice->EndPoints[i];
                        }

                        if(USBDevice->EndPoints[i]->Address == 0x01)
                        {
                                OutCtrEndPt = USBDevice->EndPoints[i];
                        }

                        if(USBDevice->EndPoints[i]->Address == 0x81)
                        {
                                InCtrEndPt = USBDevice->EndPoints[i];
                        }
                }
		cmbUSBDevices->Items->Add(USBDevice->DeviceName);
        }
        cmbUSBDevices->ItemIndex = 0;
        //cmbUSBDevices->ItemIndex = 0;
        //iSelectedUSBDeviceIndex = 0;
        //iSelectedInEndPt = 0;
        //iSelectedOutEndPt = 0;
        //cmbUSBDevicesChange(Application);
    }
    else
    {
        //rgrPortSelection->ItemIndex = 0;
        cmbUSBDevices->Items->Add("USB Devices not found");
        cmbUSBDevices->Enabled = false;
        cmbUSBDevices->ItemIndex = 0;
        //iSelectedUSBDeviceIndex = -1;
        //iSelectedInEndPt = -1;
        //iSelectedOutEndPt = -1;
    }
}

void __fastcall TdSerPort::cmbUSBDevicesChange(TObject *Sender)
{
        /*CloseUSBDevice();

        iSelectedUSBDeviceIndex = cmbUSBDevices->ItemIndex;
        inEndPointIndexes.clear();
        cmbInEndPt->Clear();
        outEndPointIndexes.clear();
        cmbOutEndPt->Clear();

        OpenUSBDevice(iSelectedUSBDeviceIndex);
        int iEndpts = USBDevice->EndPointCount();

        CCyUSBEndPoint *endpt;

        for (int i=1; i<iEndpts; i++)  // skip 0-control endpoint
        {
                endpt = USBDevice->EndPoints[i];
                if(USBDevice->EndPoints[i]->Address == 0x86)
                {
                         InEndpt = USBDevice->EndPoints[i];
                }
                if(USBDevice->EndPoints[i]->Address == 0x02)
                {
                         OutEndpt = USBDevice->EndPoints[i];
                }
                /*
                if( (endpt->Address & 0x80) == 0x80 )  // IN endpoint
                {
                        inEndPointIndexes.push_back(i);
                        switch(endpt->Attributes)
                        {
                                case 0:  //Control
                                        cmbInEndPt->Items->Add("Control");
                                        break;
                                case 1:  //Isochronous
                                        cmbInEndPt->Items->Add("Isochronous");
                                        break;
                                case 2:  //Bulk
                                        cmbInEndPt->Items->Add("Bulk");
                                        break;
                                case 3:  //Interrupt
                                        cmbInEndPt->Items->Add("Interrupt");
                                        break;
                                default:
                                        cmbInEndPt->Items->Add("Undefined");
                                        break;
                        }
                        continue;
                }
                if( (endpt->Address & 0x00) == 0x00 )  // OUT endpoint
                {
                        outEndPointIndexes.push_back(i);
                        switch(endpt->Attributes)
                        {
                                case 0:  //Control
                                        cmbOutEndPt->Items->Add("Control");
                                        break;
                                case 1:  //Isochronous
                                        cmbOutEndPt->Items->Add("Isochronous");
                                        break;
                                case 2:  //Bulk
                                        cmbOutEndPt->Items->Add("Bulk");
                                        break;
                                case 3:  //Interrupt
                                        cmbOutEndPt->Items->Add("Interrupt");
                                        break;
                                default:
                                        cmbOutEndPt->Items->Add("Undefined");
                                        break;
                        }
                        continue;
                }
                */
        //}
        //iSelectedInEndPt = 2;
        //InEndpt = USBDevice->EndPoints[iSelectedInEndPt];
        //iSelectedOutEndPt = 2;
        //OutEndpt = USBDevice->EndPoints[iSelectedOutEndPt];
}
//---------------------------------------------------------------------------
void TdSerPort::CloseUSBDevice()
{
        if(USBDevice->IsOpen())
        {
                USBDevice->Close();
        }
}
bool TdSerPort::OpenUSBDevice(int iDeviceIndex)
{
        CloseUSBDevice();
        if(iDeviceIndex <= USBDevice->DeviceCount() )
        {
                USBDevice->Open(iDeviceIndex);
                return true;
        }
        else
                return false;     
}
void __fastcall TdSerPort::cmbInEndPtChange(TObject *Sender)
{
        iSelectedInEndPt = inEndPointIndexes[cmbInEndPt->ItemIndex];
        //InEndpt = USBDevice->EndPoints[iSelectedInEndPt];
}
//---------------------------------------------------------------------------

void __fastcall TdSerPort::cmbOutEndPtChange(TObject *Sender)
{
        iSelectedOutEndPt = outEndPointIndexes[cmbOutEndPt->ItemIndex];
        //OutEndpt = USBDevice->EndPoints[iSelectedOutEndPt];
}
//---------------------------------------------------------------------------
void __fastcall TdSerPort::rgrPortSelectionClick(TObject *Sender)
{
        if(USBDevice->DeviceCount() == 0)
        {
                iPortSelected = 0;
                rgrPortSelection->ItemIndex = 0;
        }
        else
                iPortSelected = rgrPortSelection->ItemIndex;
}
//---------------------------------------------------------------------------

CCyUSBEndPoint *TdSerPort::getOutBulkPt()
{
        return OutEndPt;
}
CCyUSBEndPoint *TdSerPort::getInBulkPt()
{
        return InEndPt;
}

CCyUSBEndPoint *TdSerPort::getOutCtrBulkPt()
{
        return OutCtrEndPt;
}
CCyUSBEndPoint *TdSerPort::getInCtrBulkPt()
{
        return InCtrEndPt;
}
