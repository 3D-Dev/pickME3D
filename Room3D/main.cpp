#include "room3d.h"
#include "Room3DApplication.h"
#include <QtPlugin>
#include "Room3DLogoDlg.h"

#define SHOW_TIME			2000

#if defined(NDEBUG) && defined(_SUPPORT_JPEG_STATIC_LIB)
 Q_IMPORT_PLUGIN(qjpeg)
#endif

CRoom3DApplication* g_pApp = NULL;

 QString Trans(const char* s)
 {
	 QString sTransResult;
	 sTransResult = QApplication::translate("CRoom3DTempClassForTSFile", s ,0);
	 if(sTransResult.isEmpty())
		 sTransResult = QString(s);
	 return sTransResult;
 }

int main(int argc, char *argv[])
{
	CRoom3DApplication a(argc, argv);
	Room3D w;
	QTime time;
	time.start();
	// show logo window
	a.SetAppStep(APP_LOGO);
	// init application
	a.InitApplication();
	// keep logo window at least SHOW_TIME ms.
	int nElapsedTime = 0;
	while(nElapsedTime < SHOW_TIME)
	{
		::Sleep(10);
		nElapsedTime = time.elapsed();
		g_pApp->processEvents();
	}
	// move to next step, introduct video
	a.SetAppStep(APP_EDIT_STEP_1); //a.SetAppStep(APP_INTRO_MOVIE);
	return a.exec();
}
