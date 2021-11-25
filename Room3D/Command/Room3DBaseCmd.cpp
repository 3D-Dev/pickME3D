#include "Room3DBaseCmd.h"
#include "Common.h"
#include "Room3DApplication.h"
#include "Room3DDocument.h"
#include "Room3DSceneView.h"

CRoom3DBaseCmd::CRoom3DBaseCmd(void)
{
	m_pDocument = g_pApp->GetDocument();
	m_pSceneView = g_pApp->GetSceneView();
}

CRoom3DBaseCmd::~CRoom3DBaseCmd(void)
{
}
