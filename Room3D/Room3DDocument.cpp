#include "Room3DDocument.h"
#include "Room3DApplication.h"
#include "Room3D.h"
#include "Room3DSceneView.h"
#include "Room3DUndoMgr.h"
#include "Room3DUtility.h"

CRoom3DDocument::CRoom3DDocument(void)
{
	m_nBright = 0;
	m_nContrast = 0;
	m_sOrgBkImgPath = QString("");
	m_sOrgBkImg = QImage(); 
	m_sBkImg = QImage(); 
	m_sDocPath = QString("");

	m_pSceneMgr = new CRoom3DSceneMgr();
	m_p3DItemMgr = new CRoom3DItemMgr();
	m_pUndoMgr = new CRoom3DUndoMgr();
	m_bFirstInit = true;
}

CRoom3DDocument::~CRoom3DDocument(void)
{
	SAFE_DELETE(m_pSceneMgr);
	SAFE_DELETE(m_p3DItemMgr);
	SAFE_DELETE(m_pUndoMgr);
}

bool CRoom3DDocument::UpdateDoc(APP_MESSAGE a_msg, long a_lParam1, long a_lParam2, long a_lParam3)
{
	bool blResult = false;
	CRoom3DUndoCmd* pCmd = NULL;
	switch(a_msg)
	{
	case APP_MSG_LOAD_BACKGROUND_IMG:
		{
			CRoom3DLoadBackgroundImgCmd* pLoadBackImgCmd = new CRoom3DLoadBackgroundImgCmd();
			pLoadBackImgCmd->m_sPrevBackgroundImgPath = m_sOrgBkImgPath;
			pLoadBackImgCmd->m_sBackgroundImgPath = *((QString*)a_lParam1);
			pCmd = pLoadBackImgCmd;
		}
		break;
	case APP_MSG_CALIB_SETCOORDINATE:
		{
			CRoom3DCalibSetCoordinateCmd * pCalibSetCoordinate = new CRoom3DCalibSetCoordinateCmd();
			memcpy(pCalibSetCoordinate->m_fCoordinatePosition, (float*)a_lParam1, 4*2*sizeof(float));
			pCmd = pCalibSetCoordinate;
		}
		break;
	case APP_MSG_FURNITURE_ADD:
		{
			CRoom3DFurnitureAddCmd * pFurnitureCmd = new CRoom3DFurnitureAddCmd();
			if(m_pSceneMgr)
			{
				pFurnitureCmd->m_pFurniture = (void*)m_pSceneMgr->GetDragFurniture();
			}
			pCmd = pFurnitureCmd;
		}
		break;
	case APP_MSG_FURNITURE_DEL:
		{
			CRoom3DFurnitureDelCmd * pFurnitureCmd = new CRoom3DFurnitureDelCmd();
			if(m_pSceneMgr)
			{
				pFurnitureCmd->m_pFurniture = (void*)m_pSceneMgr->GetSelectedFurniture();
				if(pFurnitureCmd->m_pFurniture == NULL)
				{
					SAFE_DELETE(pFurnitureCmd);
					return blResult;
				}
			}
			pCmd = pFurnitureCmd;
		}
		break;
	case APP_MSG_FURNITURE_ROTATE:
		{
			CRoom3DFurnitureRotateCmd * pFurnitureRotate = new CRoom3DFurnitureRotateCmd();
			pFurnitureRotate->m_pFurniture = (CFurniture*)a_lParam1;
			pFurnitureRotate->m_fCurRotY = *((float*)a_lParam2);
			pFurnitureRotate->m_fPrevRotY = *((float*)a_lParam3);
			pCmd = pFurnitureRotate;
		}
		break;
	case APP_MSG_FURNITURE_MOVE:
		{
			CRoom3DFurnitureMoveCmd * pFurnitureMove = new CRoom3DFurnitureMoveCmd();
			pFurnitureMove->m_pFurniture = (CFurniture*)a_lParam1;
			pFurnitureMove->m_vCurPos = *((vec3*)a_lParam2);
			pFurnitureMove->m_vPrevPos = *((vec3*)a_lParam3);
			pCmd = pFurnitureMove;
		}
		break;
	case APP_MSG_CHANGE_BK_IMG_BRIGHT_AND_CONTRAST:
		{
			CRoom3DBkImgBrightCmd* pBrightCmd = new CRoom3DBkImgBrightCmd();
			pBrightCmd->m_nPrevBright = m_nBright;
			pBrightCmd->m_nPrevContrast = m_nContrast;
			pBrightCmd->m_nBright = (int) a_lParam1;
			pBrightCmd->m_nContrast = (int) a_lParam2;
			pCmd = pBrightCmd;
		}
		break;
	}
	if(pCmd)
	{
		// execute redo of the command and add undo command
		blResult = pCmd->Redo(); // update document
		if(!m_pUndoMgr->AddUndoCmd(pCmd))
			SAFE_DELETE(pCmd);
		m_blModifyFlg = true;
		Room3D* pMainWidget = g_pApp->GetMainWidget();
		pMainWidget->UpdateView();
		pMainWidget->UpdateControlState();
	}
	return blResult;
}

bool CRoom3DDocument::LoadBackImage(QString a_sBkImgPath)
{
	m_nBright = 50;
	m_nContrast = 50;
	m_sOrgBkImgPath = a_sBkImgPath;
	if(a_sBkImgPath.isEmpty())
	{
		m_sOrgBkImg = QImage();
	}
	else
	{
		m_sOrgBkImg.load(m_sOrgBkImgPath);
	}
	CRoom3DSceneViewWrapper* pViewWrapper = g_pApp->GetMainWidget()->GetSceneViewWrapper();
	if(pViewWrapper)
	{
		pViewWrapper->UpdateSceneVeiwLayout();
	}
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	resizeGL(viewport[2], viewport[3]);
	return true;
}

void CRoom3DDocument::SetBrightAndContrast(int a_nBright, int a_nContrast)
{
	m_nBright = a_nBright;
	m_nContrast = a_nContrast;
	MakeBkImageFromOrgImage();
}
void CRoom3DDocument::SetBright(int a_nBright)
{
	m_nBright = a_nBright;
	MakeBkImageFromOrgImage();
}

void CRoom3DDocument::SetContrast(int a_nContrast)
{
	m_nContrast = a_nContrast;
	MakeBkImageFromOrgImage();
}

void CRoom3DDocument::MakeBkImageFromOrgImage()
{
	QRect rect = g_pApp->GetSceneView()->GetViewportRect();
	m_sBkImg = m_sOrgBkImg;
	BrightenImage(&m_sBkImg, m_nBright);
	ContrastImage(&m_sBkImg, m_nContrast);
}


bool CRoom3DDocument::Undo(bool a_blUndo)
{
	if(a_blUndo)
		return m_pUndoMgr->Undo();
	else
		return m_pUndoMgr->Redo();
}

void CRoom3DDocument::initializeGL()
{
	if(m_pSceneMgr)
		m_pSceneMgr->initializeGL();
}

void CRoom3DDocument::resizeGL(int w, int h)
{
	if(m_pSceneMgr)
		m_pSceneMgr->resizeGL(w, h);

	if(!m_sOrgBkImg.isNull())
	{
		m_sOrgBkImg.load(m_sOrgBkImgPath);
		m_sOrgBkImg = m_sOrgBkImg.scaled(w, h);
		m_sOrgBkImg = m_sOrgBkImg.convertToFormat(QImage::Format_RGB888);
		m_sOrgBkImg = m_sOrgBkImg.mirrored();
		m_sBkImg = m_sOrgBkImg;

		// Redraw
		Room3D* pMainWidget = g_pApp->GetMainWidget();
		pMainWidget->UpdateView();
	}
	else
		m_sBkImg = QImage();
}

void CRoom3DDocument::Render()
{
	// Set up the rendering state
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA_TEST);

	glShadeModel(GL_SMOOTH); 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);	// GL_NICEST, GL_FASTEST, GL_DONT_CARE

	glCullFace(GL_BACK);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);	// GL_NICEST, GL_FASTEST, GL_DONT_CARE

	// Clear back buffer
	glClearColor(204/255.f, 199/255.f, 186/255.f, 0.0); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw Back Ground
	if(!m_sBkImg.isNull())
	{
		glDisable(GL_DEPTH_TEST);
		glDrawPixels(m_sBkImg.width(), m_sBkImg.height(), GL_RGB, GL_UNSIGNED_BYTE, m_sBkImg.bits());
		glEnable(GL_DEPTH_TEST);
	}

	// draw the scene:
 	if(m_pSceneMgr)
 		m_pSceneMgr->Render();
}

void CRoom3DDocument::InitDocument()
{
	m_nBright = 50;
	m_nContrast = 50;

	if(m_pSceneMgr)
	{
		m_pSceneMgr->Clear();
		m_pSceneMgr->GetCamera()->ResetParameters();
	}
	if(m_pUndoMgr)
		m_pUndoMgr->Clear();
	LoadBackImage(QString(""));
	m_sDocPath = QString("");
	m_blModifyFlg = false;

	if(m_bFirstInit)
	{
		m_p3DItemMgr->InitItemMgr();
	}
	m_bFirstInit = false;
}

void CRoom3DDocument::OnEnterDragItem(QDragMoveEvent *event, ITEM_INFO* pInfo)
{
	if(pInfo == NULL)
		return;

	if(m_pSceneMgr)
	{
		m_pSceneMgr->CreateDragFurniture(pInfo);
		OnMoveDragItem(event);
	}
}

void CRoom3DDocument::OnLeaveDragItem()
{
	if(m_pSceneMgr)
	{
		m_pSceneMgr->DeleteDragFurniture();
	}
}

void CRoom3DDocument::OnMoveDragItem(QDragMoveEvent *event)
{
	QPoint pt = event->pos();
	m_pSceneMgr->MoveDragFurniture(pt.x(), pt.y());

	// Redraw
	Room3D* pMainWidget = g_pApp->GetMainWidget();
	pMainWidget->UpdateView();
}

void CRoom3DDocument::OnDropDragItem()
{
	if(m_pSceneMgr)
	{
		m_pSceneMgr->DropDragFurniture();

		// Redraw
		Room3D* pMainWidget = g_pApp->GetMainWidget();
		pMainWidget->UpdateView();
	}
}

void CRoom3DDocument::AddFurniture(void* pFurniture)
{
	if(m_pSceneMgr)
	{
		m_pSceneMgr->AddFurniture((CFurniture*)pFurniture);
	}
}

void CRoom3DDocument::RemoveFurniture(void* pFurniture)
{
	if(pFurniture == NULL)
		return;

	if(m_pSceneMgr)
		m_pSceneMgr->RemoveFurniture((CFurniture*)pFurniture);
}

void CRoom3DDocument::BeginExporting()
{
	if(m_pSceneMgr)
		m_pSceneMgr->BeginExporting();
}

void CRoom3DDocument::EndExporting()
{
	if(m_pSceneMgr)
		m_pSceneMgr->EndExporting();
}

void CRoom3DDocument::ExportToHTML(const QString& fileName)
{
	if(m_pSceneMgr)
		m_pSceneMgr->ExportToHTML(fileName);
}

float CRoom3DDocument::GetBkImageAspectRatio()
{
	if(m_sOrgBkImg.isNull() || m_sOrgBkImg.height() == 0)
	{
		return 0.f;
	}
	return (float)m_sOrgBkImg.width() / (float)m_sOrgBkImg.height();
}

bool CRoom3DDocument::SaveDocument()
{
	QString fileName = m_sDocPath;
	if(fileName.isEmpty())
	{
		fileName = QFileDialog::getSaveFileName(g_pApp->GetMainWidget(), STR_SAVE_FILE_AS, "", STR_SAFE_FILE_FILTER);
		if(fileName.isEmpty())
		{
			return false;
		}
	}
	return SaveDocument(fileName);
}
bool CRoom3DDocument::SaveDocument(QString a_sFilePath)
{
	DOC_HEADER header;
	header.m_nVersion = ROOM3D_VERSION;
	header.m_sDocID = DOC_ID;
	QFile file(a_sFilePath);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	// write document id
	QByteArray sData = header.m_sDocID.toUtf8();
	int nSize = sData.size();
	file.write((char*) &nSize, sizeof(nSize));
	file.write(sData);
	// write version
	file.write((char*)&header.m_nVersion, sizeof(header.m_nVersion));
	// write background info
	sData = m_sOrgBkImgPath.toUtf8();
	nSize = sData.size();
	file.write((char*)&nSize, sizeof(nSize));
	file.write(sData);
	file.write((char*)&m_nBright, sizeof(m_nBright));
	file.write((char*)&m_nContrast, sizeof(m_nContrast));
	m_pSceneMgr->WriteData(&file);
	APP_STEP curStep = g_pApp->GetAppStep();
	file.write((char*)&curStep, sizeof(curStep));

	file.close();

	m_blModifyFlg = false;
	return true;
}

bool CRoom3DDocument::ReadDocument()
{
	QString fileName;
	fileName = QFileDialog::getOpenFileName(g_pApp->GetMainWidget(), STR_OPEN_FILE, "", STR_OPEN_FILE_FILTER);
	if(!fileName.isEmpty())
	{
		return ReadDocument(fileName);
	}
	return false;
}

bool CRoom3DDocument::ReadDocument(QString a_sFilePath)
{
	InitDocument();
	QFile file(a_sFilePath);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	// write document id
	QString sDocID;
	int nVersion;
	QByteArray sData;
	int nSize;
	file.read((char*) &nSize, sizeof(nSize));
	sData = file.read(nSize);
	sDocID = QString::fromUtf8(sData.data());
	if(sDocID.compare(DOC_ID) != 0)
		return false;
	// write version
	file.read((char*)&nVersion, sizeof(nVersion));
	// write background info
	file.read((char*) &nSize, sizeof(nSize));
	sData = file.read(nSize);
	m_sOrgBkImgPath = QString::fromUtf8(sData.data());
	file.read((char*)&m_nBright, sizeof(m_nBright));
	file.read((char*)&m_nContrast, sizeof(m_nContrast));
	m_pSceneMgr->ReadData(&file);

	APP_STEP curStep;
	file.read((char*)&curStep, sizeof(curStep));
	file.close();
	
	LoadBackImage(m_sOrgBkImgPath);
	SetBrightAndContrast(m_nBright, m_nContrast);
	g_pApp->SetAppStep(curStep);
	m_sDocPath = a_sFilePath;
	return true;
}
