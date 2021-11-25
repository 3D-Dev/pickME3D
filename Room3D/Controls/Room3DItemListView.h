//----------------------------------------------//
//	File: Room3DItemListView.h								//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QListWidget>
#include "Room3DItemDelegate.h"
struct ITEM_CATEGORY_INFO;
struct ITEM_SEARCH_INFO;
/*!
	\class CRoom3DItemListView
	\brief This is list view class to display 3d item's image.(by 001, 2012/6/21).
	\sa 
*/
class CRoom3DItemListView : public QListWidget
{
	Q_OBJECT
public:
	CRoom3DItemListView(QWidget * parent = 0);
	~CRoom3DItemListView(void);
	void SetCategoryInfo(ITEM_CATEGORY_INFO* a_pInfo);
	int GetCategoryCode(){return m_nCategoryCode;}
	ITEM_SEARCH_INFO* GetSearchInfo(){return m_pSearchInfo;}
	void SelectItem(QString a_sProductID);
	int FindItemWidget(QString a_sProductID);
protected slots:
	void OnDeleteItem();
	void OnImportItem();
protected:
	virtual void mouseDoubleClickEvent ( QMouseEvent * event );
	virtual void startDrag(Qt::DropActions supportedActions);
	virtual QStringList mimeTypes() const;
	virtual void contextMenuEvent(QContextMenuEvent *event);
protected:
	int						m_nCategoryCode;
	ITEM_CATEGORY_INFO*		m_pCategoryInfo;
	CRoom3DItemDelegate*	m_pItemDelegate;
	ITEM_SEARCH_INFO*		m_pSearchInfo;
};
