#include "lms7002_pnlRBB_view.h"
#include <map>
#include <wx/msgdlg.h>
#include "numericSlider.h"
#include "lms7002_gui_utilities.h"
#include "lms7suiteEvents.h"
#include "lms7suiteAppFrame.h"
using namespace lime;

lms7002_pnlRBB_view::lms7002_pnlRBB_view( wxWindow* parent )
:
pnlRBB_view( parent )
{

}

lms7002_pnlRBB_view::lms7002_pnlRBB_view( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
    : pnlRBB_view(parent, id, pos, size, style), lmsControl(nullptr)
{
    wndId2Enum[cmbC_CTL_LPFH_RBB] = LMS7param(C_CTL_LPFH_RBB);
    wndId2Enum[cmbC_CTL_LPFL_RBB] = LMS7param(C_CTL_LPFL_RBB);
    wndId2Enum[cmbC_CTL_PGA_RBB] = LMS7param(C_CTL_PGA_RBB);
    wndId2Enum[chkEN_G_RBB] = LMS7param(EN_G_RBB);
    wndId2Enum[cmbG_PGA_RBB] = LMS7param(G_PGA_RBB);
    wndId2Enum[cmbICT_LPF_IN_RBB] = LMS7param(ICT_LPF_IN_RBB);
    wndId2Enum[cmbICT_LPF_OUT_RBB] = LMS7param(ICT_LPF_OUT_RBB);
    wndId2Enum[cmbICT_PGA_IN_RBB] = LMS7param(ICT_PGA_IN_RBB);
    wndId2Enum[cmbICT_PGA_OUT_RBB] = LMS7param(ICT_PGA_OUT_RBB);
    wndId2Enum[cmbINPUT_CTL_PGA_RBB] = LMS7param(INPUT_CTL_PGA_RBB);
    wndId2Enum[rgrOSW_PGA_RBB] = LMS7param(OSW_PGA_RBB);
    wndId2Enum[chkPD_LPFH_RBB] = LMS7param(PD_LPFH_RBB);
    wndId2Enum[chkPD_LPFL_RBB] = LMS7param(PD_LPFL_RBB);
    wndId2Enum[chkPD_PGA_RBB] = LMS7param(PD_PGA_RBB);
    wndId2Enum[cmbRCC_CTL_LPFH_RBB] = LMS7param(RCC_CTL_LPFH_RBB);
    wndId2Enum[cmbRCC_CTL_LPFL_RBB] = LMS7param(RCC_CTL_LPFL_RBB);
    wndId2Enum[cmbRCC_CTL_PGA_RBB] = LMS7param(RCC_CTL_PGA_RBB);
    wndId2Enum[cmbR_CTL_LPF_RBB] = LMS7param(R_CTL_LPF_RBB);
    wndId2Enum[chkEN_DIR_RBB] = LMS7param(EN_DIR_RBB);

    wxArrayString temp;
    temp.clear();
    for (int i = 0; i<32; ++i)
    {
        temp.push_back(wxString::Format(_("%i dB"), -12 + i));
    }
    cmbG_PGA_RBB->Set(temp);

    temp.clear();
    for (int i = 0; i<32; ++i)
    {
        temp.push_back(wxString::Format(_("%i"), i));
    }
    cmbICT_LPF_IN_RBB->Set(temp);
    cmbICT_LPF_OUT_RBB->Set(temp);
    cmbICT_PGA_IN_RBB->Set(temp);
    cmbICT_PGA_OUT_RBB->Set(temp);
    cmbRCC_CTL_PGA_RBB->Append(temp);
    cmbR_CTL_LPF_RBB->Append(temp);

    temp.clear();
    temp.push_back("LPFL_RBB");
    temp.push_back("LPFH_RBB");
    temp.push_back("LPF bypass");
    temp.push_back("Loopback from TX");
    temp.push_back("Loopback path from peak detector");
    cmbINPUT_CTL_PGA_RBB->Set(temp);

    temp.clear();
    for (int i = 0; i<8; ++i)
    {
        temp.push_back(wxString::Format(_("%i"), i));
    }
    cmbRCC_CTL_LPFH_RBB->Set(temp);

    temp.clear();
    temp.push_back("when rxMode is 1.4 MHz");
    temp.push_back("when 3 MHz");
    temp.push_back("when 5 MHz");
    temp.push_back("when 10 MHz");
    temp.push_back("when 15 MHz");
    temp.push_back("when 20 MHz");
    cmbRCC_CTL_LPFL_RBB->Set(temp);

    LMS7002_WXGUI::UpdateTooltips(wndId2Enum, true);
}

void lms7002_pnlRBB_view::Initialize(lms_device_t* pControl)
{
    lmsControl = pControl;
    assert(lmsControl != nullptr);
}

void lms7002_pnlRBB_view::ParameterChangeHandler(wxSpinEvent& event)
{
    wxCommandEvent evt;
    evt.SetInt(event.GetInt());
    evt.SetId(event.GetId());
    evt.SetEventObject(event.GetEventObject());
    ParameterChangeHandler(evt);
}

void lms7002_pnlRBB_view::ParameterChangeHandler(wxCommandEvent& event)
{
    assert(lmsControl != nullptr);
    LMS7Parameter parameter;
    try
    {
        parameter = wndId2Enum.at(reinterpret_cast<wxWindow*>(event.GetEventObject()));
    }
    catch (std::exception & e)
    {
        std::cout << "Control element(ID = " << event.GetId() << ") don't have assigned LMS parameter." << std::endl;
        return;
    }
    LMS_WriteParam(lmsControl,parameter,event.GetInt());
}

void lms7002_pnlRBB_view::OncmbBBLoopbackSelected( wxCommandEvent& event )
{
    switch (cmbBBLoopback->GetSelection())
    {
    case 0:
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFH_RBB),true);
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFL_RBB),false);
        break;
    case 1:
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFH_RBB),false);
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFL_RBB),true);
        break;
    case 2:
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFH_RBB),false);
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFL_RBB),false);
        break;
    default:
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFH_RBB),false);
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFL_RBB),false);
    }
}

void lms7002_pnlRBB_view::UpdateGUI()
{
    assert(lmsControl != nullptr);

    LMS7002_WXGUI::UpdateControlsByMap(this, lmsControl, wndId2Enum);

    long BBloopbackValue = 0;
    uint16_t EN_LB_LPFH_RBBvalue;
    LMS_ReadParam(lmsControl,LMS7param(EN_LB_LPFH_RBB),&EN_LB_LPFH_RBBvalue);

    uint16_t EN_LB_LPFL_RBBvalue;
    LMS_ReadParam(lmsControl,LMS7param(EN_LB_LPFL_RBB),&EN_LB_LPFL_RBBvalue);
    if (!EN_LB_LPFH_RBBvalue && !EN_LB_LPFL_RBBvalue)
        BBloopbackValue = 2;
    else if (EN_LB_LPFH_RBBvalue && !EN_LB_LPFL_RBBvalue)
        BBloopbackValue = 0;
    else if (!EN_LB_LPFH_RBBvalue && EN_LB_LPFL_RBBvalue)
        BBloopbackValue = 1;
    else //invalid combination
    {
        BBloopbackValue = 2;
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFH_RBB),false);
        LMS_WriteParam(lmsControl,LMS7param(EN_LB_LPFL_RBB),false);
    }
    cmbBBLoopback->SetSelection(BBloopbackValue);

    //check if B channel is enabled
    uint16_t value;
    LMS_ReadParam(lmsControl,LMS7param(MAC),&value);
    if (value >= 2)
    {
        LMS_ReadParam(lmsControl,LMS7param(MIMO_SISO),&value);
        if (value != 0)
            wxMessageBox(_("MIMO channel B is disabled"), _("Warning"));
    }

    LMS_ReadParam(lmsControl,LMS7param(TRX_GAIN_SRC),&value);
    cmbG_PGA_RBB->Enable(!value);
    cmbC_CTL_PGA_RBB->Enable(!value);
}

void lms7002_pnlRBB_view::OnbtnTuneFilter(wxCommandEvent& event)
{
    double input1;
    txtLowBW_MHz->GetValue().ToDouble(&input1);

    int status;
    uint16_t ch;

    LMS_ReadParam(lmsControl,LMS7param(MAC),&ch);
    ch = (ch == 2) ? 1 : 0;
    ch += 2*LMS7SuiteAppFrame::m_lmsSelection;
    status = LMS_SetLPFBW(lmsControl,LMS_CH_RX,ch,input1*1e6);
    if (status != 0){
        wxMessageBox(wxString(_("Rx Filter tune failed")), _("Error"));
        return;
    }
    LMS_Synchronize(lmsControl,false);
    UpdateGUI();
}
