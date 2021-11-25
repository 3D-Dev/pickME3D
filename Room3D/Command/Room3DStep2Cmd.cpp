#include "Room3DStep2Cmd.h"
#include "Common.h"
#include "Room3DApplication.h"

CRoom3DStep2Cmd::CRoom3DStep2Cmd(void)
{
}

CRoom3DStep2Cmd::~CRoom3DStep2Cmd(void)
{
	
}

void CRoom3DStep2Cmd::MousePressEvent(QMouseEvent * a_pEvent)
{
	g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->OnMousePress(a_pEvent->x(), a_pEvent->y());
	g_pApp->GetMainWidget()->UpdateControlState();
}

void CRoom3DStep2Cmd::MouseReleaseEvent(QMouseEvent * a_pEvent)
{
	g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->OnMouseUp(a_pEvent->x(), a_pEvent->y());
	g_pApp->GetSceneView()->updateGL();
}

void CRoom3DStep2Cmd::MouseMoveEvent(QMouseEvent * a_pEvent)
{
	g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->OnMouseMove(a_pEvent->x(), a_pEvent->y());
	g_pApp->GetSceneView()->updateGL();
}

void CRoom3DStep2Cmd::ResetDocument()
{
	g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->ResetParameters();
	g_pApp->GetMainWidget()->ShowGuideDlg(STEP_2_SET_CORNER);
	g_pApp->GetMainWidget()->UpdateControlState();
}
