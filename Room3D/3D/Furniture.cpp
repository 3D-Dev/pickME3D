#include "Furniture.h"
#include "Common.h"
#include "Room3DApplication.h"
#include "Room3DItemMgr.h"

/************************************************************************/
/*  class : CGizmo                                                      */
/*  desc: Bounding Box attached mark                                    */
/************************************************************************/
CGizmo::CGizmo()
{
	m_nTex1 = -1;
	m_nTex2 = -1;
	m_bFocus = false;
	m_bVisible = true;
	m_vPos = vec2(0, 0);
	m_vSize = vec2(GIZMO_SIZE, GIZMO_SIZE);
	m_nType = GIZMO_TYPE_MAX;
}

CGizmo::~CGizmo()
{
}

void CGizmo::Render()
{
	if(m_bVisible == false)
		return;

	GLint viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);

	vec3 vertex[4];
	vertex[0] = m_vPos;
	vertex[1] = vec3(m_vPos.x, m_vPos.y + m_vSize.y, m_vPos.z);
	vertex[2] = vec3(m_vPos.x+m_vSize.x, m_vPos.y + m_vSize.y, m_vPos.z);
	vertex[3] = vec3(m_vPos.x+m_vSize.x, m_vPos.y, m_vPos.z);

	vec2 texCoord[4];
	texCoord[0] = vec2(0, 0);
	texCoord[1] = vec2(0, 1);
	texCoord[2] = vec2(1, 1);
	texCoord[3] = vec2(1, 0);

	if(m_vSize.x < 0)
	{
		// swap 3-0, 2-1
		vec2 temp = texCoord[0];
		texCoord[0] = texCoord[3];
		texCoord[3] = temp;

		temp = texCoord[1];
		texCoord[1] = texCoord[2];
		texCoord[2] = temp;
	}

	if(m_vSize.y < 0)
	{
		// swap 1-0, 3-2
		vec2 temp = texCoord[0];
		texCoord[0] = texCoord[1];
		texCoord[1] = temp;

		temp = texCoord[2];
		texCoord[2] = texCoord[3];
		texCoord[3] = temp;
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewPort[0], viewPort[0]+viewPort[2], viewPort[1], viewPort[1]+viewPort[3]);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//Render
	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	GLint nTexEnvMode = GL_MODULATE;
	CTexturePool* pTexturePool = g_pApp->GetDocument()->GetItemMgr()->GetTexturePool();

	if(m_bFocus)
		pTexturePool->SetTex(m_nTex1);
	else
		pTexturePool->SetTex(m_nTex2);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &nTexEnvMode);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);
	for(int i=0; i<4; i++)
	{
		glTexCoord2fv(texCoord[i]);
		glVertex3f(vertex[i].x, vertex[i].y, GIZMO_DEPTH * vertex[i].z);
	}
	glEnd();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, nTexEnvMode);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

bool CGizmo::SetPos(const vec3& vScrPos)
{
	bool bResult = true;
	m_vPos = vScrPos;
	GLint viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);

	vec3 vertex[4];
	vertex[0] = m_vPos;
	vertex[1] = vec3(m_vPos.x, m_vPos.y + m_vSize.y, m_vPos.z);
	vertex[2] = vec3(m_vPos.x+m_vSize.x, m_vPos.y + m_vSize.y, m_vPos.z);
	vertex[3] = vec3(m_vPos.x+m_vSize.x, m_vPos.y, m_vPos.z);

	for(int i=0; i<4; i++)
	{
		bResult &= ((vertex[i].x >= viewPort[0]) 
					&& (vertex[i].x <= viewPort[0]+viewPort[2])
					&& (vertex[i].y >= viewPort[1])
					&& (vertex[i].y <= viewPort[1]+viewPort[3]));
	}
	
	return bResult;
}

void CGizmo::SetSize(const vec2& vScrSize)
{
	m_vSize = vScrSize;
}

bool CGizmo::InRange(int x, int y)
{
	bool bRet = false;

	if(m_bVisible == false)
		return false;

	GLint viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);

	vec2 vPoint(x-viewPort[0], viewPort[3] - (y - viewPort[1]));
	vec2 vPos(m_vPos.x, m_vPos.y);

	vec2 vOffset = vPoint - vPos;

	bRet = ((vOffset.x * m_vSize.x) >= 0) 
			&& ((vOffset.y * m_vSize.y) >= 0) 
			&& (fabsf(vOffset.x) <= fabsf(m_vSize.x))
			&& (fabsf(vOffset.y) <= fabsf(m_vSize.y));

	return bRet;
}

void CGizmo::SetTextures(QString strTex1, QString strTex2)
{
	if(strTex1.isEmpty() || strTex2.isEmpty())
		return;

	CTexturePool* pTexturePool = g_pApp->GetDocument()->GetItemMgr()->GetTexturePool();
	m_nTex1 = pTexturePool->Load(strTex1.toUtf8().data());
	m_nTex2 = pTexturePool->Load(strTex2.toUtf8().data());
}

/************************************************************************/
/* class : CFurniture                                                   */
/* desc : Model data capsule class (Put model in 3D World)              */
/************************************************************************/
CFurniture::CFurniture(void)
{
	m_sID = QString();
	m_pModel = NULL;
	m_vPos = vec3(0.f, 0.f, 0.f);
	m_fRotY = 0.f;
	m_vSize = vec3(1.f, 1.f, 1.f);
	m_matWorld = mat4_id;

	m_matModel = mat4_id;

	m_nSelectID = -1;
	m_bSelect = false;
	m_bLock = false;

	InitBBox();

	m_aGizmos[GIZMO_UPDOWN].SetSize(vec2(GIZMO_SIZE, -(2 * GIZMO_SIZE)));
	m_aGizmos[GIZMO_UPDOWN].SetType(GIZMO_UPDOWN);
	m_aGizmos[GIZMO_UPDOWN].SetTextures(GIZMO_IMG_UPDOWN1, GIZMO_IMG_UPDOWN2);

	m_aGizmos[GIZMO_LOCK].SetSize(vec2(-GIZMO_SIZE, -GIZMO_SIZE));
	m_aGizmos[GIZMO_LOCK].SetType(GIZMO_LOCK);
	m_aGizmos[GIZMO_LOCK].SetTextures(GIZMO_IMG_LOCK1, GIZMO_IMG_LOCK2);

	m_aGizmos[GIZMO_UNLOCK].SetSize(vec2(-GIZMO_SIZE, -GIZMO_SIZE));
	m_aGizmos[GIZMO_UNLOCK].SetType(GIZMO_UNLOCK);
	m_aGizmos[GIZMO_UNLOCK].SetTextures(GIZMO_IMG_UNLOCK1, GIZMO_IMG_UNLOCK2);

	m_aGizmos[GIZMO_DELETE].SetSize(vec2(GIZMO_SIZE, -GIZMO_SIZE));
	m_aGizmos[GIZMO_DELETE].SetType(GIZMO_DELETE);
	m_aGizmos[GIZMO_DELETE].SetTextures(GIZMO_IMG_DELETE1, GIZMO_IMG_DELETE2);

	m_aGizmos[GIZMO_ROTATION].SetSize(vec2(GIZMO_SIZE, GIZMO_SIZE));
	m_aGizmos[GIZMO_ROTATION].SetType(GIZMO_ROTATION);
	m_aGizmos[GIZMO_ROTATION].SetTextures(GIZMO_IMG_ROTATION1, GIZMO_IMG_ROTATION2);

	m_aGizmos[GIZMO_LOCK].SetVisible(false);
}

CFurniture::~CFurniture(void)
{
}

void CFurniture::InitBBox()
{
	m_fBBoxDelta = 0.02f;
	m_sBBox.m_vCenter = vec3(0.f, 0.f, 0.f);
	m_sBBox.m_vAxis[0] = vec3(0.5f + m_fBBoxDelta, 0.f, 0.f);
	m_sBBox.m_vAxis[1] = vec3(0.f, 0.5f + m_fBBoxDelta, 0.f);
	m_sBBox.m_vAxis[2] = vec3(0.f, 0.f, 0.5f + m_fBBoxDelta);

	m_bBBoxAttachFloor = true;
	m_bBBoxDrawRed = false;
}

void CFurniture::SetItem(ITEM_INFO* pItemInfo)
{
	if(pItemInfo == NULL)
		return;

	vec3 vSize(pItemInfo->m_fLength/100.f, pItemInfo->m_fHeight/100.f, pItemInfo->m_fWidth/100.f);
	SetModel(pItemInfo->m_pModel);
	SetSize(vSize);
	m_sID = pItemInfo->m_sProductID;
}

void CFurniture::SetModel(CModel* pModel)
{
	m_pModel = pModel;
	CalcModelMatrix();
}

#define POS_EPSILON (FLT_EPSILON*100)
void CFurniture::SetPos(const vec3& vPos)
{ 
	m_vPos = vPos; 
	CalcWorldMatrix();

	// Update Bounding Box to Attach Floor
	if(m_bBBoxAttachFloor)
	{
		if(fabsf(m_vPos.y -m_vSize.y / 2.f)<=POS_EPSILON)
		{
			float fHeight = (m_vPos.y / m_vSize.y) + 0.5f;
			m_sBBox.m_vAxis[1] = vec3(0.f, fHeight/2.f + m_fBBoxDelta, 0.f);
			m_sBBox.m_vCenter = vec3(0.f, 0.5f - fHeight/2.f, 0.f);
		}
		else
		{
			if(m_vPos.y >= m_vSize.y / 2.f)
			{
				float fHeight = (m_vPos.y / m_vSize.y) + 0.5f;
				m_sBBox.m_vAxis[1] = vec3(0.f, fHeight/2.f + m_fBBoxDelta, 0.f);
				m_sBBox.m_vCenter = vec3(0.f, 0.5f - fHeight/2.f, 0.f);
				m_bBBoxDrawRed = false;
			}
			else
			{
				InitBBox();
				m_bBBoxDrawRed = true;
			}

		}
	}
}

void CFurniture::SetRot(float fRotY) 
{ 
	m_fRotY = fRotY; 
	CalcWorldMatrix();
}

void CFurniture::SetSize(vec3 vSize) 
{ 
	m_vSize = vSize; 
	CalcWorldMatrix();
}

void CFurniture::CalcWorldMatrix()
{
	mat4 matTrans = mat4_id;
	matTrans.set_translation(m_vPos);
	mat4 matRot = mat4_id;
	matRot.set_rot(m_fRotY * fto_rad, vec3(0,1,0));
	mat4 matScale = mat4_id;
	matScale.set_scale(m_vSize);

	m_matWorld = matTrans * matRot * matScale;
}

void CFurniture::CalcModelMatrix()
{
	if(m_pModel == NULL)
		return;

	vec3 vModelCenter = m_pModel->GetCenter();
	vec3 vModelSize = m_pModel->GetSize();

	mat4 matRot = mat4_id;
	matRot.set_rot(-90.f * fto_rad, vec3(1,0,0));
	mat4 matScale = mat4_id;
	matScale.set_scale(vec3(1.f/vModelSize.x, 1.f/vModelSize.y, 1.f/vModelSize.z));
	mat4 matTrans = mat4_id;
	matTrans.set_translation(vec3(-vModelCenter.x,- vModelCenter.y, -vModelCenter.z));

	m_matModel = matRot * matScale * matTrans;
}

vec3 CFurniture::CalcRayBoxIntersect(const Ray& ray)
{
	vec3 vRet(FLT_MAX, FLT_MAX, FLT_MAX);
	
	// Convert Ray to Object Space
	Ray rayObject;
	mat4 matWorldInv = mat4_id;
	invert(matWorldInv, m_matWorld);

	vec4 vDir(ray.m_vDir.x, ray.m_vDir.y, ray.m_vDir.z, 0.f); 
	vec4 vDirObject;
	mult(vDirObject, matWorldInv, vDir);
	if(vDirObject.w != 0)
		vDirObject = vDirObject / vDirObject.w;

	rayObject.m_vDir = vec3(vDirObject.x, vDirObject.y, vDirObject.z);
	rayObject.m_vDir.normalize();

	vec4 vPos(ray.m_vPos.x, ray.m_vPos.y, ray.m_vPos.z, 1.f); 
	vec4 vPosObject;
	mult(vPosObject, matWorldInv, vPos);
	if(vPosObject.w != 0)
		vPosObject = vPosObject / vPosObject.w;
	rayObject.m_vPos = vec3(vPosObject.x, vPosObject.y, vPosObject.z);

	float fDist = RayBoxIntersect(rayObject, m_sBBox);
	if(fDist!=FLT_MAX && fDist>=0)
	{
		vec3 vTemp = rayObject.m_vPos + fDist * rayObject.m_vDir;
		vPosObject = vec4(vTemp.x, vTemp.y, vTemp.z, 1.f);
		mult(vPos, m_matWorld, vPosObject);
		if(vPos.w != 0)
			vPos = vPos / vPos.w;

		vRet = vec3(vPos.x, vPos.y, vPos.z);
	}

#if PICK_TEST /*002*/
	m_vIntersectPos = vRet;
#endif

	return vRet;
}

void CFurniture::Render(bool bAlpha)
{
	if(m_pModel == NULL)
		return;

	glLoadName(m_nSelectID);

	glPushMatrix();
	glMultMatrixf(m_matWorld);
	RenderModel(bAlpha);

	if(bAlpha)
		RenderBox();

	glPopMatrix();

#if PICK_TEST /*002*/
	// Render Intersect pos
	if(m_bSelect)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glColor3f(1.0f, 1.0f, 0.0f);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(4.f);

		glBegin(GL_POINTS);
		glVertex3fv(m_vIntersectPos);
		glEnd();

		glDisable(GL_POINT_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}
#endif
}

void CFurniture::RenderModel(bool bAlpha)
{
	if(m_pModel == NULL)
		return;

	// This if expression is for testing, under the assumption that there is no model with material alpha property
	if(bAlpha == true)
		return;

	glPushMatrix();
	glMultMatrixf(m_matModel);

	glEnable(GL_LIGHTING);
	m_pModel->Render(bAlpha);
	glDisable(GL_LIGHTING);

	glPopMatrix();
}

void CFurniture::RenderBox()
{
	if(!m_bSelect)
		return;

	float alpha = 0.1f;

	// Calc Box Vertex Position
	vec3 aVertex[8];

	vec3 vAxisX = m_sBBox.m_vAxis[0];
	vec3 vAxisY = m_sBBox.m_vAxis[1];
	vec3 vAxisZ = m_sBBox.m_vAxis[2];

	vec3 aWinVertex[8];

	int i = 0;
	for(i=0; i<8 ; i++)
	{
		int nSignY = (int)(i/4)*2-1;
		int nSignX = (int)((i%4)/2)*2-1;
		int nSignZ = (int)((i%4)%2)*2 -1;
		aVertex[i] = m_sBBox.m_vCenter + (float)nSignX * vAxisX + (float)nSignY * vAxisY + (float)nSignZ * vAxisZ;
	}

	// Draw Box
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	glBegin(GL_QUADS);

	// bottom-gray
	if(m_bBBoxDrawRed)
		glColor4f(1.f, 0.f, 0.f, alpha);
	else
		glColor4f(0.5, 0.5, 0.5, alpha);
	glVertex3fv(aVertex[0]);
	glVertex3fv(aVertex[1]);
	glVertex3fv(aVertex[3]);
	glVertex3fv(aVertex[2]);

	// top-white
	if(m_bBBoxDrawRed)
		glColor4f(1.f, 0.f, 0.f, alpha);
	else
		glColor4f(1.0, 1.0, 1.0, alpha);
	glVertex3fv(aVertex[4]);
	glVertex3fv(aVertex[5]);
	glVertex3fv(aVertex[7]);
	glVertex3fv(aVertex[6]);

	// left-yellow
	if(m_bBBoxDrawRed)
		glColor4f(1.f, 0.f, 0.f, alpha);
	else
		glColor4f(1.0, 1.0, 0.0, alpha);
	glVertex3fv(aVertex[4]);
	glVertex3fv(aVertex[5]);
	glVertex3fv(aVertex[1]);
	glVertex3fv(aVertex[0]);

	// right-green
	if(m_bBBoxDrawRed)
		glColor4f(1.f, 0.f, 0.f, alpha);
	else
		glColor4f(0.0, 1.0, 0.0, alpha);
	glVertex3fv(aVertex[6]);
	glVertex3fv(aVertex[7]);
	glVertex3fv(aVertex[3]);
	glVertex3fv(aVertex[2]);

	// back-pink
	// right-green
	if(m_bBBoxDrawRed)
		glColor4f(1.f, 0.f, 0.f, alpha);
	else
		glColor4f(1.0, 0.0, 1.0, alpha);
	glVertex3fv(aVertex[4]);
	glVertex3fv(aVertex[0]);
	glVertex3fv(aVertex[2]);
	glVertex3fv(aVertex[6]);

	// front-cyan
	// right-green
	if(m_bBBoxDrawRed)
		glColor4f(1.f, 0.f, 0.f, alpha);
	else
		glColor4f(0.0, 1.0, 1.0, alpha);
	glVertex3fv(aVertex[5]);
	glVertex3fv(aVertex[1]);
	glVertex3fv(aVertex[3]);
	glVertex3fv(aVertex[7]);

	glEnd();

	// Draw Box Line
	vAxisX.x += (m_fBBoxDelta/10.f);
	vAxisY.y += (m_fBBoxDelta/10.f);
	vAxisZ.z += (m_fBBoxDelta/10.f);

	for(i=0; i<8 ; i++)
	{
		int nSignY = (int)(i/4)*2-1;
		int nSignX = (int)((i%4)/2)*2-1;
		int nSignZ = (int)((i%4)%2)*2 -1;
		aVertex[i] = m_sBBox.m_vCenter + (float)nSignX * vAxisX + (float)nSignY * vAxisY + (float)nSignZ * vAxisZ;
	}

	glLineWidth(1.0f);
	glPointSize(1.0f);
	glColor3f(0.0, 1.0, 0.0);

	// bottom
	glBegin(GL_LINE_LOOP);
	glVertex3fv(aVertex[0]);
	glVertex3fv(aVertex[1]);
	glVertex3fv(aVertex[3]);
	glVertex3fv(aVertex[2]);
	glEnd();

	// top
	glBegin(GL_LINE_LOOP);
	glVertex3fv(aVertex[4]);
	glVertex3fv(aVertex[5]);
	glVertex3fv(aVertex[7]);
	glVertex3fv(aVertex[6]);
	glEnd();

	// left
	glBegin(GL_LINES);
	glVertex3fv(aVertex[5]);
	glVertex3fv(aVertex[1]);
	glVertex3fv(aVertex[0]);
	glVertex3fv(aVertex[4]);

	// right
	glVertex3fv(aVertex[7]);
	glVertex3fv(aVertex[3]);
	glVertex3fv(aVertex[6]);
	glVertex3fv(aVertex[2]);
 	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	// Calc and Set Gizmo Position
	GLdouble matModelView[16] = {0,};
	GLdouble matProjection[16] = {0,};
	GLint viewPort[4];

	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
	glGetDoublev(GL_PROJECTION_MATRIX, matProjection);

	for(i=0; i<8 ; i++)
	{
		GLdouble x, y, z;
		gluProject(aVertex[i].x, aVertex[i].y, aVertex[i].z, matModelView, matProjection, viewPort, &x, &y, &z);
		aWinVertex[i].x = (float)x;
		aWinVertex[i].y = (float)y;
		aWinVertex[i].z = (float)z;
	}

	int nUpDownIndex = 5;
	if(m_aGizmos[GIZMO_UPDOWN].SetPos(aWinVertex[nUpDownIndex]) == false)
	{
		for(nUpDownIndex = 4; nUpDownIndex < 8; nUpDownIndex++)
		{
			if(m_aGizmos[GIZMO_UPDOWN].SetPos(aWinVertex[nUpDownIndex]))
				break;
		}

		if(nUpDownIndex >= 8)
		{
			nUpDownIndex = 5;
			m_aGizmos[GIZMO_UPDOWN].SetPos(aWinVertex[nUpDownIndex]);
		}
	}

	int nDelIndex = 7;
	if(m_aGizmos[GIZMO_LOCK].SetPos(aWinVertex[nDelIndex]) == false 
		|| m_aGizmos[GIZMO_UNLOCK].SetPos(aWinVertex[nDelIndex]) == false
		|| m_aGizmos[GIZMO_DELETE].SetPos(aWinVertex[nDelIndex]) == false)
	{
		for(nDelIndex = 4; nDelIndex < 8; nDelIndex++)
		{
			if(nDelIndex != nUpDownIndex)
			{
				if(m_aGizmos[GIZMO_LOCK].SetPos(aWinVertex[nDelIndex]) 
					&& m_aGizmos[GIZMO_UNLOCK].SetPos(aWinVertex[nDelIndex])
					&& m_aGizmos[GIZMO_DELETE].SetPos(aWinVertex[nDelIndex]))
					break;
			}
		}

		if(nDelIndex >= 8)
		{
			nDelIndex = 7;
			if(nDelIndex == nUpDownIndex)
				nDelIndex = 6;

			m_aGizmos[GIZMO_LOCK].SetPos(aWinVertex[nDelIndex]);
			m_aGizmos[GIZMO_UNLOCK].SetPos(aWinVertex[nDelIndex]);
			m_aGizmos[GIZMO_DELETE].SetPos(aWinVertex[nDelIndex]);
		}
	}

	int nRotIndex = 2;
	if(m_aGizmos[GIZMO_ROTATION].SetPos(aWinVertex[nRotIndex]) == false)
	{
		for(nRotIndex = 0; nRotIndex < 4; nRotIndex++)
		{
			if(m_aGizmos[GIZMO_ROTATION].SetPos(aWinVertex[nRotIndex]))
				break;
		}

		if(nRotIndex >= 4)
		{
			nRotIndex = 2;
			m_aGizmos[GIZMO_ROTATION].SetPos(aWinVertex[nRotIndex]);
		}
	}

	// Short Gizmos far to near
	int aIndex[GIZMO_TYPE_MAX];
	for(i=0; i<GIZMO_TYPE_MAX; i++)
		aIndex[i] = i;

	for(i=0; i<GIZMO_TYPE_MAX; i++)
	{
		int iMax = i;
		vec3 vPosMax = m_aGizmos[aIndex[iMax]].GetPos();

		for(int j=i+1; j<GIZMO_TYPE_MAX; j++)
		{
			vec3 vPos = m_aGizmos[aIndex[j]].GetPos();
			if(vPosMax.z < vPos.z)
			{
				vPosMax = vPos;
				iMax = j;
			}
		}

		// swap
		int iTemp = aIndex[i];
		aIndex[i] = aIndex[iMax];
		aIndex[iMax] = iTemp;
	}

	// Draw Gizmos
	for(i=0; i<GIZMO_TYPE_MAX; i++)
	{
		m_aGizmos[aIndex[i]].Render();
	}
}

void CFurniture::SetFocus(int x, int y)
{
	CGizmo* pGizmo = NULL;
	float fDepth = FLT_MAX;

	for(int i=0; i<GIZMO_TYPE_MAX; i++)
	{
		m_aGizmos[i].SetFocus(false);

		if(m_aGizmos[i].InRange(x, y))
		{
			vec3 vPos = m_aGizmos[i].GetPos();
			if(fDepth > vPos.z)
			{
				fDepth = vPos.z;
				pGizmo = &m_aGizmos[i];
			}
		}
	}

	if(pGizmo)
		pGizmo->SetFocus(true);
}

CGizmo* CFurniture::GetFocus()
{
	CGizmo* pGizmo = NULL;

	for(int i=0; i<GIZMO_TYPE_MAX; i++)
	{
		if(m_aGizmos[i].GetFocus())
		{
			pGizmo = &m_aGizmos[i];
		}
	}

	return pGizmo;
}

void CFurniture::SetLock(bool bLock)
{
	m_bLock = bLock;
	for(int i=0; i<GIZMO_TYPE_MAX; i++)
	{
		m_aGizmos[i].SetVisible(!m_bLock);
	}

	if(m_bLock)
		m_aGizmos[GIZMO_LOCK].SetVisible(true);
	else
		m_aGizmos[GIZMO_LOCK].SetVisible(false);
}

bool CFurniture::ReadData(QFile* a_pFile)
{
	QByteArray sData;
	int nSize;
	a_pFile->read((char*)&nSize, sizeof(nSize));
	sData = a_pFile->read(nSize);
	m_sID = QString::fromUtf8(sData.data());

	a_pFile->read((char*)&m_vPos, sizeof(m_vPos));
	a_pFile->read((char*)&m_fRotY, sizeof(m_fRotY));
	a_pFile->read((char*)&m_bLock, sizeof(m_bLock));

	return true;
}

void CFurniture::RefreshAfterRead()
{
	SetPos(m_vPos);
	SetRot(m_fRotY);
	SetLock(m_bLock);
}

bool CFurniture::WriteData(QFile* a_pFile)
{
	QByteArray sData = m_sID.toUtf8();
	int nSize = sData.size();
	a_pFile->write((char*)&nSize, sizeof(nSize));
	a_pFile->write(sData);

	a_pFile->write((char*)&m_vPos, sizeof(m_vPos));
	a_pFile->write((char*)&m_fRotY, sizeof(m_fRotY));
	a_pFile->write((char*)&m_bLock, sizeof(m_bLock));
	return true;
}
