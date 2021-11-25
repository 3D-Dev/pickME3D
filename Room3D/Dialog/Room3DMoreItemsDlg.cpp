#include "Room3DMoreItemsDlg.h"
#include "Common.h"
#include <QDomDocument>
#include "Room3DApplication.h"

#define CHECKBOX_WIDTH	20

CRoom3DMoreItemsDlg::CRoom3DMoreItemsDlg(QWidget* a_pParent) : CRoom3DBaseDlg(a_pParent)
{
	InitControls();

	m_pItemDelegate = new CRoom3DMoreItemDelegate();
	m_pTableWidget->setAcceptDrops(false);
	m_pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	QAbstractItemDelegate* pOldDelegate = m_pTableWidget->itemDelegate();
	delete pOldDelegate;
	m_pTableWidget->setItemDelegate(m_pItemDelegate);
	this->setWindowTitle("Get items");

	m_pDownloadMgr = g_pApp->GetDocument()->GetItemMgr()->GetItemDownloadMgr();
	connect(m_pDownloadMgr , SIGNAL(UpdateItemInfoUI(bool)), this, SLOT(OnUpdateItemTableWidget(bool)));
	connect(m_pDownloadMgr, SIGNAL(UpdateItemInfoUIImage(int, QPixmap&, bool)), this, SLOT(OnUpdateItemInfoImage(int, QPixmap&, bool)));
	m_pDownloadMgr->RequestDownloadItemList();
}

CRoom3DMoreItemsDlg::~CRoom3DMoreItemsDlg(void)
{
	SAFE_DELETE(m_pItemDelegate);
}

void CRoom3DMoreItemsDlg::InitControls()
{
	this->setFixedSize(QSize(600,480));
	m_pLayout = new QVBoxLayout(this);

	m_pTableWidget = new QTableWidget(this);
	m_pLayout->addWidget(m_pTableWidget);

	m_pSelectAll = new QCheckBox();
	m_pSelectAll->setText("Select All");
	m_pLayout->addWidget(m_pSelectAll);


	m_pButtonWidget = new QWidget(this);
	m_pButtonWidget->setMinimumHeight(30);
	m_pButtonWidget->setMaximumHeight(30);
	m_pLayout->addWidget(m_pButtonWidget);

	m_pBtnLayout = new QHBoxLayout(m_pButtonWidget);
	m_pBtnLayout->setContentsMargins(0,0,0,0);
	
	m_pBtnLeftSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_pBtnLayout->addItem(m_pBtnLeftSpacer);

	//m_pOKBtn = new CRoom3DImageButton(BUTTON_IMG_NEW, 0, Qt::PointingHandCursor, m_pButtonWidget);
	//m_pOKBtn->setMaximumWidth(50);
	//m_pOKBtn->setMinimumWidth(50);
	//m_pBtnLayout->addWidget(m_pOKBtn);

	//m_pCancelBtn = new CRoom3DImageButton(BUTTON_IMG_CLOSE, 1, Qt::PointingHandCursor, m_pButtonWidget);
	//m_pCancelBtn->setMaximumWidth(50);
	//m_pCancelBtn->setMinimumWidth(50);

	//m_pBtnLayout->addWidget(m_pCancelBtn);
	m_pButtonBox = new QDialogButtonBox(this);
	m_pButtonBox->setObjectName(QString::fromUtf8("m_pButtonBox"));
	m_pButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	m_pBtnLayout->addWidget(m_pButtonBox);


	//connect(m_pOKBtn, SIGNAL(pressed(int)), this, SLOT(OnPressed(int)));
	//connect(m_pCancelBtn, SIGNAL(pressed(int)), this, SLOT(OnPressed(int)));
	connect(m_pButtonBox, SIGNAL(accepted()), this, SLOT(verify()));
	connect(m_pButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(m_pSelectAll, SIGNAL(stateChanged(int)), this, SLOT(OnSelectAll(int)));

	QStringList strList;
	strList.push_back(ITEM_MORE_DLG_COLUMN_NAME);
	strList.push_back(ITEM_MORE_DLG_COLUMN_CATEGORY);
	strList.push_back(ITEM_MORE_DLG_COLUMN_PRICE);
	strList.push_back(ITEM_MORE_DLG_COLUMN_WIDTH);
	strList.push_back(ITEM_MORE_DLG_COLUMN_LENGTH);
	strList.push_back(ITEM_MORE_DLG_COLUMN_HEIGHT);
	strList.push_back(ITEM_MORE_DLG_COLUMN_NAME);

	m_pTableWidget->setColumnCount(7);
	m_pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTableWidget->horizontalHeader()->setStretchLastSection(true);
	m_pTableWidget->verticalHeader()->setVisible(false);
	m_pTableWidget->setCornerButtonEnabled(false);
	m_pTableWidget->setHorizontalHeaderLabels(strList);
	m_pTableWidget->verticalHeader()->setDefaultSectionSize(20);
	m_pTableWidget->setColumnWidth(0, 200);
	m_pTableWidget->setColumnWidth(1, 60);
	m_pTableWidget->setColumnWidth(2, 70);
	m_pTableWidget->setColumnWidth(3, 60);
	m_pTableWidget->setColumnWidth(4, 60);
	m_pTableWidget->setColumnWidth(5, 60);
	m_pTableWidget->setColumnWidth(6, 50);
}

void CRoom3DMoreItemsDlg::OnUpdateItemTableWidget(bool error)
{
	QVector<ITEM_INFO_EX*> pItems = m_pDownloadMgr->GetItemInfoList();
	int nItemCount = pItems.count();
	m_pTableWidget->setRowCount(nItemCount);
	QTableWidgetItem* pItem = NULL;
	for(int i = 0; i < nItemCount; i++)
	{
		ITEM_INFO_EX* pItemInfo = pItems.at(i);
		// product name
		pItem = new QTableWidgetItem(pItemInfo->m_sName);
		m_pTableWidget->setItem(i, 0, pItem);
		QVariant userData((long)pItemInfo);
		pItem->setData(Qt::UserRole, userData);
		CRoom3DItemCheckBox* pCheckBox = new CRoom3DItemCheckBox(m_pTableWidget);
		pCheckBox->setText(pItemInfo->m_sName);
		m_pTableWidget->setCellWidget(i, 0, pCheckBox);
		// product category
		pItem = new QTableWidgetItem(QString().setNum(pItemInfo->m_nCategoryCode1));
		m_pTableWidget->setItem(i, 1, pItem);
		// product price
		pItem = new QTableWidgetItem(QString().setNum(pItemInfo->m_fPrice, 'f', 2));
		m_pTableWidget->setItem(i, 2, pItem);
		// product width
		pItem = new QTableWidgetItem(QString().setNum(pItemInfo->m_fWidth, 'f', 2));
		m_pTableWidget->setItem(i, 3, pItem);
		// product length
		pItem = new QTableWidgetItem(QString().setNum(pItemInfo->m_fLength, 'f', 2));
		m_pTableWidget->setItem(i, 4, pItem);
		// product height
		pItem = new QTableWidgetItem(QString().setNum(pItemInfo->m_fHeight, 'f', 2));
		m_pTableWidget->setItem(i, 5, pItem);
	}
}

void CRoom3DMoreItemsDlg::OnUpdateItemInfoImage(int a_nRow, QPixmap& a_pixmap, bool error)
{
	QTableWidgetItem* pItem = new QTableWidgetItem();
	pItem->setIcon(QIcon(a_pixmap));
	m_pTableWidget->setItem(a_nRow, 6, pItem);
}

void CRoom3DMoreItemsDlg::verify()
{
	QVector<ITEM_INFO_EX*> pItems;
	GetSelectedItemInfoList(pItems);
	m_pDownloadMgr->RequestDownloadItemDatas(pItems);
	accept();
}

void CRoom3DMoreItemsDlg::OnSelectAll(int a_nState)
{
	if(a_nState == Qt::PartiallyChecked)
	{
		return;
	}
	Qt::CheckState nCheckState;
	if(a_nState == Qt::Checked)
	{
		nCheckState = Qt::Checked;
	}
	else
	{
		nCheckState = Qt::Unchecked;
	}
	int nCount = m_pTableWidget->rowCount();
	for(int i = 0; i < nCount; i++)
	{
		QCheckBox* pCheckBox = (QCheckBox*)m_pTableWidget->cellWidget(i,0);
		if(NULL == pCheckBox)
			continue;
		pCheckBox->setCheckState(nCheckState);
	}
}

void CRoom3DMoreItemsDlg::OnPressed(int a_nBtnID)
{
}

void CRoom3DMoreItemsDlg::GetSelectedItemInfoList(QVector<ITEM_INFO_EX*>& a_pSelItems)
{
	int nCount = m_pTableWidget->rowCount();
	for(int i = 0; i < nCount; i++)
	{
		QCheckBox* pCheckBox = (QCheckBox*)m_pTableWidget->cellWidget(i,0);
		if(NULL == pCheckBox)
			continue;
		if(pCheckBox->checkState() == Qt::Checked)
		{
			ITEM_INFO_EX* pInfo = (ITEM_INFO_EX*) m_pTableWidget->item(i,0)->data(Qt::UserRole).toInt();
			a_pSelItems.push_back(pInfo);
		}
	}
}