#pragma once
#include "Room3DBaseDlg.h"
#include "Room3DImageButton.h"
#include "Room3DItemMgr.h"
#include "Room3DItemDelegate.h"

class CRoom3DItemCheckBox : public QCheckBox
{
	Q_OBJECT
public:
	CRoom3DItemCheckBox(QWidget* a_pParent) : QCheckBox(a_pParent)
	{
	};
	~CRoom3DItemCheckBox(){};
protected:
};

class CRoom3DMoreItemsDlg : public CRoom3DBaseDlg
{
	Q_OBJECT
public:
	CRoom3DMoreItemsDlg(QWidget* a_pParent);
	~CRoom3DMoreItemsDlg(void);
protected:
	void InitControls();
protected slots:
	void verify();
	void OnPressed(int a_nBtnID);
	void OnUpdateItemTableWidget(bool error);
	void OnUpdateItemInfoImage(int a_nRow, QPixmap& a_pixmap, bool error);
	void OnSelectAll(int a_nState);
protected:
	void GetSelectedItemInfoList(QVector<ITEM_INFO_EX*>& a_pSelItems);

protected:
	QVBoxLayout* m_pLayout;
	QCheckBox*				m_pSelectAll;
	QTableWidget*			m_pTableWidget;

	QHBoxLayout*			m_pBtnLayout;
	QSpacerItem*			m_pBtnLeftSpacer;
	QWidget*				m_pButtonWidget;
	QDialogButtonBox *		m_pButtonBox;
	//CRoom3DImageButton*		m_pOKBtn;
	//CRoom3DImageButton*		m_pCancelBtn;

	CRoom3DItemDownloadMgr* m_pDownloadMgr;

	CRoom3DMoreItemDelegate* m_pItemDelegate;

};
