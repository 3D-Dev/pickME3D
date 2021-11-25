#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <windows.h>
#include "Math3D.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    

#define MAX_PATH_LEN 256

#define CAMERA_FOV 45.f
#define CAMERA_NEAR 0.1f
#define CAMERA_FAR 10000.f

#define USE_MODEL_DISP_LIST 0
#define USE_THREAD 1

#define CALIBRATION_SCALE 1.0f

#define TEST 0
#if TEST
#define PICK_TEST 1
#define CAMERA_TEST 1
#define FPS_TEST 1
#define SHOW_FPS 1
#else
#define PICK_TEST 0
#define SHOW_CALIB_AXIS 0
#define FPS_TEST 0
#define SHOW_FPS 0
#endif

struct Ray
{
	vec3 m_vPos;
	vec3 m_vDir;
};

struct BoundBox
{ 
	vec3 m_vMin;
	vec3 m_vMax;
};

struct ORect
{
	vec3 m_vCenter;
	vec3 m_vAxis[2];
};

struct OBoundBox
{
	vec3 m_vCenter;
	vec3 m_vAxis[3];
};

Ray CalcPickingRay(float x, float y);
float RayPlaneIntersect(const Ray& ray, const vec4& plane);
bool CheckPointInRect(const vec3& vPos, const ORect& rect);
float RayRectIntersect(const Ray& ray, const ORect& rect);
float RayBoxIntersect(const Ray& ray, const OBoundBox& box);
float RayBoxIntersect(const Ray& ray, const BoundBox& box);

void ConvBBtoOBB(OBoundBox& outBox, const BoundBox& box);
vec3 ConvWinDeltaToWorldDelta(const vec2& vScrDelta, const vec3& vWorldPos);
UINT GetSelectID(UINT nHits, UINT* pSelectBuf, UINT nBufferSize);

#endif //_GLOBAL_H_