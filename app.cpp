//---------------------------------------
// wsApplication::Create()
//---------------------------------------
// Called by std_kernel.cpp, this method creates the UI
// which is then "run" via calls to wsApplication::Initialize()
// and wsApplication::timeSlice()

#include "uiWindow.h"
#include "LooperVersion.h"
#include <circle/logger.h>
#include <utils/myUtils.h>

#define log_name  "app"


void wsApplication::Create()
{
	// Note that this is NOT a derived application.
	// This is the DEFINITION of THE wsApplication::Create() method,
	// so it has access to the private m_pScreen member

	// This is also a good place to display the version ...

	LOG("Looper " LOOPER_VERSION " wsApplication::Create(%08x)",this);

	// SPLASH SCREEN

	wsDC *pDC = new wsDC(m_pScreen);
	wsRect full_screen(0,0,m_pScreen->GetWidth()-1,m_pScreen->GetHeight()-1);
	pDC->setFont(wsFont16x26);
	pDC->fillScreen( wsDARK_CYAN );
	pDC->putText(
		wsDARK_CYAN,
		wsBLACK,
		full_screen,
		ALIGN_CENTER,
		0,0,
		"Looper " LOOPER_VERSION );
	delay(3000);


	new uiWindow(this,ID_WIN_LOOPER,0,0,getWidth()-1,getHeight()-1);
}
