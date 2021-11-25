#include "Room3DSceneMgr.h"
#include "Common.h"
#include "Room3DApplication.h"
#include <QDomDocument>
#include <QFile>

/************************************************************************/
/*  class : CFloor                                                      */
/*  desc: Virtual XZ plane                                              */
/************************************************************************/
CFloor::CFloor()
{
	m_bVisible = false;
	m_sColor = vec4(0.f, 0.f, 1.f, 0.2f);
	m_v0 = vec3(0.f, 0.f, 0.f);
	m_v1 = vec3(0.f, 0.f, 0.f);
	m_nTiles = 20;
}

CFloor::~CFloor()
{
}

void CFloor::Init(CCameraInfo* pCamera)
{
	if(pCamera == NULL)
		return;

	// Calc Floor Range
	mat4 mViewInv = mat4_id;
	mat4 mView = mat4_id;
	GLint viewPort[4];

	glGetIntegerv(GL_VIEWPORT, viewPort);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	pCamera->SetGLMatrix();
	glGetFloatv(GL_MODELVIEW_MATRIX, mView);
	invert(mViewInv, mView);
	glPopMatrix();

	m_v0 = vec3(0.f, 0.f, 0.f); // World Org Pos
	m_v1 = vec3(mViewInv.a03, mViewInv.a13, mViewInv.a23); // Camera Pos
	m_v1 *= 2.f;
	m_v1.y = 0.f;

	m_bVisible = false;
}

void CFloor::Render()
{
	// Render Floor
	if(m_bVisible)
	{
		vec3 vertex[4];
		int i = 0;
		vertex[0] = m_v0;
		vertex[1] = vec3(m_v0.x, 0.f, m_v1.z);
		vertex[2] = m_v1;
		vertex[3] = vec3(m_v1.x, 0.f, m_v0.z);

		glDisable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4fv(m_sColor);
		glBegin(GL_QUADS);
		for(i=0; i<4; i++)
		{
			glVertex3fv(vertex[i]);
		}
		glEnd();

		vec4 sLineColor = m_sColor;
		sLineColor.a = 0.1f;
		glColor4fv(sLineColor);
		
		vec3 vInterval = (m_v1 - m_v0)/(float)m_nTiles;
		for(i=0; i<=m_nTiles; i++)
		{
			// row
			vec3 vStart = m_v0;
			vStart.z += vInterval.z * i;
			vStart.y = 0;

			vec3 vEnd = vStart;
			vEnd.x = m_v1.x;
			
			glBegin(GL_LINES);
			glVertex3fv(vStart);
			glVertex3fv(vEnd);
			glEnd();

			// colume
			vStart = m_v0;
			vStart.x += vInterval.x * i;
			vStart.y = 0;

			vEnd = vStart;
			vEnd.z = m_v1.z;

			glBegin(GL_LINES);
			glVertex3fv(vStart);
			glVertex3fv(vEnd);
			glEnd();
		}

		glDisable(GL_BLEND);
		glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);
	}
}

void CFloor::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

/************************************************************************/
/* class : CRoom3DSceneMgr                                              */
/* desc :  3D Object Manager								            */
/************************************************************************/
CRoom3DSceneMgr::CRoom3DSceneMgr(void)
{
	m_pCurFurniture = NULL;
	m_pDragFurniture = NULL;
	m_bDragExact = false;
	m_bExporting = false;
}

CRoom3DSceneMgr::~CRoom3DSceneMgr(void)
{
	Clear();
}

void CRoom3DSceneMgr::initializeGL()
{
}

void CRoom3DSceneMgr::resizeGL(int w, int h)
{
	// setup viewport, projection etc.:
	GLfloat fRatio;

	glViewport(0,0,w,h);
	fRatio = (GLfloat)w/(GLfloat)h;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(m_Camera.GetFOV(), fRatio, CAMERA_NEAR, CAMERA_FAR);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	m_Camera.OnSize(w, h);
}

void CRoom3DSceneMgr::BeginExporting()
{
	if(m_pCurFurniture)
		m_pCurFurniture->Select(false);

	m_bExporting = true;
}

void CRoom3DSceneMgr::EndExporting()
{
	if(m_pCurFurniture)
		m_pCurFurniture->Select(true);

	m_bExporting = false;
}

void CRoom3DSceneMgr::ExportToHTML(const QString& fileName)
{
	CRoom3DItemMgr* pItemMgr = g_pApp->GetDocument()->GetItemMgr();

	if(pItemMgr == NULL)
		return;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QVector<SUMMARY_INFO> vecSceneInfo;
	QVector<CFurniture*>::Iterator it = m_vecFurnitures.begin();

	int nTotalNumber = 0;
	float fTotalPrice = 0.f;

	while(it != m_vecFurnitures.end())
	{
		CFurniture* pFurniture = (*it);
		if(pFurniture)
		{
			bool bFound = false;
			QString strFurnitureID = pFurniture->GetID();
			for(QVector<SUMMARY_INFO>::Iterator itInfo=vecSceneInfo.begin(); itInfo!=vecSceneInfo.end(); itInfo++)
			{
				if(itInfo->m_strFurnitureID == strFurnitureID)
				{
					bFound = true;
					itInfo->m_nNumber++;
				}
			}

			if(!bFound)
			{
				SUMMARY_INFO sInfo = {"", 0};
				sInfo.m_strFurnitureID = strFurnitureID;
				sInfo.m_nNumber++;
				vecSceneInfo.push_back(sInfo);
			}
		}
		it++;
	}

	QDomDocument doc;
	// Make Html
	QDomElement tagHtml = doc.createElement("html");
	
	// Make head
	QDomElement tagHead = doc.createElement("head");
	QDomElement tagMeta = doc.createElement("meta");
	tagMeta.setAttribute("http-equiv", "Content-Type");
	tagMeta.setAttribute("content", "text/html; charset=utf-8");
	QDomElement tagTitle = doc.createElement("title");
	QDomText txTitle = doc.createTextNode("report");
	tagTitle.appendChild(txTitle);
	tagHead.appendChild(tagMeta);
	tagHead.appendChild(tagTitle);
	tagHtml.appendChild(tagHead);

	// Make body
	QDomElement tagBody = doc.createElement("body");

	// Make table
	QDomElement tagTable = doc.createElement("table");
	
	// Make table head
	QDomElement tagTrHead = doc.createElement("tr");
	tagTrHead.setAttribute("bgcolor", "#00FFFF");
	QDomElement tagTdHead1 = doc.createElement("td");
	tagTdHead1.setAttribute("style", "font-weight:bold; color:#00F");
	QDomElement tagTdHead2 = doc.createElement("td");
	tagTdHead2.setAttribute("style", "font-weight:bold; color:#00F");
	QDomElement tagTdHead3 = doc.createElement("td");
	tagTdHead3.setAttribute("style", "font-weight:bold; color:#00F");

	QDomText txTdHead1 = doc.createTextNode("ProductID");
	QDomText txTdHead2 = doc.createTextNode("Number");
	QDomText txTdHead3 = doc.createTextNode("Price");

	tagTdHead1.appendChild(txTdHead1);
	tagTdHead2.appendChild(txTdHead2);
	tagTdHead3.appendChild(txTdHead3);

	tagTrHead.appendChild(tagTdHead1);
	tagTrHead.appendChild(tagTdHead2);
	tagTrHead.appendChild(tagTdHead3);

	tagTable.appendChild(tagTrHead);

	// Make table body
	for(QVector<SUMMARY_INFO>::Iterator itInfo=vecSceneInfo.begin(); itInfo!=vecSceneInfo.end(); itInfo++)
	{
		SUMMARY_INFO sInfo = (SUMMARY_INFO)(*itInfo);
		ITEM_INFO* pInfo = pItemMgr->GetItemInfo(sInfo.m_strFurnitureID);
		if(pInfo)
		{
			QDomElement tagTrBody = doc.createElement("tr");
			tagTrBody.setAttribute("bgcolor", "#999966");
			QDomElement tagTdBody1 = doc.createElement("td");
			QDomElement tagTdBody2 = doc.createElement("td");
			QDomElement tagTdBody3 = doc.createElement("td");

			QDomText txTdBody1 = doc.createTextNode(pInfo->m_sProductID);

			QString sNum;
			sNum.setNum(sInfo.m_nNumber);
			QDomText txTdBody2 = doc.createTextNode(sNum);
			
			QString sPrice;
			float fPrice = pInfo->m_fPrice * (float)sInfo.m_nNumber;
			sPrice.setNum(fPrice, 'f', 2);
			QDomText txTdBody3 = doc.createTextNode(sPrice);

			tagTdBody1.appendChild(txTdBody1);
			tagTdBody2.appendChild(txTdBody2);
			tagTdBody3.appendChild(txTdBody3);

			tagTrBody.appendChild(tagTdBody1);
			tagTrBody.appendChild(tagTdBody2);
			tagTrBody.appendChild(tagTdBody3);

			tagTable.appendChild(tagTrBody);

			nTotalNumber += sInfo.m_nNumber;
			fTotalPrice += fPrice;
		}
	}

	// Make table foot
	QDomElement tagTrFoot = doc.createElement("tr");
	tagTrFoot.setAttribute("bgcolor", "#66FF66");
	QDomElement tagTdFoot1 = doc.createElement("td");
	tagTdFoot1.setAttribute("style", "font-weight:bold; color:#330");
	QDomElement tagTdFoot2 = doc.createElement("td");
	tagTdFoot2.setAttribute("style", "font-weight:bold; color:#330");
	QDomElement tagTdFoot3 = doc.createElement("td");
	tagTdFoot3.setAttribute("style", "font-weight:bold; color:#330");

	QDomText txTdFoot1 = doc.createTextNode("Total");
	QString sTotalNum;
	sTotalNum.setNum(nTotalNumber);
	QDomText txTdFoot2 = doc.createTextNode(sTotalNum);
	QString sTotalPrice;
	sTotalPrice.setNum(fTotalPrice, 'f', 2);
	QDomText txTdFoot3 = doc.createTextNode(sTotalPrice);

	tagTdFoot1.appendChild(txTdFoot1);
	tagTdFoot2.appendChild(txTdFoot2);
	tagTdFoot3.appendChild(txTdFoot3);

	tagTrFoot.appendChild(tagTdFoot1);
	tagTrFoot.appendChild(tagTdFoot2);
	tagTrFoot.appendChild(tagTdFoot3);

	tagTable.appendChild(tagTrFoot);

	tagBody.appendChild(tagTable);
	tagHtml.appendChild(tagBody);

	doc.appendChild(tagHtml);

	QString strHtml = doc.toString();
	file.write(strHtml.toUtf8().data());
	file.close();
}

void CRoom3DSceneMgr::CalcTotalPrice(int& nTotalNumber, float& fTotalPrice)
{
	CRoom3DItemMgr* pItemMgr = g_pApp->GetDocument()->GetItemMgr();

	if(pItemMgr == NULL)
		return;

	QVector<CFurniture*>::Iterator it = m_vecFurnitures.begin();

	nTotalNumber = 0;
	fTotalPrice = 0.f;

	while(it != m_vecFurnitures.end())
	{
		CFurniture* pFurniture = (*it);
		if(pFurniture)
		{
			ITEM_INFO* pInfo = pItemMgr->GetItemInfo(pFurniture->GetID());
			if(pInfo)
			{
				nTotalNumber ++;
				fTotalPrice += pInfo->m_fPrice;
			}
		}
		it++;
	}
}

void CRoom3DSceneMgr::Render()
{
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	GetCamera()->SetGLMatrix();

	switch(g_pApp->GetAppStep())
	{
	case APP_EDIT_STEP_2:
		{
			RenderCoordinate();
		}
		break;
	case APP_EDIT_STEP_3:
		{
			if(m_bExporting)
			{
				// draw the furniture:
				RenderFurnitures();
			}
			else
			{
				// Render Floor
				m_sFloor.Render();

				// draw now dragging furnigure:
				RenderDragFurniture();

#if FPS_TEST /*002-->fps test*/
				RenderTest();
#else
				// draw the furniture:
				RenderFurnitures();
#endif
			}
		}
		break;
	}
}

void CRoom3DSceneMgr::RenderCoordinate()
{
	m_Camera.OnDraw();
}

#if FPS_TEST /*002-->fps test*/
void CRoom3DSceneMgr::RenderTest()
{
	for(int i=0; i<50000; i++)
	{
		CTexturePool* pTexturePool = g_pApp->GetDocument()->GetItemMgr()->GetTexturePool();
		if(pTexturePool)
			pTexturePool->SetTex(i%5);

		vec4 color;
		GetRandomVector(color, vec4(0, 0, 0, 0), vec4(1, 1, 1, 1));
		glMaterialfv(GL_FRONT, GL_AMBIENT, color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
		glMaterialfv(GL_FRONT, GL_SPECULAR, color);
		float power = GetRandomFloat(0.f, 1.f);
		glMaterialf(GL_FRONT, GL_SHININESS, pow(2, 10.0*power));

		glBegin(GL_TRIANGLES);
		vec3 normal;
		GetRandomVector(normal, vec3(-1, -1, -1), vec3(1, 1, 1));
		normal.norm();
		glNormal3fv(normal);

		for (int i=0; i<3; ++i)
		{
			GetRandomVector(normal, vec3(-1, -1, -1), vec3(1, 1, 1));
			normal.norm();
			glNormal3fv(normal);

			if (pTexturePool) 
			{
				vec2 texcoord;
				GetRandomVector(texcoord, vec2(0,0), vec2(1,1));
				glTexCoord2fv(texcoord);
			}

			vec3 vertex;
			GetRandomVector(vertex, vec3(-1, -1, -1), vec3(1, 1, 1));
			glVertex3fv(vertex);
		}
		glEnd();

		if(pTexturePool)
			glDisable(GL_TEXTURE_2D);
	}
}
#endif

void CRoom3DSceneMgr::RenderFurnitures()
{
	QVector<CFurniture*>::Iterator it = m_vecFurnitures.begin();

	glInitNames();
	glPushName(0);

	while(it != m_vecFurnitures.end())
	{
		CFurniture* pFurniture = (*it);
		pFurniture->Render(false);
		it++;
	}

	it = m_vecFurnitures.begin();
	while(it != m_vecFurnitures.end())
	{
		CFurniture* pFurniture = (*it);
		pFurniture->Render(true);
		it++;
	}
}

void CRoom3DSceneMgr::RenderDragFurniture()
{
	// draw now dragging furnigure:
	if(m_pDragFurniture)
	{
		m_pDragFurniture->Render(false);

		// draw expected position:
		if(!m_bDragExact)
		{
			vec3 vPos = m_pDragFurniture->GetPos();
			vPos.y = 0;

			glDisable(GL_LIGHTING);
			glEnable(GL_COLOR_MATERIAL);
			glColor3f(0.0f, 1.0f, 0.0f);
			glEnable(GL_POINT_SMOOTH);
			glPointSize(5.f);

			glBegin(GL_POINTS);
			glVertex3fv(vPos);
			glEnd();

			glDisable(GL_COLOR_MATERIAL);
			glDisable(GL_POINT_SMOOTH);
			glEnable(GL_LIGHTING);
		}
	}
}

void CRoom3DSceneMgr::CreateDragFurniture(ITEM_INFO* pInfo)
{
	if(pInfo == NULL || pInfo->m_pModel == NULL)
		return;

	SAFE_DELETE(m_pDragFurniture);
	m_pDragFurniture = new CFurniture;
	m_pDragFurniture->SetItem(pInfo);
}

void CRoom3DSceneMgr::DeleteDragFurniture()
{
	SAFE_DELETE(m_pDragFurniture);
}

void CRoom3DSceneMgr::MoveDragFurniture(int x, int y)
{
	vec3 vPos;
	CalcDragFurniturePos(vPos, (float)x, (float)y);

	if(m_pDragFurniture)
		m_pDragFurniture->SetPos(vPos);
}

void CRoom3DSceneMgr::CalcDragFurniturePos(vec3& vPos, float x, float y)
{
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	GetCamera()->SetGLMatrix();
	Ray pickRay = CalcPickingRay(x, y);
	glPopMatrix();

	vec4 plane(0.f, 1.f, 0.f, 0.f); //xz plane
	float fDist = RayPlaneIntersect(pickRay, plane);
	if(fDist != FLT_MAX && fDist >= 0.f)
		m_bDragExact = true;
	else
		m_bDragExact = false;

	if(m_bDragExact)
	{
		vec3 vP0 = pickRay.m_vPos + fDist * pickRay.m_vDir;
		vec3 vRay = pickRay.m_vDir;
		vec3 vSize = m_pDragFurniture->GetSize();
		float fY = vSize.y / 2.f;
#if PICK_TEST
		vPos = vP0 + (-1.f) * (fY/fabsf(vRay.y)) * vRay;
#else
		vP0.y = fY;
		vPos = vP0;
#endif
	}
	else
	{
		vPos = pickRay.m_vPos + FURNITURE_DEFAULT_DIST * pickRay.m_vDir;
	}
}

void CRoom3DSceneMgr::DropDragFurniture()
{
	if(m_pDragFurniture)
	{
		if(!m_bDragExact)
		{
			vec3 vPos = m_pDragFurniture->GetPos();
			vec3 vSize = m_pDragFurniture->GetSize();
			vPos.y = vSize.y / 2.f;
			m_pDragFurniture->SetPos(vPos);
		}

		SetSelectedFurniture(m_pDragFurniture);
		m_pDragFurniture = NULL;
	}
}

void CRoom3DSceneMgr::AddFurniture(CFurniture* pFurniture)
{
	if(pFurniture)
	{
		int nMaxID = 0;
		QVector<CFurniture*>::Iterator it = m_vecFurnitures.begin();

		while(it != m_vecFurnitures.end())
		{
			CFurniture* pTemp = (*it);
			if(pTemp)
			{
				if(nMaxID < pTemp->GetSelectID())
					nMaxID = pTemp->GetSelectID();
			}
			it++;
		}

		pFurniture->SetSelectID(nMaxID + 1);
		m_vecFurnitures.push_back(pFurniture);
	}
}

void CRoom3DSceneMgr::RemoveFurniture(CFurniture* pFurniture)
{
	QVector<CFurniture*>::Iterator it = m_vecFurnitures.begin();

	while(it != m_vecFurnitures.end())
	{
		if(pFurniture == (*it))
		{
			m_vecFurnitures.erase(it);
			break;
		}

		it++;
	}

	if(pFurniture == m_pCurFurniture)
	{
		pFurniture->Select(false);
		m_pCurFurniture = NULL;
	}
}

#define BUFFER_LENGTH 64

CFurniture* CRoom3DSceneMgr::GetHitFurniture(int x, int y)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); 
	CFurniture* pFurniture = NULL;

	GLuint selectBuf[BUFFER_LENGTH] = {0, };
	GLint hits = 0, viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(BUFFER_LENGTH, selectBuf);

	glRenderMode(GL_SELECT);
	glEnable(GL_DEPTH_TEST);

	mat4 matProj;
	glGetFloatv(GL_PROJECTION_MATRIX, matProj);

	// Projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((double)x, (double)(viewport[3] - y), 2.f, 2.f, viewport);
	glMultMatrixf(matProj);

	// Render
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	GetCamera()->SetGLMatrix();

	// draw the furniture:
	RenderFurnitures();

	hits = glRenderMode(GL_RENDER);

	// Restore
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	GLuint nSelectID = GetSelectID(hits, selectBuf, BUFFER_LENGTH);

	if(nSelectID != 0)
	{
		// Find Selected Furniture
		QVector<CFurniture*>::Iterator it = m_vecFurnitures.begin();

		while(it != m_vecFurnitures.end())
		{
			CFurniture* pTemp = (*it);
			if(pTemp && nSelectID == pTemp->GetSelectID())
			{
				pFurniture = pTemp;
				break;
			}
			it++;
		}
	}
	glPopAttrib(); 
	return pFurniture;
}

CCameraInfo* CRoom3DSceneMgr::GetCamera()
{
	return &m_Camera;
}
void CRoom3DSceneMgr::Clear()
{
	m_pCurFurniture = NULL;
	m_pDragFurniture = NULL;

	while(!m_vecFurnitures.isEmpty())
	{
		SAFE_DELETE(m_vecFurnitures.back());
		m_vecFurnitures.pop_back();
	}
}

void CRoom3DSceneMgr::InitFloor()
{
	m_sFloor.Init(&m_Camera);
}

bool CRoom3DSceneMgr::WriteData(QFile* a_pFile)
{
	int nCount = m_vecFurnitures.count();
	a_pFile->write((char*)&nCount, sizeof(nCount));
	QVector<CFurniture*>::iterator it = m_vecFurnitures.begin();
	while(it != m_vecFurnitures.end())
	{
		(*it)->WriteData(a_pFile);
		it++;
	}
	m_Camera.WriteData(a_pFile);
	return true;
}

bool CRoom3DSceneMgr::ReadData(QFile* a_pFile)
{
	int nCount = 0;
	a_pFile->read((char*)&nCount, sizeof(nCount));
	CRoom3DItemMgr* pItemMgr = g_pApp->GetDocument()->GetItemMgr();
	for( int i = 0; i < nCount; i++)
	{
		CFurniture* pFurniture = new CFurniture();
		pFurniture->ReadData(a_pFile);
		ITEM_INFO* pItemInfo = pItemMgr->GetItemInfo(pFurniture->GetID());
		if(pItemInfo)
		{
			pFurniture->SetItem(pItemInfo);
			pFurniture->RefreshAfterRead();
			AddFurniture(pFurniture);
		}
		else
			SAFE_DELETE(pFurniture);
	}
	m_Camera.ReadData(a_pFile);
	return true;
}

bool CRoom3DSceneMgr::UpdateFurnitures()
{
	CRoom3DItemMgr* pItemMgr = g_pApp->GetDocument()->GetItemMgr();
	QVector<CFurniture*>::Iterator it = m_vecFurnitures.begin();

	while(it != m_vecFurnitures.end())
	{
		CFurniture* pFurniture =  (*it);
		if(pFurniture->GetModel() == NULL)
		{
			ITEM_INFO* pItemInfo = pItemMgr->GetItemInfo(pFurniture->GetID());
			if(pItemInfo)
			{
				pFurniture->SetItem(pItemInfo);
				pFurniture->RefreshAfterRead();
				it++;
			}
			else
			{
				it = m_vecFurnitures.erase(it);
				SAFE_DELETE(pFurniture);
			}
		}
		else
			it++;
	}
	return true;
}