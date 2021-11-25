#include "Room3DStep3ToolBox.h"
#include "Define.h"
#include "Common.h"
#include "Room3DApplication.h"
#include "Room3DItemListView.h"
#include "Room3DUtility.h"
#include "Room3DMoreItemsDlg.h"
#include "Room3DWebDlg.h"

CRoom3DStep3ToolBox::CRoom3DStep3ToolBox(QWidget* parent) : CRoom3DBaseToolBox(parent)
{
	m_pItemMgr = g_pApp->GetDocument()->GetItemMgr();
	InitControls();
	UpdateCategoryTabs();
}

CRoom3DStep3ToolBox::~CRoom3DStep3ToolBox(void)
{
//	ClearAllTabls();
	SAFE_DELETE(m_pVerticalLayout);
}

void CRoom3DStep3ToolBox::InitControls()
{
	InitBkWidget();

	m_pVerticalLayout = new QVBoxLayout(m_pBkWidget);
	m_pVerticalLayout->setSpacing(0);
	m_pVerticalLayout->setContentsMargins(3, TOOLBOX_CONTENTS_MARGIN, 3, TOOLBOX_CONTENTS_MARGIN);

	m_pVLayout1 = new QVBoxLayout();
	m_pVLayout1->setSpacing(0);
	m_pVLayout1->setContentsMargins(7, 0, 7, 0);
	// Add more button.
	m_pMoreWidget = new QWidget(this);
	m_pMoreLayout = new QHBoxLayout(m_pMoreWidget);
	m_pMoreLayout->setSpacing(6);
	m_pMoreLayout->setContentsMargins(0, 0, 0, TOOLBOX_CONTENTS_MARGIN);
	m_pMoreBtn = new CRoom3DImageButton(BUTTON_IMG_MORE_ITEM, BTN_MORE_ITEM , Qt::PointingHandCursor, this);
	m_pMoreLayout->addWidget(m_pMoreBtn);

	// Add TabWidget.
	m_pItemTabWidget = new CTabViewContainer(this);
	m_pItemTabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	m_pVLayout1->addWidget(m_pMoreWidget);
	m_pVLayout1->addWidget(m_pItemTabWidget);
	m_pVerticalLayout->addLayout(m_pVLayout1);

	// Add Sparator
	m_pSeparator = new QFrame(m_pBkWidget);
	m_pSeparator->setFixedHeight(16);
	m_pSeparator->setFrameShape(QFrame::HLine);
	m_pSeparator->setFrameShadow(QFrame::Sunken);

	QPalette curPalette = m_pSeparator->palette();
	curPalette.setColor(QPalette::Dark, QColor(22,20,16));
	curPalette.setColor(QPalette::Light, QColor(22,20,16));
	m_pSeparator->setPalette(curPalette);
	m_pVerticalLayout->addWidget(m_pSeparator);

	m_pVLayout2 = new QVBoxLayout();
	m_pVLayout2->setSpacing(0);
	m_pVLayout2->setContentsMargins(7, 0, 7, 0);
	// Add description
	m_pDescTitleLbl = new QLabel(this);
	m_pDescTitleLbl->setText(STR_DESCRIPTION);
	curPalette = m_pSeparator->palette();
	curPalette.setColor(QPalette::WindowText, QColor(30,123,157));
	m_pDescTitleLbl->setPalette(curPalette);
	m_pDescTitleLbl->setMaximumHeight(20);

	m_pDescStackWidget = new QStackedWidget(this);
	m_pDescStackWidget->setFixedHeight(150);
	m_pDescItemPage = new QWidget();
	m_pDescItemLayout = new QVBoxLayout(m_pDescItemPage);
	m_pDescItemLayout->setSpacing(0);
	m_pDescItemLayout->setContentsMargins(15, 0, 7, 0);
	// overview
	m_pDescOverviewLayout = new QHBoxLayout();
	m_pDescImgLbl = new QLabel(m_pDescItemPage);
	m_pDescImgLbl->setFixedWidth(60);
	m_pOverViewEdit = new QTextEdit(m_pDescItemPage);
	m_pOverViewEdit->setReadOnly(true);
	//m_pOverViewEdit->setWordWrapMode(QTextOption::WrapAnywhere);
	//m_pOverViewEdit->setTextInteractionFlags(Qt::NoTextInteraction);
	m_pOverViewEdit->setFixedHeight(60);
	SetDefaultLineEditPalette(m_pOverViewEdit);
	m_pDescOverviewLayout->addWidget(m_pDescImgLbl);
	m_pDescOverviewLayout->addWidget(m_pOverViewEdit);

	// price
	m_pDescPriceLayout = new QHBoxLayout();
	m_pDescPriceLayout->setSpacing(4);
	m_pPriceLbl_1 = new QLabel(m_pDescItemPage);
	m_pPriceLbl_1->setFixedWidth(100);
	SetDefaultPalette(m_pPriceLbl_1);
	m_pPriceEidt_1 = new QLineEdit(m_pDescItemPage);
	m_pPriceEidt_1->setFixedWidth(80);
	m_pPriceEidt_1->setReadOnly(true);
	SetDefaultLineEditPalette(m_pPriceEidt_1);
	m_pPriceEidt_2 = new QLineEdit(m_pDescItemPage);
	m_pPriceEidt_2->setReadOnly(true);
	SetDefaultLineEditPalette(m_pPriceEidt_2);
	m_pDescPriceLayout->addWidget(m_pPriceLbl_1);
	m_pDescPriceLayout->addWidget(m_pPriceEidt_1);
	m_pDescPriceLayout->addWidget(m_pPriceEidt_2);

	// price note
	//m_pDescPriceNoteLayout = new QHBoxLayout();
	//m_pPriceNoteLbl_1 = new QLabel(m_pDescItemPage);
	//m_pPriceNoteLbl_1->setFixedWidth(100);
	//SetDefaultPalette(m_pPriceNoteLbl_1);
	//m_pPriceNoteLbl_2 = new QLabel(m_pDescItemPage);
	//SetDefaultPalette(m_pPriceNoteLbl_2);
	//m_pDescPriceNoteLayout->addWidget(m_pPriceNoteLbl_1);
	//m_pDescPriceNoteLayout->addWidget(m_pPriceNoteLbl_2);

	// Update on
	m_pDescUpdateLayout = new QHBoxLayout();
	m_pDescUpdateLayout->setSpacing(4);
	m_pUpdateLbl_1 = new QLabel(m_pDescItemPage);
	m_pUpdateLbl_1->setFixedWidth(100);
	SetDefaultPalette(m_pUpdateLbl_1);
	m_pUpdateLbl_2 = new QLineEdit(m_pDescItemPage);
	m_pUpdateLbl_2->setReadOnly(true);
	SetDefaultLineEditPalette(m_pUpdateLbl_2);
	m_pDescUpdateLayout->addWidget(m_pUpdateLbl_1);
	m_pDescUpdateLayout->addWidget(m_pUpdateLbl_2);


	// Size
	m_pDescSizeLayout = new QHBoxLayout();
	m_pDescSizeLayout->setSpacing(4);
	m_pSizeLbl_1 = new QLabel(m_pDescItemPage);
	m_pSizeLbl_1->setFixedWidth(100);
	SetDefaultPalette(m_pSizeLbl_1);

	m_pSizeEdit_1 = new QLineEdit(m_pDescItemPage);
	m_pSizeEdit_1->setFixedWidth(40);
	m_pSizeEdit_1->setReadOnly(true);
	SetDefaultLineEditPalette(m_pSizeEdit_1);
	m_pSizeLbl_2 = new QLabel(m_pDescItemPage);
	SetDefaultPalette(m_pSizeLbl_2);
	m_pSizeLbl_2->setText(tr("cm"));
	m_pSizeLbl_3 = new QLabel(m_pDescItemPage);
	curPalette = m_pSizeLbl_3->palette();
	curPalette.setColor(QPalette::WindowText, QColor(30,123,157));
	m_pSizeLbl_3->setPalette(curPalette);
	m_pSizeLbl_3->setText(tr("*"));
	m_pSizeEdit_2 = new QLineEdit(m_pDescItemPage);
	m_pSizeEdit_2->setFixedWidth(40);
	m_pSizeEdit_2->setReadOnly(true);
	SetDefaultLineEditPalette(m_pSizeEdit_2);
	m_pSizeLbl_4 = new QLabel(m_pDescItemPage);
	SetDefaultPalette(m_pSizeLbl_4);
	m_pSizeLbl_4->setText(tr("cm"));
	m_pSizeLbl_5 = new QLabel(m_pDescItemPage);
	curPalette = m_pSizeLbl_5->palette();
	curPalette.setColor(QPalette::WindowText, QColor(30,123,157));
	m_pSizeLbl_5->setPalette(curPalette);
	m_pSizeLbl_5->setText(tr("*"));
	m_pSizeEdit_3 = new QLineEdit(m_pDescItemPage);
	m_pSizeEdit_3->setFixedWidth(40);
	m_pSizeEdit_3->setReadOnly(true);
	SetDefaultLineEditPalette(m_pSizeEdit_3);
	m_pSizeLbl_6 = new QLabel(m_pDescItemPage);
	SetDefaultPalette(m_pSizeLbl_6);
	m_pSizeLbl_6->setText(tr("cm"));
	m_pDescSizeLayout->addWidget(m_pSizeLbl_1);
	m_pDescSizeLayout->addWidget(m_pSizeEdit_1);
	m_pDescSizeLayout->addWidget(m_pSizeLbl_2);
	m_pDescSizeLayout->addWidget(m_pSizeLbl_3);
	m_pDescSizeLayout->addWidget(m_pSizeEdit_2);
	m_pDescSizeLayout->addWidget(m_pSizeLbl_4);
	m_pDescSizeLayout->addWidget(m_pSizeLbl_5);
	m_pDescSizeLayout->addWidget(m_pSizeEdit_3);
	m_pDescSizeLayout->addWidget(m_pSizeLbl_6);

	m_pDescItemLayout->addLayout(m_pDescOverviewLayout);
	m_pDescItemLayout->addLayout(m_pDescSizeLayout);
	m_pDescItemLayout->addLayout(m_pDescPriceLayout);
//	m_pDescItemLayout->addLayout(m_pDescPriceNoteLayout);
	m_pDescItemLayout->addLayout(m_pDescUpdateLayout);

	m_pDescStackWidget->addWidget(m_pDescItemPage);

	// No item page
	m_pDescNoItemPage = new QWidget();
	m_pdescNoItemLayout = new QHBoxLayout(m_pDescNoItemPage);
	m_pdescNoItemLayout->setSpacing(0);
	m_pdescNoItemLayout->setContentsMargins(11, 0, 11, 0);
	m_pDescNoItemLbl = new QLabel(m_pDescNoItemPage);
	m_pDescNoItemLbl->setAlignment(Qt::AlignCenter);
	m_pDescNoItemLbl->setText(TOOLBOX3_NO_SELECTED_ITEM);
	SetDefaultPalette(m_pDescNoItemLbl);
	m_pdescNoItemLayout->addWidget(m_pDescNoItemLbl);
	m_pDescStackWidget->addWidget(m_pDescNoItemPage);

	// Add search box.
	m_pSearchTitleLbl = new QLabel(this);
	m_pSearchTitleLbl->setText(STR_SEARCH);
	m_pSearchTitleLbl->setMaximumHeight(20);
	curPalette = m_pSeparator->palette();
	curPalette.setColor(QPalette::WindowText, QColor(30,123,157));
	m_pSearchTitleLbl->setPalette(curPalette);


	m_pSearchWidget = new QWidget(this);
	m_pSearchWidget->setFixedHeight(30);
	m_pSearchGridPlayout = new QGridLayout(m_pSearchWidget);
	m_pSearchGridPlayout->setSpacing(6);
	m_pSearchGridPlayout->setContentsMargins(11, 0, 11, 0);
	m_pSearchPriceLbl = new QLabel(m_pSearchWidget);
	SetDefaultPalette(m_pSearchPriceLbl);
	m_pSearchPriceLbl->setFixedWidth(100);

	m_pSarchPriceUpDownCombo = new QComboBox(m_pSearchWidget);


	m_pSearchBtn = new CRoom3DImageButton(BUTTON_IMG_FIND, BTN_SEARCH_ITEM, Qt::PointingHandCursor, m_pSearchWidget);
	m_pSearchBtn->setMaximumWidth(50);

	m_pSearchGridPlayout->addWidget(m_pSearchPriceLbl, 1, 1, 1, 1);
	m_pSearchGridPlayout->addWidget(m_pSarchPriceUpDownCombo, 1, 2, 1, 1);
	m_pSearchGridPlayout->addWidget(m_pSearchBtn, 1, 3, 1, 1);

	m_pVLayout2->addWidget(m_pDescTitleLbl);
	m_pVLayout2->addWidget(m_pDescStackWidget);
	m_pVLayout2->addWidget(m_pSearchTitleLbl);
	m_pVLayout2->addWidget(m_pSearchWidget);
	m_pVerticalLayout->addLayout(m_pVLayout2);

	connect(m_pItemTabWidget, SIGNAL(currentChanged(int)), this, SLOT(OnCurrentTabChanged(int)));
	connect(m_pSearchBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pMoreBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));

	m_pSearchBtn->setText(STR_SEARCH);
	m_pSarchPriceUpDownCombo->addItem(tr("All"));
	m_pSarchPriceUpDownCombo->addItem(tr("0 to 500 baht"));
	m_pSarchPriceUpDownCombo->addItem(tr("501 to 3,000 baht"));
	m_pSarchPriceUpDownCombo->addItem(tr("3,001 to 10,000 baht"));
	m_pSarchPriceUpDownCombo->addItem(tr("10,001 to 50,000 baht"));
	m_pSarchPriceUpDownCombo->addItem(tr("More than 50,000 baht"));
	m_pSarchPriceUpDownCombo->setCurrentIndex(0);

	m_pPriceLbl_1->setText(STR_APPROX_PRICE);
	// m_pPriceNoteLbl_1->setText(STR__PRICE_NOTE);
	m_pUpdateLbl_1->setText(STR_PRICE_UPDATE_ON);
	m_pSizeLbl_1->setText(STR_DIMENTION);

	m_pSearchPriceLbl->setText(STR_PRICE_RANGE);

	m_pDescImgLbl->hide();
}

void CRoom3DStep3ToolBox::ClearAllTabls()
{
	for(int i = 0; i < m_pItemTabWidget->count(); i++)
	{
		QWidget* pPage= m_pItemTabWidget->widget(i);
		SAFE_DELETE(pPage);
	}
	m_pItemTabWidget->clear();
}

void CRoom3DStep3ToolBox::UpdateCategoryTabs()
{
	QVector<ITEM_CATEGORY_INFO*> catInfors = m_pItemMgr->GetItemCategoryInfos();
#if 0
	QVector<ITEM_CATEGORY_INFO*>::iterator it = catInfors.begin();
	while(it != catInfors.end())
	{
		UpdateCategoryTab(*it);
		it++;
	}
#else
	if(catInfors.count() > 6)
	{
		bool blAdded = false;
		if(catInfors.at(0)->m_eActionType == ACT_ADD)
		{
			blAdded = true;
		}
		UpdateCategoryTab(catInfors.at(2));
		UpdateCategoryTab(catInfors.at(1));
		UpdateCategoryTab(catInfors.at(0));
		UpdateCategoryTab(catInfors.at(6));
		UpdateCategoryTab(catInfors.at(5));
		UpdateCategoryTab(catInfors.at(4));
		UpdateCategoryTab(catInfors.at(3));
		if(blAdded)
		{
			m_pItemTabWidget->setCurrentWidget(m_pItemTabWidget->widget(6));
		}
	}
#endif
}

void CRoom3DStep3ToolBox::UpdateCategoryTab(ITEM_CATEGORY_INFO* a_pCategoryInfo)
{
	CRoom3DItemListView* pItemListView = NULL;
	if(a_pCategoryInfo->m_eActionType == ACT_ADD)
	{
		pItemListView= new CRoom3DItemListView();
		connect(pItemListView, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(OnCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
		connect(pItemListView, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnItemClicked(QListWidgetItem*)));
		connect(pItemListView, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(OnItemDoubleClicked(QListWidgetItem*)));
		// Add list view to tab
		m_pItemTabWidget->addTab(pItemListView, a_pCategoryInfo->m_sCategoryName);
		// Add search info
		ITEM_SEARCH_INFO* pItemSearchInfo = pItemListView->GetSearchInfo();
		memset(pItemSearchInfo, 0 , sizeof(ITEM_SEARCH_INFO));
	}
	else 
		pItemListView = FindListView(a_pCategoryInfo->m_nCategoryCode);
	if(pItemListView)
	{
		pItemListView->SetCategoryInfo(a_pCategoryInfo);
	}
	a_pCategoryInfo->m_eActionType = ACT_NONE;
}

CRoom3DItemListView* CRoom3DStep3ToolBox::FindListView(int a_nCategoryCode)
{
	for(int i = 0; i < m_pItemTabWidget->count(); i++)
	{
		CRoom3DItemListView* pListView = (CRoom3DItemListView*) m_pItemTabWidget->widget(i);
		if(pListView->GetCategoryCode() == a_nCategoryCode)
			return pListView;
	}
	return NULL;
}

// this function is called when item's selection has been changed.
void CRoom3DStep3ToolBox::OnCurrentItemChanged( QListWidgetItem * a_pCurrent, QListWidgetItem * a_pPrevious)
{
	if(a_pCurrent)
	{
		ITEM_INFO* pItemInfo = (ITEM_INFO*) a_pCurrent->data(Qt::UserRole).toInt();
		UpdateItemControls(pItemInfo);
	}
}

void CRoom3DStep3ToolBox::OnItemClicked( QListWidgetItem * a_pCurrent)
{
	ITEM_INFO* pItemInfo = (ITEM_INFO*) a_pCurrent->data(Qt::UserRole).toInt();
	UpdateItemControls(pItemInfo);
}

void CRoom3DStep3ToolBox::OnItemDoubleClicked ( QListWidgetItem * item )
{
	ITEM_INFO* pItemInfo = (ITEM_INFO*) item->data(Qt::UserRole).toInt();
}

void CRoom3DStep3ToolBox::OnCurrentTabChanged(int a_nIndex)
{
	CRoom3DItemListView* pItemListView = (CRoom3DItemListView*) m_pItemTabWidget->widget(a_nIndex);
	if(pItemListView)
	{
		ITEM_SEARCH_INFO* pSearchInfo = pItemListView->GetSearchInfo();
		QList<QListWidgetItem*> items = pItemListView->selectedItems();
		ITEM_INFO* pItemInfo = NULL;
		if(items.size() > 0)
		{
			pItemInfo = (ITEM_INFO*) items[0]->data(Qt::UserRole).toInt();
		}
		//UpdateSearchData(pSearchInfo, false);//-temp
		UpdateItemControls(pItemInfo);
	}
}

void CRoom3DStep3ToolBox::UpdateItemControls(ITEM_INFO* a_pItemInfo)
{
	if(a_pItemInfo == NULL)
	{
		m_pDescStackWidget->setCurrentIndex(1);
	}
	else
	{
		m_pDescStackWidget->setCurrentIndex(0);
		// update overview
		m_pOverViewEdit->setText(a_pItemInfo->m_sDesc);
		// update price
		QString sPrice;
		sPrice.setNum(a_pItemInfo->m_fPrice, 'f', 2);
		m_pPriceEidt_1->setText(sPrice);
		QString sPriceNote = a_pItemInfo->m_sPriceNote;
		if(sPriceNote.isEmpty())
			sPriceNote = "-";
		// m_pPriceNoteLbl_2->setText(sPriceNote);
		m_pPriceEidt_2->setText(sPriceNote);
		m_pPriceEidt_2->setCursorPosition(0);

		// update on
		QString sUpdateDate = a_pItemInfo->m_sUpdateDate;
		if(sUpdateDate.isEmpty())
			sUpdateDate = STR_NO_UPDATE_ON;
		m_pUpdateLbl_2->setText(sUpdateDate);
		// update size
		QString sSize(STR_NO_SIZE);
		m_pSizeEdit_1->setText(QString().setNum(a_pItemInfo->m_fWidth));
		m_pSizeEdit_2->setText(QString().setNum(a_pItemInfo->m_fLength));
		m_pSizeEdit_3->setText(QString().setNum(a_pItemInfo->m_fHeight));
		// Set item image
		QString strPath = a_pItemInfo->m_sImagePath;
		strPath = g_pApp->GetResourcePath(strPath);
		// if 2d image doesn/t exist, then default image
		if(!QFile::exists(strPath))
		{
			strPath = DEFAULT_ITEM_IMG;
		}
		QRect targetRect = QRect(QPoint(0,0), m_pDescImgLbl->size());
		targetRect.adjust(5,5,0,0);
		QPixmap pixmap(strPath);
		pixmap = pixmap.scaled(targetRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		m_pDescImgLbl->setPixmap(pixmap);
	}
}

void CRoom3DStep3ToolBox::pressedButton(int a_nBtnID)
{
	switch(a_nBtnID)
	{
	case BTN_MORE_ITEM:
		{
			CRoom3DWebDlg webDlg(g_pApp->GetMainWidget());
			int nResult = webDlg.exec();
			if(nResult == QDialog::Accepted)
			{
			}
			break;
		}
	case BTN_SEARCH_ITEM:
		{
			CRoom3DItemListView* pItemListView = (CRoom3DItemListView*)m_pItemTabWidget->currentWidget();
			if(pItemListView)
			{
				ITEM_SEARCH_INFO* pSearchInfo = pItemListView->GetSearchInfo();
				UpdateSearchData(pSearchInfo, true);
				SearchItem(pSearchInfo);
			}

		}
		break;
	}
}

void CRoom3DStep3ToolBox::UpdateSearchData(ITEM_SEARCH_INFO* a_pSearchInfo, bool a_bUpdateData)
{
	if(a_bUpdateData)
	{
		CRoom3DItemListView* pItemListView = (CRoom3DItemListView*)m_pItemTabWidget->currentWidget();
		if(pItemListView)
		{
			a_pSearchInfo->m_nCategoryCode = pItemListView->GetCategoryCode();
			a_pSearchInfo->m_nPriceComboIndex = m_pSarchPriceUpDownCombo->currentIndex();
			UpdateSearchInfoFromComboIndex(a_pSearchInfo->m_nPriceComboIndex, a_pSearchInfo);
		}
	}
	else
	{
		m_pSarchPriceUpDownCombo->setCurrentIndex(a_pSearchInfo->m_nPriceComboIndex);
	}
}

bool CRoom3DStep3ToolBox::SearchItem(ITEM_SEARCH_INFO* a_pItemSearchInfo)
{
	bool blResult = m_pItemMgr->Search(a_pItemSearchInfo);
	if(blResult)
	{
		QVector<ITEM_CATEGORY_INFO*> catInfors = m_pItemMgr->GetItemCategoryInfos();
		QVector<ITEM_CATEGORY_INFO*>::iterator it = catInfors.begin();
		while(it != catInfors.end())
		{
			if((*it)->m_nCategoryCode == a_pItemSearchInfo->m_nCategoryCode)
			{
				UpdateCategoryTab(*it);
				return true;
			}
			it++;
		}
	}
	return false;
}


void CRoom3DStep3ToolBox::UpdateControlState(void* a_pUserData)
{
	UpdateCategoryTabs();
	if(a_pUserData)
	{
		ITEM_INFO_FOR_UI* pInfoUI = (ITEM_INFO_FOR_UI*)a_pUserData;
		ITEM_INFO* pInfo = NULL;
		CRoom3DItemListView* pItemListView = (CRoom3DItemListView*)m_pItemTabWidget->currentWidget();
		// if current widget doesn't contain the item with pInfo->m_sItemID, then change current widget.
		int nCurItemIndex = pItemListView->FindItemWidget(pInfoUI->m_sItemID);
		if( nCurItemIndex < 0 )
		{
			pItemListView = FindListView(pInfoUI->m_nCategoryCode);
			if(pItemListView)
			{
				m_pItemTabWidget->setCurrentWidget(pItemListView);
				pItemListView->SelectItem(pInfoUI->m_sItemID);
			}
			pInfo = pInfoUI->m_pItemInfo;
		}
		else
		{
			pInfo = (ITEM_INFO*) pItemListView->item(nCurItemIndex)->data(Qt::UserRole).toInt();
		}
		UpdateItemControls(pInfo);
	}
}

void CRoom3DStep3ToolBox::showEvent(QShowEvent * e)
{
}