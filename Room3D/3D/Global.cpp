#include "Global.h"
#include "gl/gl.h"
#include "gl/glu.h"

Ray CalcPickingRay(float x, float y)
{
	Ray ray;

	GLint viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);

	mat4 matProj;
	glGetFloatv(GL_PROJECTION_MATRIX, matProj);

	mat4 matView;
	glGetFloatv(GL_MODELVIEW_MATRIX, matView);

	mat4 matInverse;

	vec4 vPoint(0.f, 0.f, -1.f, 1.f);

	// Compute the picked point in projection space
	vPoint.x = ( ( ( 2.0f * (x - (float)viewPort[0]) ) / (float)viewPort[2]) - 1.f );
	vPoint.y = -( ( ( 2.0f * (y - (float)viewPort[1]) ) / (float)viewPort[3]) - 1.f );
	
	// Convert to view frustum near plane point
	invert(matInverse, matProj);
	vPoint = matInverse * vPoint;
	vPoint.x /= vPoint.w;
	vPoint.y /= vPoint.w;
	vPoint.z /= vPoint.w;

	// We get pick ray in view space vec3(vPoint.x, vPoint.y, vPoint.z), so we need it to convert to world space
	invert(matInverse, matView);
	ray.m_vDir.x = matInverse.a00 * vPoint.x + matInverse.a01 * vPoint.y + matInverse.a02 * vPoint.z;
	ray.m_vDir.y = matInverse.a10 * vPoint.x + matInverse.a11 * vPoint.y + matInverse.a12 * vPoint.z;
	ray.m_vDir.z = matInverse.a20 * vPoint.x + matInverse.a21 * vPoint.y + matInverse.a22 * vPoint.z;
	
	ray.m_vPos.x = matInverse.a03;
	ray.m_vPos.y = matInverse.a13;
	ray.m_vPos.z = matInverse.a23;

	ray.m_vDir.normalize();
	return ray;
}

float RayPlaneIntersect(const Ray& ray, const vec4& plane)
{
	float t = FLT_MAX;

	vec3 vNorm(plane.x, plane.y, plane.z);
	float normXpos = dot(vNorm, ray.m_vPos);
	float normXdir = dot(vNorm, ray.m_vDir);

	if(normXdir != 0) // not parallel
	{
		t = (plane.w - normXpos) / normXdir;
	}

	return t;
}

bool CheckPointInRect(const vec3& vPos, const ORect& rect)
{
	bool bRet = true;
	vec3 vOffset = vPos - rect.m_vCenter;
	for(int i=0 ; i<2; i++)
	{
		vec3 vAxis = rect.m_vAxis[i];
		float fAxisLen = vAxis.norm();
		vAxis.normalize();
		float fDot = fabsf(dot(vOffset, vAxis));
		bRet &= (fDot <= fAxisLen);
	}

	return bRet;
}

float RayRectIntersect(const Ray& ray, const ORect& rect)
{
	vec3 vNorm;
	cross(vNorm, rect.m_vAxis[0], rect.m_vAxis[1]);
	vNorm.normalize();
	float fD = dot(rect.m_vCenter, vNorm);

	float fDist = RayPlaneIntersect(ray, vec4(vNorm.x, vNorm.y, vNorm.z, fD));

	if(fDist!=FLT_MAX && fDist>=0)
	{
		vec3 vPos = ray.m_vPos + fDist * ray.m_vDir;
		if(CheckPointInRect(vPos, rect) == false)
			fDist = FLT_MAX;
	}
	else
	{
		fDist = FLT_MAX;
	}

	return fDist;
}

float RayBoxIntersect(const Ray& ray, const OBoundBox& box)
{
	float fResult = FLT_MAX;

	for(int i=0; i<6; i++)
	{
		ORect rect;
		int nAxis = i%3;
		int nAxisSign = 2 * (i/3) - 1;

		rect.m_vCenter = box.m_vCenter;
		rect.m_vCenter += nAxisSign * box.m_vAxis[nAxis];
		rect.m_vAxis[0] = box.m_vAxis[(nAxis+1)%3];
		rect.m_vAxis[1] = box.m_vAxis[(nAxis+2)%3];

		float fDist = RayRectIntersect(ray, rect);
		if(fDist < fResult)
			fResult = fDist;
	}

	return fResult;
}

float RayBoxIntersect(const Ray& ray, const BoundBox& box)
{
	OBoundBox oBox;
	ConvBBtoOBB(oBox, box);
	return RayBoxIntersect(ray, oBox);
}

void ConvBBtoOBB(OBoundBox& outBox, const BoundBox& box)
{
	vec3 vCenter = (box.m_vMin + box.m_vMax)/2.f;
	vec3 vOffset = vCenter - box.m_vMin;
	outBox.m_vCenter = vCenter;
	outBox.m_vAxis[0] = vec3(vOffset.x, 0, 0);
	outBox.m_vAxis[1] = vec3(0, vOffset.y, 0);
	outBox.m_vAxis[2] = vec3(0, 0, vOffset.z);
}

vec3 ConvWinDeltaToWorldDelta(const vec2& vScrDelta, const vec3& vWorldPos)
{
	vec3 vDeltaWorld(0.f, 0.f, 0.f);

	GLint viewPort[4];
	glGetIntegerv(GL_VIEWPORT, viewPort);

	mat4 matProj;
	glGetFloatv(GL_PROJECTION_MATRIX, matProj);

	mat4 matView;
	glGetFloatv(GL_MODELVIEW_MATRIX, matView);

	mat4 matInverse;

	vec4 vDeltaView(0.f, 0.f, 0.f, 0.f);

	// Compute the delta in projection space
	vDeltaView.x = ( 2.0f * vScrDelta.x / (float)viewPort[2]);
	vDeltaView.y = -( 2.0f * vScrDelta.y / (float)viewPort[3]);

	invert(matInverse, matProj);

	// projection space front plane center --> near plane
	vec4 v1(0.f, 0.f, -1.f, 1.f);
	vec4 v2 = v1 + vDeltaView;

	// Compute view space pos in near plane
	v1 = matInverse * v1;
	if(v1.w != 0)
		v1 = v1 / v1.w;

	v2 = matInverse * v2;
	if(v2.w != 0)
		v2 = v2 / v2.w;

	vDeltaView = v2 - v1;

	// Compute view space position
	vec3 vPos = vWorldPos;
	vec4 vPosView(vPos.x, vPos.y, vPos.z, 1.f);
	vPosView = matView * vPosView;
	if(vPosView.w != 0)
		vPosView = vPosView / vPosView.w;

	// Calc scale (view space pos z / near plane z) and view space delta
	float fScale = vPosView.z / v1.z;
	vDeltaView *= fScale;

	// Calc world space delta
	invert(matInverse, matView);
	vDeltaWorld = matInverse * vDeltaView;

	return vDeltaWorld;
}

UINT GetSelectID(UINT nHits, UINT* pSelectBuf, UINT nBufferSize)
{
	UINT nSelectID = 0;

	if(pSelectBuf && nHits > 0)
	{
		UINT blockIndex = 0;
		UINT index = 0;
		UINT nMinZ = UINT_MAX;

		while(blockIndex < nHits && index < nBufferSize)
		{
			UINT count = pSelectBuf[index];
			UINT blockSize = 3 + count;
			UINT minZ = pSelectBuf[index + 1];
			if(minZ < nMinZ)
			{
				nMinZ = minZ;
				nSelectID = pSelectBuf[index + 3];
			}

			index += blockSize;
			blockIndex ++;
		}
	}

	return nSelectID;
}