#include "Room3DItemListView.h"
#include "Room3DItemMgr.h"
#include "Room3DApplication.h"
#include <QListWidgetItem>
#include <Room3DUtility.h>

#define ITEM_WIDTH		60
#define ITEM_HEIGHT		60
#define ITEM_GRID_WIDTH		ITEM_WIDTH + 10
#define ITEM_GRID_HEIGHT	ITEM_HEIGHT + 10

CRoom3DItemListView::CRoom3DItemListView(QWidget * parent) : QListWidget(parent)
{

	setDragEnabled(false);
	setDragDropMode(NoDragDrop);
	setViewMode(QListView::IconMode);
	setIconSize(QSize(ITEM_WIDTH, ITEM_HEIGHT));
	setGridSize(QSize(ITEM_GRID_WIDTH, ITEM_GRID_HEIGHT));
	setMovement(QListView::Snap);
	setAcceptDrops(false);
	setDropIndicatorShown(true);

	QAbstractItemDelegate * pOldItemDelegate = itemDelegate();
	SAFE_DELETE(pOldItemDelegate);
	m_pItemDelegate = new CRoom3DItemDelegate(this);
	setItemDelegate(m_pItemDelegate);

	m_pSearchInfo = new ITEM_SEARCH_INFO();
	memset(m_pSearchInfo, 0 , sizeof(ITEM_SEARCH_INFO));
	SetDefaultLineEditPalette(this);
	setResizeMode(QListView::Adjust);
}

CRoom3DItemListView::~CRoom3DItemListView(void)
{
	delete m_pItemDelegate;
}

void CRoom3DItemListView::SetCategoryInfo(ITEM_CATEGORY_INFO* a_pInfo)
{
	if(a_pInfo->m_eActionType == ACT_UPDATE)
		this->clear();
	m_pCategoryInfo = a_pInfo;
	m_nCategoryCode = m_pCategoryInfo->m_nCategoryCode;
	QVector<ITEM_INFO_FOR_UI*> items = m_pCategoryInfo->m_vItems;
	QVector<ITEM_INFO_FOR_UI*>::iterator it = items .begin();
	while(it != items.end())
	{
		ITEM_INFO* pInfo = (*it)->m_pItemInfo; 
		QString strPath = pInfo->m_sModelPath;
		strPath = g_pApp->GetResourcePath(strPath);
		switch((*it)->m_eActionType)
		{
		case ACT_ADD:
			{
				// if model data exist
				if(QFile::exists(strPath))
				{
					QListWidgetItem* pListWidgetItem = new QListWidgetItem(this);
					strPath = pInfo->m_sImagePath;
					strPath = g_pApp->GetResourcePath(strPath);
					// if 2d image doesn/t exist, then default image
					if(!QFile::exists(strPath))
					{
						strPath = DEFAULT_ITEM_IMG;
					}
					QVariant userData((long)pInfo);
					pListWidgetItem->setData(Qt::UserRole, userData);
					QIcon icon;
					icon.addFile(strPath, QSize(ITEM_WIDTH, ITEM_HEIGHT));
					pListWidgetItem->setIcon(icon);
					pListWidgetItem->setSizeHint(QSize(ITEM_WIDTH , ITEM_HEIGHT));
					this->addItem(pListWidgetItem);
				}
			}
		break;
		case ACT_REMOVE:
			{
				int nIndex = FindItemWidget(pInfo->m_sProductID);
				if(nIndex > -1)
				{
					QListWidgetItem* pWidgetItem = takeItem(nIndex);
					delete pWidgetItem;
				}
			}
			break;
		case ACT_UPDATE:
			{
				int nIndex = FindItemWidget(pInfo->m_sProductID);
				if(nIndex > -1)
				{
					QListWidgetItem* pListWidgetItem = item(nIndex);
					strPath = pInfo->m_sImagePath;
					strPath = g_pApp->GetResourcePath(strPath);
					// if 2d image doesn/t exist, then default image
					if(!QFile::exists(strPath))
					{
						strPath = DEFAULT_ITEM_IMG;
					}
					QVariant userData((long)pInfo);
					pListWidgetItem->setData(Qt::UserRole, userData);
					QIcon icon;
					icon.addFile(strPath, QSize(ITEM_WIDTH, ITEM_HEIGHT));
					pListWidgetItem->setIcon(icon);
					pListWidgetItem->setSizeHint(QSize(ITEM_WIDTH , ITEM_HEIGHT));
					this->addItem(pListWidgetItem);
				}
			}
			break;
		}
		(*it)->m_eActionType = ACT_NONE;
		it++;
	}
}

int CRoom3DItemListView::FindItemWidget(QString a_sProductID)
{
	int nCount = this->count();
	for(int i = 0; i < nCount; i++)
	{
		ITEM_INFO* pInfo = (ITEM_INFO*) item(i)->data(Qt::UserRole).toInt();
		if(pInfo->m_sProductID.compare(a_sProductID) == 0)
			return i;
	}
	return -1;
}

void CRoom3DItemListView::mouseDoubleClickEvent ( QMouseEvent * event )
{
	QListWidget::mouseDoubleClickEvent(event);
}

QStringList CRoom3DItemListView::mimeTypes() const
{
	QStringList types;
	types << QLatin1String(ITEM_DRAG_MIME_TYPE);
	return types;
}
void CRoom3DItemListView::startDrag(Qt::DropActions supportedActions)
{
	QModelIndexList *indexes = new QModelIndexList();//-temp
	*indexes = selectionModel()->selectedIndexes();
	QDrag *drag = new QDrag(this);
	QMimeData* pMimeData = model()->mimeData(*indexes);
	drag->setMimeData(pMimeData);
	Qt::DropAction action = drag->exec(supportedActions, Qt::CopyAction);
}

void CRoom3DItemListView::SelectItem(QString a_sProductID)
{
	int nIndex = FindItemWidget(a_sProductID);
	if(nIndex > -1)
	{
		this->setCurrentRow(nIndex);
	}
}

void CRoom3DItemListView::contextMenuEvent(QContextMenuEvent *event)
{
	CRoom3DMenu contextMenu(this);

	CRoom3DAction deleteAction(this);
	deleteAction.setText(tr("Delete Item"));

	contextMenu.addAction(&deleteAction);

	connect(&deleteAction, SIGNAL(triggered()), this, SLOT(OnDeleteItem()));
	if(selectedItems().count() > 0)
		deleteAction.setEnabled(true);
	else
		deleteAction.setEnabled(false);
	contextMenu.exec(event->globalPos());
}

void CRoom3DItemListView::OnDeleteItem()
{
	if(selectedItems().count() > 0)
	{
		if(QMessageBox::Ok == QMessageBox::question(g_pApp->GetMainWidget(),tr("Item delete"), tr("Are you sure delete the item?"), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok))
		{
			ITEM_INFO* pInfo = (ITEM_INFO*) selectedItems()[0]->data(Qt::UserRole).toInt();
			CRoom3DItemMgr* pItemMgr = g_pApp->GetDocument()->GetItemMgr();
			pItemMgr->DeleteItemInfo(pInfo->m_sProductID, m_nCategoryCode);
		}
	}
}

void CRoom3DItemListView::OnImportItem()
{
	return;
}