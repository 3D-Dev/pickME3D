#include "Room3DStep3Cmd.h"
#include "Room3DDocument.h"
#include "Room3DApplication.h"


CRoom3DStep3Cmd::CRoom3DStep3Cmd(void)
{
	m_bRBFlag = false;
	m_bLBFlag = false;
	m_pDocument->Get3DSceneMgr()->InitFloor();
}

CRoom3DStep3Cmd::~CRoom3DStep3Cmd(void)
{
}

void CRoom3DStep3Cmd::ResetDocument()
{
	m_pDocument->Get3DSceneMgr()->Clear();
}

void CRoom3DStep3Cmd::MousePressEvent(QMouseEvent * a_pEvent)
{
	CRoom3DSceneMgr* pSceneMgr = m_pDocument->Get3DSceneMgr();
	CFurniture* pFurniture = pSceneMgr->GetSelectedFurniture();

	if(pFurniture)
	{
		if(a_pEvent->button() == Qt::LeftButton)
		{
			pFurniture->SetFocus(a_pEvent->x(), a_pEvent->y());
			
			CGizmo* pGizmo = pFurniture->GetFocus();

			if(pGizmo == NULL)
			{
				pFurniture = pSceneMgr->GetHitFurniture(a_pEvent->x(), a_pEvent->y());
				pSceneMgr->SetSelectedFurniture(pFurniture);
			}
		}
	}
	else
	{
		pFurniture = pSceneMgr->GetHitFurniture(a_pEvent->x(), a_pEvent->y());
		pSceneMgr->SetSelectedFurniture(pFurniture);
	}

	if(pFurniture)
	{
		m_startPos = pFurniture->GetPos();
		m_fStartRotY = pFurniture->GetRot();

		Ray ray = CalcPickingRay(a_pEvent->x(), a_pEvent->y());
		vec3 vPickPos;
		vPickPos = pFurniture->CalcRayBoxIntersect(ray);
		vec3 vPos = pFurniture->GetPos();
		m_deltaPos = vPickPos - vPos;
	}

	m_nPrevX = a_pEvent->x();
	m_nPrevY = a_pEvent->y();

	if(a_pEvent->button() == Qt::LeftButton)
	{
		m_bLBFlag = true;
	}
	if(a_pEvent->button() == Qt::RightButton)
	{
		m_bRBFlag = true;
	}

	g_pApp->GetSceneView()->updateGL();
}

void CRoom3DStep3Cmd::MouseReleaseEvent(QMouseEvent * a_pEvent)
{
	CRoom3DSceneMgr* pSceneMgr = m_pDocument->Get3DSceneMgr();
	CFurniture* pFurniture = pSceneMgr->GetSelectedFurniture();

	if(pFurniture && m_bLBFlag)
	{
		CGizmo* pGizmo = pFurniture->GetFocus();
		if(pGizmo)
		{
			pGizmo->SetFocus(false);

			switch(pGizmo->GetType())
			{
			case GIZMO_UPDOWN:
				{
					vec3 pos = pFurniture->GetPos();
					g_pApp->GetDocument()->UpdateDoc(APP_MSG_FURNITURE_MOVE, (long) pFurniture, (long) &pos, (long)&m_startPos);
				}
				break;
			case GIZMO_ROTATION:
				{
					float fRot = pFurniture->GetRot();
					g_pApp->GetDocument()->UpdateDoc(APP_MSG_FURNITURE_ROTATE, (long)pFurniture, (long)&fRot, (long)&m_fStartRotY);
				}
				break;
			case GIZMO_DELETE:
				{
					g_pApp->GetDocument()->UpdateDoc(APP_MSG_FURNITURE_DEL);
				}
				break;
			case GIZMO_LOCK:
				{
					pFurniture->SetLock(false);
					g_pApp->GetSceneView()->updateGL();
				}
				break;
			case GIZMO_UNLOCK:
				{
					pFurniture->SetLock(true);
					g_pApp->GetSceneView()->updateGL();
				}
				break;
			}
		}
		else
		{
			if(pFurniture->GetLock() == false)
			{
				vec3 pos = pFurniture->GetPos();
				g_pApp->GetDocument()->UpdateDoc(APP_MSG_FURNITURE_MOVE, (long) pFurniture, (long) &pos, (long)&m_startPos);
			}
		}
	}

	m_bRBFlag = false;
	m_bLBFlag = false;
}

void CRoom3DStep3Cmd::MouseMoveEvent(QMouseEvent * a_pEvent)
{
	CRoom3DSceneMgr* pSceneMgr = m_pDocument->Get3DSceneMgr();
	CFurniture* pFurniture = pSceneMgr->GetSelectedFurniture();

	QRect rect = g_pApp->GetSceneView()->geometry();

	if(pFurniture)
	{
		if(m_bLBFlag)
		{
			CGizmo* pGizmo = pFurniture->GetFocus();
			if(pGizmo)
			{
				switch(pGizmo->GetType())
				{
				case GIZMO_UPDOWN:
					{
						vec3 vPos = pFurniture->GetPos();
						vec2 vDelta(0.f, (float)(a_pEvent->y() - m_nPrevY));
						vec3 vOffsetWorld = ConvWinDeltaToWorldDelta(vDelta, vPos);
						vOffsetWorld.x = vOffsetWorld.z = 0.f;
						vPos += vOffsetWorld;
						pFurniture->SetPos(vPos);
					}
					break;
				case GIZMO_ROTATION:
					{
						float fScale = 360.f * 3 / rect.width();
						float fRotY = pFurniture->GetRot() + (a_pEvent->x() - m_nPrevX) * fScale;		
						fRotY = fRotY - 360 * ((int)(fRotY / 360.f));
						pFurniture->SetRot(fRotY);
					}
					break;
				}
			}
			else
			{
				if(pFurniture->GetLock() == false)
				{
					vec3 vPos = pFurniture->GetPos();
					Ray pickray = CalcPickingRay(a_pEvent->x(), a_pEvent->y());

					vec3 vPickPos = vPos + m_deltaPos;
					float fDist = RayPlaneIntersect(pickray, vec4(0, 1, 0, vPickPos.y));

					if(fDist!=FLT_MAX && fDist >= 0)
					{
						vec3 vTargetPos;
						vTargetPos = pickray.m_vPos + fDist * pickray.m_vDir;
						vTargetPos = vTargetPos - m_deltaPos;
						pFurniture->SetPos(vTargetPos);
					}
				}
			}
		}
		else
		{
			pFurniture->SetFocus(a_pEvent->x(), a_pEvent->y());
		}
	}

	g_pApp->GetSceneView()->updateGL();

	m_nPrevX = a_pEvent->x();
	m_nPrevY = a_pEvent->y();
}

void CRoom3DStep3Cmd::WheelEvent(QWheelEvent* a_pEvent)
{
	int nDelta = a_pEvent->delta();
	CRoom3DSceneMgr* pSceneMgr = m_pDocument->Get3DSceneMgr();
	CFurniture* pFurniture = pSceneMgr->GetSelectedFurniture();
	if(pFurniture)
	{
		m_startPos = pFurniture->GetPos();
		vec3 pos = pFurniture->GetPos();
		float fScale = 0.005f;
		pos.z += nDelta * fScale;
		g_pApp->GetDocument()->UpdateDoc(APP_MSG_FURNITURE_MOVE, (long) pFurniture, (long) &pos, (long)&m_startPos);
	}
}

void CRoom3DStep3Cmd::keyPressEvent ( QKeyEvent * a_pEvent )
{
	switch (a_pEvent->key()) 
	{
	 case Qt::Key_Delete:
		 {
			 g_pApp->GetDocument()->UpdateDoc(APP_MSG_FURNITURE_DEL);
		 }
		 break;
	 default:
		 break;
	}
}

void CRoom3DStep3Cmd::keyReleaseEvent ( QKeyEvent * a_pEvent )
{
	switch (a_pEvent->key()) 
	{
	case Qt::Key_Delete:
		break;
	default:
		break;
	}
}
