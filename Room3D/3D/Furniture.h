#pragma once

#include "Model.h"
#include "Math3D.h"
#include <QString>
#include <QFile>

#define GIZMO_SIZE 18
#define GIZMO_DEPTH -0.01

struct ITEM_INFO;

typedef enum FURNITURE_GIZMO_TYPE
{
	GIZMO_ROTATION,
	GIZMO_UPDOWN,
	GIZMO_DELETE,
	GIZMO_LOCK,
	GIZMO_UNLOCK,
	GIZMO_TYPE_MAX
}FURNITURE_GIZMO_TYPE;

//////////////////////////////////////////////////////////////////////////
// class CGizmo
//////////////////////////////////////////////////////////////////////////
class CGizmo
{
public:
	CGizmo();
	~CGizmo();
	void Render();
	vec3 GetPos() { return m_vPos; }
	bool SetPos(const vec3& vScrPos);
	void SetSize(const vec2& vScrSize);
	bool InRange(int x, int y);
	bool GetFocus() { return m_bFocus; }
	void SetFocus(bool bFocus) { m_bFocus = bFocus; }
	int GetType() { return m_nType; }
	void SetType(int type) { m_nType = type; }

	void SetTextures(QString strTex1, QString strTex2);
	void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	bool GetVisible() { return m_bVisible; }
protected:
	vec3 m_vPos;
	vec2 m_vSize;
	bool m_bFocus;
	bool m_bVisible;
	GLuint m_nTex1;
	GLuint m_nTex2;
	int m_nType;

};

//////////////////////////////////////////////////////////////////////////
// class CFurniture
//////////////////////////////////////////////////////////////////////////
class CFurniture
{
public:
	CFurniture(void);
	~CFurniture(void);

public:
	void SetItem(ITEM_INFO* pItemInfo);
	void SetModel(CModel* pModel);
	void SetSelectID(int nSelectID){ m_nSelectID = nSelectID; }
	int GetSelectID() { return m_nSelectID; }
	void Select(bool bSelect) { m_bSelect = bSelect; }

	void Render(bool bAlpha=false);
	vec3 GetPos() const { return m_vPos; }
	float GetRot() { return m_fRotY; }
	vec3 GetSize() { return m_vSize; }
	void SetPos(const vec3& vPos);
	void SetRot(float fRotY);
	void SetSize(vec3 vSize);

	CModel* GetModel() {return m_pModel;}
	
	void SetFocus(int x, int y);
	CGizmo* GetFocus();

	void SetLock(bool bLock);
	bool GetLock() { return m_bLock; }

	vec3 CalcRayBoxIntersect(const Ray& ray);

	QString GetID() { return m_sID; }

	// for read/write
	bool ReadData(QFile* a_pFile);
	bool WriteData(QFile* a_pFile);
	void RefreshAfterRead();

protected:
	void RenderModel(bool bAlpha);
	void RenderBox();
	void CalcModelMatrix();
	void CalcWorldMatrix();
	void InitBBox();

protected:
	CModel* m_pModel;

	// Transform
	vec3 m_vPos;
	float m_fRotY;
	vec3 m_vSize;
	mat4 m_matWorld;

#if PICK_TEST /*002*/
	vec3 m_vIntersectPos;
#endif

	// Selection
	int m_nSelectID;
	bool m_bSelect;

	// Lock
	bool m_bLock;

	// Move model to bounding box center and scale to 1*1*1 matrix
	mat4 m_matModel;

	// Gizmo
	CGizmo m_aGizmos[GIZMO_TYPE_MAX];

	// Item Info ID
	QString m_sID;

	// Render and Pick Bounding Box
	float m_fBBoxDelta;
	OBoundBox m_sBBox;
	bool m_bBBoxAttachFloor;
	bool m_bBBoxDrawRed;
};
