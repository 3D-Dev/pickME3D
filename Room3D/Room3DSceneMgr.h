//----------------------------------------------//
//	File: Room3DSceneMgr.h						//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QVector>
#include <QMouseEvent>
#include <QWheelEvent>
#include "Furniture.h"
#include "CameraInfo.h"

class CRoom3DStep3Cmd;
/*!
	\class CRoom3DDocument
	\brief This is class to manage all 3D data.(by 001, 2012/6/23).
	\sa 
*/
#define FURNITURE_DEFAULT_DIST 10.f

//////////////////////////////////////////////////////////////////////////
// class CFloor
//////////////////////////////////////////////////////////////////////////
class CFloor
{
public:
	CFloor();
	~CFloor();

	void Init(CCameraInfo* pCamera);
	void SetColor(const vec4& color) { m_sColor = color; }
	void Render();
	void SetVisible(bool bVisible);
	bool IsVisible(){ return m_bVisible; }

protected:
	bool m_bVisible;
	vec4 m_sColor;
	vec3 m_v0;
	vec3 m_v1;
	int m_nTiles;
};

//////////////////////////////////////////////////////////////////////////
// class CRoom3DSceneMgr
//////////////////////////////////////////////////////////////////////////
typedef struct SUMMARY_INFO 
{
	QString m_strFurnitureID;
	int m_nNumber;
}SUMMARY_INFO;

class CRoom3DSceneMgr
{
	friend class CRoom3DStep3Cmd;
public:
	CRoom3DSceneMgr(void);
	~CRoom3DSceneMgr(void);

public:
	void initializeGL();
	void resizeGL(int w, int h);
	void Render();
	void Clear();

	void CreateDragFurniture(ITEM_INFO* pInfo);
	void DeleteDragFurniture();
	void MoveDragFurniture(int x, int y);
	void DropDragFurniture();

	void AddFurniture(CFurniture* pFurniture);
	void RemoveFurniture(CFurniture* pFurniture);

	CFurniture* GetDragFurniture() { return m_pDragFurniture; }

	CCameraInfo* GetCamera();

	CFurniture* GetHitFurniture(int x, int y);
	CFurniture* GetSelectedFurniture(){return m_pCurFurniture;}
	void SetSelectedFurniture(CFurniture* a_pFurniture)
	{
		if(m_pCurFurniture)
			m_pCurFurniture->Select(false);
		m_pCurFurniture = a_pFurniture;
		if(m_pCurFurniture)
			m_pCurFurniture->Select(true);
	}

	void InitFloor();
	CFloor* GetFloor() { return &m_sFloor; }

	// Image Export function
	void BeginExporting();
	void EndExporting();

	// XML Export function
	void ExportToHTML(const QString& fileName);

	void CalcTotalPrice(int& nTotalNumber, float& fTotalPrice);

	// document read/write function
	bool WriteData(QFile* a_pFile);
	bool ReadData(QFile* a_pFile);
	bool UpdateFurnitures();

protected:
	void RenderFurnitures();
	void RenderDragFurniture();
	void RenderCoordinate();
#if FPS_TEST /*002-->fps test*/
	void RenderTest();
#endif

public:
	void CalcDragFurniturePos(vec3& vPos, float x, float y);

protected:
	QVector<CFurniture*> m_vecFurnitures;
	CFurniture* m_pCurFurniture; // Selected furniture
	CFurniture* m_pDragFurniture; // Now dragging furniture
	bool m_bDragExact; // Dragging follow floor
	CFloor m_sFloor; // Floor

	CCameraInfo m_Camera;

	bool m_bExporting;
};
