//----------------------------------------------//
//	File: Room3DStep3ToolBox.h					//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//

#pragma once
#include "Define.h"
#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include "Room3DImageButton.h"
#include <QVBoxLayout>
#include <QTextEdit>
#include "Room3DItemMgr.h"
#include "Room3DStep1ToolBox.h"
#include <QLineEdit>
#include "Room3DTabWidget.h"

class CRoom3DItemListView;

class CRoom3DStep3ToolBox : public CRoom3DBaseToolBox
{
	Q_OBJECT
	enum BUTTON_ID
	{
		BTN_MORE_ITEM = 0,
		BTN_SEARCH_ITEM,
		BTN_TUTORIAL,
	};

public:
	CRoom3DStep3ToolBox(QWidget* parent = 0);
	~CRoom3DStep3ToolBox(void);
	void UpdateControlState(void* a_pUserData = NULL);
protected:
	void InitControls();
	void UpdateLayout();
	virtual void showEvent(QShowEvent * e);
private:
	void ClearAllTabls();
	void UpdateCategoryTabs();
	bool SearchItem(ITEM_SEARCH_INFO* a_pItemSearchInfo);
	void UpdateCategoryTab(ITEM_CATEGORY_INFO* a_pCategoryInfo);
	CRoom3DItemListView* FindListView(int a_nCategoryCode);


protected slots:
	void OnCurrentItemChanged( QListWidgetItem * a_pCurrent, QListWidgetItem * a_pPrevious);
	void OnItemClicked( QListWidgetItem * a_pCurrent);
	void OnItemDoubleClicked ( QListWidgetItem * item );
	void OnCurrentTabChanged(int a_nIndex);
	void pressedButton(int a_nBtnID);
private:
	void UpdateItemControls(ITEM_INFO* a_pItemInfo);
	void UpdateSearchData(ITEM_SEARCH_INFO* a_pSearchInfo, bool a_bUpdateData);
protected:
	QVBoxLayout* m_pVerticalLayout;
	QVBoxLayout* m_pVLayout1;
	QVBoxLayout* m_pVLayout2;
	
	QWidget*		m_pMoreWidget;
	QHBoxLayout*	m_pMoreLayout;
	CRoom3DImageButton* m_pMoreBtn;
	
	CTabViewContainer* m_pItemTabWidget;
	QFrame*			m_pSeparator;

	QStackedWidget* m_pStackedDescWidget;
	QLabel *m_pDescTitleLbl;
	QStackedWidget *m_pDescStackWidget;
	QWidget *m_pDescItemPage;
	QVBoxLayout *m_pDescItemLayout;

	QLabel *m_pDescImgLbl;

	// Description overview
	QHBoxLayout *m_pDescOverviewLayout;
	QTextEdit*	m_pOverViewEdit;

	// Price
	QHBoxLayout *m_pDescPriceLayout;
	QLabel *m_pPriceLbl_1;
	QLineEdit *m_pPriceEidt_1;
	QLineEdit *m_pPriceEidt_2;
	// Price Note
	QHBoxLayout *m_pDescPriceNoteLayout;
	QLabel* m_pPriceNoteLbl_1;
	QLabel* m_pPriceNoteLbl_2;

	// Update on
	QHBoxLayout *m_pDescUpdateLayout;
	QLabel *m_pUpdateLbl_1;
	QLineEdit *m_pUpdateLbl_2;

	// Size
	QHBoxLayout *m_pDescSizeLayout;
	QLabel *m_pSizeLbl_1;
	QLineEdit* m_pSizeEdit_1;
	QLabel *m_pSizeLbl_2;
	QLabel *m_pSizeLbl_3;
	QLineEdit* m_pSizeEdit_2;
	QLabel *m_pSizeLbl_4;
	QLabel *m_pSizeLbl_5;
	QLineEdit* m_pSizeEdit_3;
	QLabel *m_pSizeLbl_6;


	QWidget *m_pDescNoItemPage;
	QHBoxLayout *m_pdescNoItemLayout;
	QLabel *m_pDescNoItemLbl;

	QLabel	*m_pSearchTitleLbl;
	QWidget *m_pSearchWidget;
	QGridLayout		*m_pSearchGridPlayout;
	QLabel			*m_pSearchPriceLbl;
	QComboBox		*m_pSarchPriceUpDownCombo;
	CRoom3DImageButton	*m_pSearchBtn;

	CRoom3DItemMgr		*m_pItemMgr;
};
