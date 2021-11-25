#include "Room3DStep1Cmd.h"
#include "Room3DDocument.h"
#include "Room3DApplication.h"

CRoom3DStep1Cmd::CRoom3DStep1Cmd(void)
{
}

CRoom3DStep1Cmd::~CRoom3DStep1Cmd(void)
{
}

void CRoom3DStep1Cmd::ResetDocument()
{
	m_pDocument->LoadBackImage(QString());
	g_pApp->GetMainWidget()->ShowGuideDlg(STEP_1_BROWSE);

}
