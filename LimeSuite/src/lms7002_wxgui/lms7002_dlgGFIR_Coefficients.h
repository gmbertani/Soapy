#ifndef __lms7002_dlgGFIR_Coefficients__
#define __lms7002_dlgGFIR_Coefficients__

/**
@file
Subclass of dlgGFIR_Coefficients, which is generated by wxFormBuilder.
*/

#include "lms7002_wxgui.h"

//// end generated include
#include <vector>

/** Implementing dlgGFIR_Coefficients */
class lms7002_dlgGFIR_Coefficients : public dlgGFIR_Coefficients
{
	protected:
		// Handlers for dlgGFIR_Coefficients events.
		void OnLoadFromFile( wxCommandEvent& event );
		void OnSaveToFile( wxCommandEvent& event );
		void OnClearTable( wxCommandEvent& event );
        void OnspinCoefCountChange(wxSpinEvent& event);
        void OnBtnOkClick(wxCommandEvent& event);
        void OnBtnCancelClick(wxCommandEvent& event);
	public:
		/** Constructor */
		lms7002_dlgGFIR_Coefficients( wxWindow* parent );
	//// end generated class members
        void SetCoefficients(const std::vector<double> &coefficients);
        std::vector<double> GetCoefficients();
	
};

#endif // __lms7002_dlgGFIR_Coefficients__
