#pragma once

#include "Global.h"
#include "Math3D.h"
#include "lib3ds/file.h"
#include "lib3ds/light.h"
#include "lib3ds/material.h"
#include "gl/gl.h"
#include "gl/glu.h"

class CModel
{
public:
	CModel(void);
	~CModel(void);

public:
	bool Load(char* pFileName);
	void Render(bool bAlpha = false);

	char* GetName() { return m_strFileName; }
	BoundBox GetBoundBox() { return m_sBB; }
	vec3 GetCenter() { return m_vCenter; }
	vec3 GetSize() { return m_vSize; }

protected:
	void CalcBoundingBox();
	void CreateDefaultLight();
	void LoadTextures();
	void LoadTexture(Lib3dsTextureMap* pTextureMap);

	void RenderNodes(bool bAlpha);
	void RenderNode(Lib3dsNode *pNode, bool bAlpha);

	void LightUpdate(Lib3dsLight *pLight);
	void SetLight();
protected:
	char m_strFileName[MAX_PATH_LEN];
	Lib3dsFile* m_pFile;
	
#if USE_MODEL_DISP_LIST
	// Display List
	GLuint m_nDispList;
#endif

	// Bounding Box
	BoundBox m_sBB;
	vec3 m_vCenter;
	vec3 m_vSize;
};
