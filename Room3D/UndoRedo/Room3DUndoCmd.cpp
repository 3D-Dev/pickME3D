#include "Room3DUndoCmd.h"
#include "Room3DApplication.h"
#include "Room3DUndoMgr.h"

CRoom3DUndoCmd::CRoom3DUndoCmd(void)
{
	m_bUndo = true;
	m_pDocument = g_pApp->GetDocument();
}

CRoom3DUndoCmd::~CRoom3DUndoCmd(void)
{
}

bool CRoom3DLoadBackgroundImgCmd::Undo()
{
	CRoom3DUndoCmd::Undo();

	CRoom3DDocument* pDocument = g_pApp->GetDocument();
	return pDocument->LoadBackImage(m_sPrevBackgroundImgPath);
}

bool CRoom3DLoadBackgroundImgCmd::Redo()
{
	CRoom3DUndoCmd::Redo();

	CRoom3DDocument* pDocument = g_pApp->GetDocument();
	return pDocument->LoadBackImage(m_sBackgroundImgPath);
}

bool CRoom3DCalibSetCoordinateCmd::Undo()
{
	CRoom3DUndoCmd::Undo();

	CRoom3DDocument* pDocument = g_pApp->GetDocument();
	pDocument->Get3DSceneMgr()->GetCamera()->InitCalibration();
	return true;
}

bool CRoom3DCalibSetCoordinateCmd::Redo()
{
	CRoom3DUndoCmd::Redo();

	CRoom3DDocument* pDocument = g_pApp->GetDocument();
	for( int i = 0; i < 4; i++ )
	{
		pDocument->Get3DSceneMgr()->GetCamera()->OnSetCoordPos(m_fCoordinatePosition[i*2+0], m_fCoordinatePosition[i*2+1]);
	}
	return true;
}

bool CRoom3DFurnitureAddCmd::Undo()
{
	CRoom3DUndoCmd::Undo();

	CRoom3DDocument* pDocument = g_pApp->GetDocument();
	pDocument->RemoveFurniture(m_pFurniture);
	return true;
}

bool CRoom3DFurnitureAddCmd::Redo()
{
	CRoom3DUndoCmd::Redo();

	CRoom3DDocument* pDocument = g_pApp->GetDocument();
	pDocument->AddFurniture(m_pFurniture);
	return true;
}

bool CRoom3DFurnitureDelCmd::Undo()
{
	CRoom3DUndoCmd::Undo();

	CRoom3DDocument* pDocument = g_pApp->GetDocument();
	pDocument->AddFurniture(m_pFurniture);
	return true;
}

bool CRoom3DFurnitureDelCmd::Redo()
{
	CRoom3DUndoCmd::Redo();

	CRoom3DDocument* pDocument = g_pApp->GetDocument();
	pDocument->RemoveFurniture(m_pFurniture);
	return true;
}

bool CRoom3DFurnitureMoveCmd::Redo()
{
	CRoom3DUndoCmd::Redo();

	if(m_pFurniture)
	{
		m_pFurniture->SetPos(m_vCurPos);
		return true;
	}
	return false;
}

bool CRoom3DFurnitureMoveCmd::Undo()
{
	CRoom3DUndoCmd::Undo();

	if(m_pFurniture)
	{
		m_pFurniture->SetPos(m_vPrevPos);
		return true;
	}
	return false;
}

bool CRoom3DFurnitureRotateCmd::Undo()
{
	CRoom3DUndoCmd::Undo();

	if(m_pFurniture)
	{
		m_pFurniture->SetRot(m_fPrevRotY);
		return true;
	}
	return false;
}

bool CRoom3DFurnitureRotateCmd::Redo()
{
	CRoom3DUndoCmd::Redo();

	if(m_pFurniture)
	{
		m_pFurniture->SetRot(m_fCurRotY);
		return true;
	}
	return false;
}

bool CRoom3DBkImgBrightCmd::Undo()
{
	m_pDocument->SetBrightAndContrast(m_nPrevBright, m_nPrevContrast);
	return true;
}

bool CRoom3DBkImgBrightCmd::Redo()
{
	m_pDocument->SetBrightAndContrast(m_nBright, m_nContrast);
	return true;
}