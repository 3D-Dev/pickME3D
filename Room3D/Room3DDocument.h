//----------------------------------------------//
//	File: Room3DFurnitureMgr.h						//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include "Common.h"
#include "Define.h"
#include <QImage>
#include "Room3DItemMgr.h"
#include "Room3DSceneMgr.h"

class CRoom3DUndoMgr;
/*!
	\class CRoom3DDocument
	\brief This is class to manage all data.(by 001, 2012/6/23).
	\sa 
*/
class CRoom3DDocument
{
	friend class CRoom3DUndoCmd;
public:
	CRoom3DDocument(void);
	~CRoom3DDocument(void);
public:
	// Initialize and render
	void InitDocument();
	void initializeGL();
	void resizeGL(int w, int h);
	void Render();

	// Change documtne contens with undo/redo
	bool UpdateDoc(APP_MESSAGE a_msg, long a_lParam1 = 0, long a_lParam2 = 0, long a_lParam3 = 0);
	bool Undo(bool a_blUndo);

	// Change background image
	bool LoadBackImage(QString a_sBkImgPath);
	void SetBright(int a_nBright);
	void SetContrast(int a_nContrast);
	void SetBrightAndContrast(int a_nBright, int a_nContrast);
	void GetBrightAndContrast(int& a_nBright, int& a_nContrast){a_nBright = m_nBright; a_nContrast = m_nContrast;}
	QImage* GetBackImage(){return &m_sBkImg;}


	// Get/Set function
	CRoom3DItemMgr* GetItemMgr(){return m_p3DItemMgr;}
	CRoom3DSceneMgr* Get3DSceneMgr(){ return m_pSceneMgr; }
	CRoom3DUndoMgr* GetUndoMgr(){return m_pUndoMgr;}
	float GetBkImageAspectRatio();

	// Event function
	void OnEnterDragItem(QDragMoveEvent *event, ITEM_INFO* pInfo);
	void OnLeaveDragItem();
	void OnMoveDragItem(QDragMoveEvent *event);
	void OnDropDragItem();

	// Item edit function
	void AddFurniture(void* pFurniture);
	void RemoveFurniture(void* pFurniture);

	// Image Export function
	void BeginExporting();
	void EndExporting();

	// XML Export function
	void ExportToHTML(const QString& fileName);

	bool SaveDocument();
	bool ReadDocument();
	bool IsModified(){return m_blModifyFlg;}
protected:
	void MakeBkImageFromOrgImage();
	bool SaveDocument(QString a_sFilePath);
	bool ReadDocument(QString a_sFilePath);
protected:
	QImage	m_sOrgBkImg;
	QImage	m_sBkImg;
	int		m_nBright;
	int		m_nContrast;
	QString m_sOrgBkImgPath;
	QString m_sDocPath;
	bool	m_blModifyFlg;

	CRoom3DSceneMgr* m_pSceneMgr;
	CRoom3DItemMgr* m_p3DItemMgr;

	CRoom3DUndoMgr* m_pUndoMgr;
	bool m_bFirstInit;

};
