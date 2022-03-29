#ifndef PNLRXVGA2_H
#define PNLRXVGA2_H

//(*Headers(pnlRxVGA2)
#include <wx/panel.h>
class wxStaticBoxSizer;
class wxComboBox;
class wxFlexGridSizer;
class wxButton;
class wxStaticLine;
class wxStaticText;
class wxRadioBox;
class wxCheckBox;
//*)

class pnlRxVGA2: public wxPanel
{
	public:

		pnlRxVGA2(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize, int styles= 0, wxString idname="");
		virtual ~pnlRxVGA2();

        void FillComboBoxes();
        void SetGuiDefaults();
        void SetGuiDecode();
        void Initialize();
        bool UpdateGUI();

        void UpdateVerRevMask(char Ver, char Rev, char Mask);

		//(*Declarations(pnlRxVGA2)
		wxCheckBox* chbPwrDCDACA;
		wxComboBox* cmbVCM;
		wxCheckBox* chbPwrDCCmpA;
		wxButton* btnDCReadVal;
		wxButton* btnDCRstClbr;
		wxButton* btnDCStartClbr;
		wxCheckBox* chbPwrCurrRef;
		wxButton* btnDCLoadVal;
		wxCheckBox* chbPwrBypAB;
		wxStaticText* lblDC_CLBR_DONE;
		wxStaticText* lblDC_LOCK;
		wxStaticText* StaticText1;
		wxCheckBox* chbPwrBG;
		wxComboBox* cmbVga2G_u;
		wxStaticText* StaticText3;
		wxCheckBox* chbPwrBypA;
		wxStaticLine* StaticLine1;
		wxComboBox* cmbVga2GA_t;
		wxCheckBox* chbPwrDCDACB;
		wxComboBox* cmbDCCalAddr;
		wxStaticText* lblDC_UD;
		wxStaticText* StaticText8;
		wxCheckBox* chbPwrDCCmpB;
		wxStaticText* StaticText7;
		wxComboBox* cmbCalVal;
		wxStaticText* StaticText4;
		wxStaticText* StaticText5;
		wxStaticText* StaticText2;
		wxStaticText* StaticText6;
		wxComboBox* cmbVga2GB_t;
		wxRadioBox* rgrDecode;
		wxCheckBox* chbPwrVGA2Mods;
		wxStaticText* StaticText9;
		wxStaticText* lbDC_REGVAL;
		wxCheckBox* chbPwrBypB;
		wxCheckBox* chbPwrDCCurrR;
		//*)

	protected:

		//(*Identifiers(pnlRxVGA2)
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON4;
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICLINE1;
		static const long ID_COMBOBOX1;
		static const long ID_COMBOBOX2;
		static const long ID_BUTTON5;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT7;
		static const long ID_STATICTEXT4;
		static const long ID_STATICTEXT8;
		static const long ID_STATICTEXT5;
		static const long ID_STATICTEXT9;
		static const long ID_STATICTEXT6;
		static const long ID_STATICTEXT10;
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX3;
		static const long ID_CHECKBOX4;
		static const long ID_CHECKBOX5;
		static const long ID_CHECKBOX6;
		static const long ID_CHECKBOX7;
		static const long ID_CHECKBOX8;
		static const long ID_CHECKBOX9;
		static const long ID_CHECKBOX10;
		static const long ID_CHECKBOX11;
		static const long ID_STATICTEXT11;
		static const long ID_COMBOBOX3;
		static const long ID_STATICTEXT12;
		static const long ID_COMBOBOX4;
		static const long ID_STATICTEXT13;
		static const long ID_COMBOBOX5;
		static const long ID_COMBOBOX6;
		static const long ID_RADIOBOX2;
		//*)

	private:

		//(*Handlers(pnlRxVGA2)
		void chbPwrVGA2ModsClick(wxCommandEvent& event);
		void rgrDecodeClick(wxCommandEvent& event);
		void cmbVga2G_uChange(wxCommandEvent& event);
		void chbPwrDCCurrRClick(wxCommandEvent& event);
		void chbPwrDCDACBClick(wxCommandEvent& event);
		void chbPwrDCCmpBClick(wxCommandEvent& event);
		void chbPwrDCDACAClick(wxCommandEvent& event);
		void chbPwrDCCmpAClick(wxCommandEvent& event);
		void chbPwrBGClick(wxCommandEvent& event);
		void chbPwrBypABClick(wxCommandEvent& event);
		void chbPwrBypBClick(wxCommandEvent& event);
		void chbPwrBypAClick(wxCommandEvent& event);
		void chbPwrCurrRefClick(wxCommandEvent& event);
		void cmbVga2GB_tChange(wxCommandEvent& event);
		void cmbVga2GA_tChange(wxCommandEvent& event);
		void cmbDCCalAddrChange(wxCommandEvent& event);
		void cmbCalValChange(wxCommandEvent& event);
		void btnDCRstClbrClick(wxCommandEvent& event);
		void btnDCLoadValClick(wxCommandEvent& event);
		void btnDCStartClbrClick(wxCommandEvent& event);
		void btnDCReadValClick(wxCommandEvent& event);
		void cmbVCMChange(wxCommandEvent& event);
		void OnEraseBackground(wxEraseEvent& event);
		//*)

	protected:
        char m_cMAddr;
        char m_cVer;
        char m_cRev;
        char m_cMask;
        bool sendChanges;

        // VCM code to combobox index recalculation
        int VCM2CMB(int code);
        int CMB2VCM(int code);
		void BuildContent(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size);

		DECLARE_EVENT_TABLE()
};

#endif
