#include "Room3DItemMgr.h"
#include "Common.h"
#include "Room3DApplication.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDomDocument>
#include <QSqlError>
#include <QTemporaryFile>
#include "Room3DUtility.h"

#define ITEM_SERVER_ROOT_URL "http://192.168.14.160"
#define ITEM_SERVER_MAIN_URL QString(ITEM_SERVER_ROOT_URL) + QString("/ProductList.php")

CRoom3DItemMgr::CRoom3DItemMgr(void)
{
	m_pTexturePool = NULL;
	m_pTexturePool = new CTexturePool;
#if USE_THREAD
	m_pModelLoadThread = new CRoom3DModelLoadThread(this);
#endif
	m_pItemDownloadMgr = new CRoom3DItemDownloadMgr();
	connect(m_pItemDownloadMgr, SIGNAL(UpdateItemInfoData(QString, bool)), this, SLOT(OnUpdateItemInfoData(QString, bool)));
#if USE_THREAD
	connect(m_pModelLoadThread, SIGNAL(finished()), this, SLOT(OnModelLoadFinish()));
#endif
}

CRoom3DItemMgr::~CRoom3DItemMgr(void)
{
	CloseDatabase();
	ClearCategoryInfos();
	ClearModelInfos();
	SAFE_DELETE(m_pTexturePool);
#if USE_THREAD
	SAFE_DELETE(m_pModelLoadThread);
#endif
	m_categoryMap.clear();
}

void CRoom3DItemMgr::InitItemMgr()
{
	if(OpenDatabase())
	{
		UpdateAllDatabase();
		UpdateItemInfos();
		UpdateCategoryInfos();
	}
#if USE_THREAD
	m_pModelLoadThread->start(QThread::IdlePriority);
#else
	LoadModels();
#endif
}

#if USE_THREAD
void CRoom3DItemMgr::OnModelLoadFinish()
{
	Room3D* pMainWidget = g_pApp->GetMainWidget();
	pMainWidget->OnModelLoadFinish();
}

bool CRoom3DItemMgr::IsModelLoading()
{
	if(m_pModelLoadThread)
	{
		return m_pModelLoadThread->isRunning();
	}
	else
	{
		return false;
	}
}
#endif

void CRoom3DItemMgr::LoadModels()
{
	QVector<ITEM_INFO*>::Iterator itItem = m_vItems.begin();

	while(itItem != m_vItems.end())
	{
		ITEM_INFO* pInfo = (*itItem);

		if((!pInfo->m_sModelPath.isEmpty()) && (pInfo->m_pModel == NULL))
		{
			QString sModelPath = g_pApp->GetResourcePath(pInfo->m_sModelPath);

			CModel* pModel = new CModel;
			g_pApp->processEvents();
			if(pModel->Load(sModelPath.toUtf8().data()))
			{
				g_pApp->ThreadLock();
				pInfo->m_pModel = pModel;
				m_Models.push_back(pModel);
				g_pApp->ThreadUnLock();
			}
			else
			{
				SAFE_DELETE(pModel);
				pInfo->m_pModel = NULL;
			}

#if USE_THREAD
			if(m_pModelLoadThread)
				m_pModelLoadThread->MSleep(10);
#endif
		}

		itItem++;
	}
}

bool CRoom3DItemMgr::OpenDatabase()
{
	QSqlDatabase db = QSqlDatabase::addDatabase(DATABASE_TYPE);
	db.setConnectOptions(DATABSE_CONNECT_OPTION);
	db.setHostName("localhost");
	QString sDatabaseName = DATABASE_NAME_PREFIX + g_pApp->GetResourcePath(ITEM_DATABASE_FILE_NAME);
	db.setDatabaseName(sDatabaseName);
	if(false == db.open())
	{
		qDebug("Fail to open database. \n");
		QMessageBox::warning(NULL, tr("Room3D"),
			tr("Fail to open databse."));
		return false;
	}
	return true;
}

bool CRoom3DItemMgr::CloseDatabase()
{
	QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
	db.close();
	QSqlDatabase::removeDatabase(DATABSE_CONNECT_NAME);
	return true;
}

void CRoom3DItemMgr::ClearCategoryInfos()
{
	QVector<ITEM_CATEGORY_INFO*>::Iterator it = m_CategoryInfos.begin();
	while(it != m_CategoryInfos.end())
	{
		ClearCategoryInfo((*it));
		delete (*it);
		it++;
	}
	m_CategoryInfos.clear();
}

void CRoom3DItemMgr::ClearModelInfos()
{
	QVector<CModel*>::Iterator it = m_Models.begin();
	while(it != m_Models.end())
	{
		CModel* p = ((*it));
		SAFE_DELETE(p);
		it++;
	}
	m_Models.clear();
}

void CRoom3DItemMgr::ClearCategoryInfo(ITEM_CATEGORY_INFO* a_pCategoryInfo)
{
	QVector<ITEM_INFO_FOR_UI*>::Iterator it = a_pCategoryInfo->m_vItems.begin();
	while(it != a_pCategoryInfo->m_vItems.end())
	{
		ITEM_INFO_FOR_UI* pInfo = (*it);
		SAFE_DELETE(pInfo);
		it++;
	}
	a_pCategoryInfo->m_vItems.clear();
}

void CRoom3DItemMgr::UpdateItemInfos()
{
	QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
	QSqlQuery q;
	QString queryString = QString("SELECT * FROM ") + QString(ITEM_TABLE_NAME);
	if(q.exec(queryString) != true)
	{
		QMessageBox::warning(NULL, tr("Room3D"),
			tr("Fail to execute sql query."));
		return;
	}
	while(q.next())
	{
		ITEM_INFO* pInfo = new ITEM_INFO();
		UpdateItemInfoFromDB(q, pInfo);
		pInfo->m_pModel = NULL;
		m_vItems.push_back(pInfo);
		m_ItemMap.insert(pInfo->m_sProductID, pInfo);
	}
}

bool CRoom3DItemMgr::UpdateCategoryInfos()
{
	ClearCategoryInfos();
	QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
	QSqlQuery q;
	QString queryString = QString("SELECT * FROM ") + QString(MASTER_TABLE_NAME);
	if(q.exec(queryString) != true)
	{
		QMessageBox::warning(NULL, tr("Room3D"),
			tr("Fail to execute sql query."));
		return false;
	}

	int nNameIdx;
	if(g_pApp->GetLanguage() == CRoom3DApplication::Lang_Eng)
		nNameIdx = q.record().indexOf(FIELD_CATEGORY_NAME_ENG);
	else
		nNameIdx = q.record().indexOf(FIELD_CATEGORY_NAME_THAI);
	int nCodeIdx = q.record().indexOf(FIELD_CATEGORY_CODE);
	while(q.next())
	{
		ITEM_CATEGORY_INFO* pInfo = new ITEM_CATEGORY_INFO();
		pInfo->m_eActionType = ACT_ADD;
		pInfo->m_sCategoryName = q.value(nNameIdx).toString();
		pInfo->m_nCategoryCode = q.value(nCodeIdx).toInt();
		if(UpdateCategoryInfo(pInfo) == true)
			m_CategoryInfos.push_back(pInfo);
	}
	q.clear();
	return true;
}

bool CRoom3DItemMgr::UpdateCategoryInfo(ITEM_CATEGORY_INFO* a_pInfo)
{
	QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
	QSqlQuery q;
	QString sCategoryCode;
	sCategoryCode.setNum(a_pInfo->m_nCategoryCode);
	QString queryString = QString("SELECT * FROM ") + QString(ITEM_TABLE_NAME) + QString(" WHERE ") 
		+ QString(FIELD_CATEGORY_CODE1) + QString(" = ") + sCategoryCode
		+ " OR " + QString(FIELD_CATEGORY_CODE2) + QString(" = ") + sCategoryCode
		+ " OR " + QString(FIELD_CATEGORY_CODE3) + QString(" = ") + sCategoryCode;
	if(q.exec(queryString) != true)
	{
		QMessageBox::warning(NULL, tr("Room3D"),
			tr("Fail to execute sql query."));
		return false;
	}
	FillDataToCategoryInfo(q, a_pInfo);
	return true;
}

ITEM_CATEGORY_INFO* CRoom3DItemMgr::FindCategoriInfo(int a_nCategoryCode)
{
	QVector<ITEM_CATEGORY_INFO*>::iterator it = m_CategoryInfos.begin();
	while(it != m_CategoryInfos.end())
	{
		if((*it)->m_nCategoryCode == a_nCategoryCode)
			return (*it);
		it++;
	}
	return NULL;
}

bool CRoom3DItemMgr::Search(ITEM_SEARCH_INFO* a_pSearchInfo)
{
	ITEM_CATEGORY_INFO* pCatagoryInfo = FindCategoriInfo(a_pSearchInfo->m_nCategoryCode);
	if(pCatagoryInfo == NULL)
		return false;
	QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
	QSqlQuery q;
	QString sConditionOperator1 = " >= ";
	QString sConditionOperator2 = " <= ";
	QString sCategoryCode;
	sCategoryCode.setNum(pCatagoryInfo->m_nCategoryCode);
	QString sCondition = QString(" WHERE ") + " ( " 
		+ QString(FIELD_CATEGORY_CODE1) + QString(" = ") + sCategoryCode
		+ " OR " + QString(FIELD_CATEGORY_CODE2) + QString(" = ") + sCategoryCode
		+ " OR " + QString(FIELD_CATEGORY_CODE3) + QString(" = ") + sCategoryCode + " ) ";
	sCondition = sCondition + QString(" AND ")
		+ QString(FIELD_PRICE) + sConditionOperator1 + QVariant (a_pSearchInfo->m_fFromPrice).toString()
		+ QString(" AND ") + QString(FIELD_PRICE) + sConditionOperator2 + QVariant (a_pSearchInfo->m_fToPrice).toString();
	QString queryString = QString("SELECT * FROM ") + QString(ITEM_TABLE_NAME) + sCondition;
	if(q.exec(queryString) != true)
	{
		QMessageBox::warning(NULL, tr("Room3D"),
			tr("Fail to execute sql query."));
		return false;
	}
	pCatagoryInfo->m_eActionType = ACT_UPDATE;
	SetItemsActions(pCatagoryInfo, ACT_REMOVE);
	int nProductCodeIdx = q.record().indexOf(FIELD_PRODUCT_ID);
	while(q.next())
	{
		QString sProductID = q.value(nProductCodeIdx).toString();
		ITEM_INFO_FOR_UI* pInfo = GetItemInfoForUI(pCatagoryInfo, sProductID);
		if(pInfo)
			pInfo->m_eActionType = ACT_ADD;
	}
	return true;
}

ITEM_INFO* CRoom3DItemMgr::GetItemInfo(QString a_sProductID)
{
	ITEM_INFO* pInfo = NULL;
	g_pApp->ThreadLock();
	QMap<QString, ITEM_INFO*>::iterator it = m_ItemMap.find(a_sProductID);
	if(it != m_ItemMap.end())
		pInfo = (*it);
	g_pApp->ThreadUnLock();
	return pInfo;
}

ITEM_INFO_FOR_UI* CRoom3DItemMgr::GetItemInfoForUI(ITEM_CATEGORY_INFO* a_pCategoryInfo, QString a_sProductID)
{
	if(a_pCategoryInfo)
	{
		QVector<ITEM_INFO_FOR_UI*>::iterator it = a_pCategoryInfo->m_vItems.begin();
		while(it != a_pCategoryInfo->m_vItems.end())
		{
			if((*it)->m_sItemID.compare(a_sProductID) == 0)
			{
				return (*it);
			}
			it++;
		}
	}
	return NULL;
}


void CRoom3DItemMgr::ResetActions()
{
	QVector<ITEM_CATEGORY_INFO*>::iterator it = m_CategoryInfos.begin();
	while(it != m_CategoryInfos.end())
	{
		(*it)->m_eActionType = ACT_NONE;
		SetItemsActions((*it), ACT_NONE);
	}
}

void CRoom3DItemMgr::SetItemsActions(ITEM_CATEGORY_INFO* a_pCategoryInfo, ACTION_TYPE a_actionType)
{
	QVector<ITEM_INFO_FOR_UI*>::Iterator it = a_pCategoryInfo->m_vItems.begin();
	while(it != a_pCategoryInfo->m_vItems.end())
	{
		(*it)->m_eActionType = a_actionType;
		it++;
	}
}

void CRoom3DItemMgr::UpdateAllDatabase()
{
	// update category map.
	QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
	QSqlQuery q;
	QString queryString = QString("SELECT * FROM ") + QString(MASTER_TABLE_NAME);
	q.exec(queryString);
	int nNameIdx;
	nNameIdx = q.record().indexOf(FIELD_CATEGORY_NAME_ENG);
	int nCodeIdx = q.record().indexOf(FIELD_CATEGORY_CODE);
	while(q.next())
	{

		QString sCategoryName = q.value(nNameIdx).toString();
		int categoryCode = q.value(nCodeIdx).toInt();
		m_categoryMap.insert(sCategoryName, categoryCode);
	}
	q.clear();
	queryString = QString("SELECT * FROM ") + QString(ITEM_TABLE_NAME);
	if(false == q.exec(queryString))
	{
		QMessageBox::warning(NULL, tr("Room3D"),
			tr("Fail to execute sql query."));
		return;
	}
	// if table contains at least one item, then return , else update database from local folders.
	if(q.next())
		return;


	// update database from folder structure.
	QString sItemRootPath = g_pApp->GetResourcePath(ITEM_ROOT_PATH);
	QDir rootDir(sItemRootPath);
	QStringList sCategoryFolders = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList::const_iterator it = sCategoryFolders.constBegin();
	while (it != sCategoryFolders.constEnd())
	{
		QString sCategoryFolderPath = (*it);
		sCategoryFolderPath = sItemRootPath + QString(QDir::separator()) + sCategoryFolderPath;
		UpdateDatabaseFromCategoryFolder(sCategoryFolderPath);
		it++;
	}
}

void CRoom3DItemMgr::UpdateDatabaseFromCategoryFolder(QString& a_sCategoryFolderPath)
{
	QDir rootDir(a_sCategoryFolderPath);
	QStringList sItemPaths = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList::const_iterator it = sItemPaths.constBegin();
	bool blInserted;
	QString sProductID;
	while (it != sItemPaths.constEnd())
	{
		QString sItemPath = (*it);
		sItemPath = a_sCategoryFolderPath + QDir::separator() + sItemPath;
		// Update items's record from item folder
		UpdateDatabase(sItemPath, blInserted, sProductID);
		it++;
	}
}

void CRoom3DItemMgr::OnUpdateItemInfoData(QString a_ItemDataPath, bool error)
{
	if(!error)
	{
		bool blInserted;
		QString sProductID;
		// Update databse
		if(false == UpdateDatabase(a_ItemDataPath, blInserted, sProductID))
		{
			qDebug("Update database error !");
			return;
		}
		ITEM_INFO* pInfo = NULL;
		ITEM_INFO_FOR_UI* pInforUI = NULL;
		pInfo = GetItemInfo(sProductID);
		// If item is newly inserted or item is a deleted item, then read item info from DB and Update UI
		if(pInfo == NULL || pInfo->m_nDeleteFlg == 1)
		{
			// update ui
			bool blUpdateUI = true;
			if(pInfo)// this is deleted item.
			{
				pInfo->m_nDeleteFlg = 0; // set undeleted.
				UpdateItemInfoFromDB(pInfo, sProductID);
			}
			else
			{
				pInfo = new ITEM_INFO(); // new item info
				pInfo->m_pModel = NULL;
				// read item info from database.
				if(false == UpdateItemInfoFromDB(pInfo, sProductID))
				{
					SAFE_DELETE(pInfo);
					qDebug("[CRoom3DItemMgr::OnUpdateItemInfoData][UpdateItemInfoFromDB] Error!\n");
					return ;
				}
				if(!pInfo->m_sModelPath.isEmpty())
				{
					QString sModelPath = g_pApp->GetResourcePath(pInfo->m_sModelPath);
					CModel* pModel = new CModel;
					if(pModel->Load(sModelPath.toUtf8().data()))
					{
						m_vItems.push_back(pInfo);
						m_ItemMap.insert(pInfo->m_sProductID, pInfo);
						pInfo->m_pModel = pModel;
						m_Models.push_back(pModel);
					}
					else
					{
						qDebug("[CRoom3DItemMgr::OnUpdateItemInfoData][pModel->Load] Error!\n");
						SAFE_DELETE(pModel);
						pInfo->m_pModel = NULL;
						blUpdateUI = false;
					}
				}
				else
					blUpdateUI = false;
				if(!blUpdateUI)
				{
					SAFE_DELETE(pInfo);
				}
			}
#define ADD_ITEM_INFO_UI(a) if(a)\
			{\
			pInforUI = new ITEM_INFO_FOR_UI();\
			pInforUI->m_sItemID = pInfo->m_sProductID;\
			pInforUI->m_nCategoryCode = a->m_nCategoryCode;\
			pInforUI->m_pItemInfo = pInfo;\
			pInforUI->m_eActionType = ACT_ADD;\
			a->m_vItems.push_back(pInforUI);\
			}
			if(blUpdateUI)
			{
				ITEM_CATEGORY_INFO* pCategoryInfo = FindCategoriInfo(pInfo->m_nCategoryCode1);
				ADD_ITEM_INFO_UI(pCategoryInfo);
				pCategoryInfo = FindCategoriInfo(pInfo->m_nCategoryCode2);
				ADD_ITEM_INFO_UI(pCategoryInfo);
				pCategoryInfo = FindCategoriInfo(pInfo->m_nCategoryCode3);
				ADD_ITEM_INFO_UI(pCategoryInfo);
			}
		}
		else // if item is updated.
		{
			 // update all category with sProduct ID
			pInfo = GetItemInfo(sProductID);
			UpdateItemInfoFromDB(pInfo, sProductID);
#define UPDATE_ITEM_INFO_UI(a, b) pTempInfoUI = GetItemInfoForUI(a, b);\
			if(pTempInfoUI)\
			{\
				pTempInfoUI->m_eActionType = ACT_UPDATE;\
				pInforUI = pTempInfoUI;\
			}
			ITEM_INFO_FOR_UI* pTempInfoUI;
			ITEM_CATEGORY_INFO* pCategoryInfo = FindCategoriInfo(pInfo->m_nCategoryCode1);
			UPDATE_ITEM_INFO_UI(pCategoryInfo, pInfo->m_sProductID);
			UPDATE_ITEM_INFO_UI(pCategoryInfo, pInfo->m_sProductID);
			UPDATE_ITEM_INFO_UI(pCategoryInfo, pInfo->m_sProductID);
		}
		if(m_pItemDownloadMgr->IsLastResponse())
		{
			g_pApp->GetMainWidget()->UpdateControlState(pInforUI);
		}
	}
}

bool CRoom3DItemMgr::UpdateDatabase(QString& a_sItemDirPath, bool& a_blInserted, QString& a_sProductID)
{
	QString itemInfoXMLPath = a_sItemDirPath + QDir::separator() + ITEM_INFO_XML_FILE_NAME;
	// find out relative path for model and 2d image path in database.
	QString sRelativePath("");
	QString sMask = QString(QDir::separator()) + RELATIVE_PATH_MASK + QString(QDir::separator());
	int nPos = a_sItemDirPath.lastIndexOf(sMask);
	if(nPos > -1)
	{
		sRelativePath = a_sItemDirPath.right(a_sItemDirPath.size() - nPos - 1);
	}

	bool blRet = true;
	ITEM_INFO_FOR_DB_UPDATE	itemInfo;
	itemInfo.m_sImagePath = sRelativePath + QDir::separator() + IMAGE_FILE_NAME;
	itemInfo.m_sModelPath = sRelativePath + QDir::separator() + MODEL_FILE_NAME;
	itemInfo.m_nDeleteFlg = 0;
	blRet = GetItemInfoFromXML(a_sItemDirPath, &itemInfo);
	if(blRet)
	{
		blRet = UpdateItemRecord(&itemInfo,a_blInserted);
		a_sProductID = itemInfo.m_sProductID;
	}
	return blRet;
}

bool CRoom3DItemMgr::UpdateItemRecord(ITEM_INFO_FOR_DB_UPDATE* a_pItemInfo, bool& a_blInserted)
{
	if(IsAvailableItemInfo(a_pItemInfo))
	{
		QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
		QSqlQuery q;
		QString sSelectQuery = QString("SELECT * FROM ") + ITEM_TABLE_NAME + " WHERE " + FIELD_PRODUCT_ID + "='" + a_pItemInfo->m_sProductID + "'";
		if(q.exec(sSelectQuery) != true)
		{
			QMessageBox::warning(NULL, tr("Room3D"),
				tr("Fail to execute sql query."));
			return false;
		}
		bool blInsert;

		if(q.next() == false)
		{
			blInsert = true;
		}
		else
		{
			blInsert = false;
		}
		
		q.clear();
		QString sSqlCommand;
		if(!blInsert)
		{
			sSqlCommand = QString("UPDATE ") + ITEM_TABLE_NAME + " SET " + FIELD_CATEGORY_CODE1 + "=" + QString().setNum(a_pItemInfo->m_nCategoryCode1) +","
				+ FIELD_CATEGORY_CODE2 + "=" + QString().setNum(a_pItemInfo->m_nCategoryCode2) +"," + FIELD_CATEGORY_CODE3 + "=" + QString().setNum(a_pItemInfo->m_nCategoryCode3) +","
				+ FIELD_PRODUCT_NAME + "='" + a_pItemInfo->m_sName + "'," + FIELD_PRODUCT_ID + "='" + a_pItemInfo->m_sProductID + "'," + FIELD_DESCRIPTION +"='"
				+ a_pItemInfo->m_sDesc + "'," + FIELD_PRICE + "=" + QString().setNum(a_pItemInfo->m_fPrice, 'f', 2) + "," + FIELD_PRICE_NOTE + "='"
				+ a_pItemInfo->m_sPriceNote + "'," + FIELD_WIDTH + "=" + QString().setNum(a_pItemInfo->m_fWidth) + "," + FIELD_LENGTH + "="
				+ QString().setNum(a_pItemInfo->m_fLength, 'f', 2) + "," + FIELD_HEIGHT + "=" + QString().setNum(a_pItemInfo->m_fHeight, 'f', 2)
				+ "," + FIELD_IMAGE_PATH + "='" + a_pItemInfo->m_sImagePath + "'," + FIELD_MODEL_PATH + "='" + a_pItemInfo->m_sModelPath + "',"
				+ FIELD_UPDATED_DATE + "='" + a_pItemInfo->m_sUpdateDate + "'," + FIELD_DELETE_FLAG + "=" + QString().setNum(a_pItemInfo->m_nDeleteFlg)
				+ " WHERE " + FIELD_PRODUCT_ID + "='" + a_pItemInfo->m_sProductID + "'" ;
		}
		else
		{
			sSqlCommand = QString("INSERT INTO ") + ITEM_TABLE_NAME
				+ "(" 
				+ FIELD_CATEGORY_CODE1 + "," FIELD_CATEGORY_CODE2 + "," FIELD_CATEGORY_CODE3 + "," + FIELD_PRODUCT_NAME + "," + FIELD_PRODUCT_ID + "," + FIELD_DESCRIPTION + ","
				+ FIELD_PRICE + "," + FIELD_PRICE_NOTE + "," + FIELD_WIDTH + "," + FIELD_LENGTH + "," + FIELD_HEIGHT + ","
				+ FIELD_IMAGE_PATH + "," + FIELD_MODEL_PATH + "," + FIELD_UPDATED_DATE + "," + FIELD_DELETE_FLAG
				+ ")" 
				" VALUES (" 
				+ QString().setNum(a_pItemInfo->m_nCategoryCode1) + "," + QString().setNum(a_pItemInfo->m_nCategoryCode2) + "," + QString().setNum(a_pItemInfo->m_nCategoryCode3) + ",'"  
				+ a_pItemInfo->m_sName + "','" + a_pItemInfo->m_sProductID + "','" + a_pItemInfo->m_sDesc + "',"
				+ QString().setNum(a_pItemInfo->m_fPrice, 'f', 2) + ",'" + a_pItemInfo->m_sPriceNote + "'," +  QString().setNum(a_pItemInfo->m_fWidth, 'f', 2) + ","
				+ QString().setNum(a_pItemInfo->m_fLength, 'f', 2) + "," + QString().setNum(a_pItemInfo->m_fHeight, 'f', 2) + ",'"
				+ a_pItemInfo->m_sImagePath + "','" + a_pItemInfo->m_sModelPath + "','" + a_pItemInfo->m_sUpdateDate + "'," + "0"
				+ ")";
		}
		if(q.exec(sSqlCommand) != true)
			return false;
		a_blInserted = blInsert;
		return true;
	}
	else 
		return false;
}

int CRoom3DItemMgr::GetCategoryCode(QString& a_sCategoryName, bool& a_blNewAdded)
{
	if(a_sCategoryName.isEmpty())
		return 0;
	QMap<QString, int>::iterator it = m_categoryMap.find(a_sCategoryName);
	int nCode;
	if(it != m_categoryMap.end())
	{
		nCode = *it;
		a_blNewAdded = false;
		return nCode;
	} 
	a_blNewAdded = true;
	it = m_categoryMap.begin();
	int nNewCategoryCode = 0;
	while(it != m_categoryMap.end())
	{
		if(nNewCategoryCode < *it)
			nNewCategoryCode = *it;
		it++;
	}
	nNewCategoryCode++;
	QString sCategoryCode;
	sCategoryCode.setNum(nNewCategoryCode);	

	QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
	QSqlQuery q;
	QString queryString = QString("INSERT INTO ") + QString(MASTER_TABLE_NAME) + "(" + FIELD_CATEGORY_NAME_ENG + "," + FIELD_CATEGORY_CODE + ")"
		" VALUES ('" + a_sCategoryName + "'," + sCategoryCode + ")";
	if(q.exec(queryString) != true)
	{
		QMessageBox::warning(NULL, tr("Room3D"),
			tr("Fail to execute sql query."));
		return -1;
	}
	m_categoryMap.insert(a_sCategoryName, nNewCategoryCode);
	return nNewCategoryCode;
}

bool CRoom3DItemMgr::IsAvailableItemInfo(ITEM_INFO_FOR_DB_UPDATE* a_pItemInfo)
{
	QString sModelPath = g_pApp->GetResourcePath(a_pItemInfo->m_sModelPath);
	if(!QFile::exists(sModelPath))
		return false;
	if(a_pItemInfo->m_nCategoryCode1 < 0 && a_pItemInfo->m_nCategoryCode2 < 0 
		&& a_pItemInfo->m_nCategoryCode3 < 0)
		return false;
	if(a_pItemInfo->m_sProductID.isEmpty())
		return false;
	return true;
}


bool CRoom3DItemMgr::UpdateItemInfoFromDB(ITEM_INFO* a_pInfo, QString a_sProductID)
{
	QSqlDatabase db = QSqlDatabase::database(DATABSE_CONNECT_NAME, false);
	QSqlQuery q;
	QString queryString = QString("SELECT * FROM ") + QString(ITEM_TABLE_NAME) + QString(" WHERE ") 
	+ FIELD_PRODUCT_ID + QString(" = '") + a_sProductID + "'";
	if(q.exec(queryString) != true)
	{
		QMessageBox::warning(NULL, tr("Room3D"),
			tr("Fail to execute sql query."));
		return false;
	}
	bool blRet = true;
	if(q.next())
		blRet = UpdateItemInfoFromDB(q, a_pInfo);
	else
		blRet = false;
	return blRet;
}

bool CRoom3DItemMgr::UpdateItemInfoFromDB(QSqlQuery& a_q, ITEM_INFO* a_pInfo)
{
	ITEM_INFO* pInfo = a_pInfo;
	int nProductNameIdx = a_q.record().indexOf(FIELD_PRODUCT_NAME);
	int nCategoryCodeIdx1 = a_q.record().indexOf(FIELD_CATEGORY_CODE1);
	int nCategoryCodeIdx2 = a_q.record().indexOf(FIELD_CATEGORY_CODE2);
	int nCategoryCodeIdx3 = a_q.record().indexOf(FIELD_CATEGORY_CODE3);
	int nProductCodeIdx = a_q.record().indexOf(FIELD_PRODUCT_ID);
	int nDescIdx = a_q.record().indexOf(FIELD_DESCRIPTION);
	int nPriceIdx = a_q.record().indexOf(FIELD_PRICE);
	int nPriceNoteIdx = a_q.record().indexOf(FIELD_PRICE_NOTE);
	int nWidthIdx = a_q.record().indexOf(FIELD_WIDTH);
	int nLengthIdx = a_q.record().indexOf(FIELD_LENGTH);
	int nHeightIdx = a_q.record().indexOf(FIELD_HEIGHT);
	int nImgPathdx = a_q.record().indexOf(FIELD_IMAGE_PATH);
	int nModelPathIdx = a_q.record().indexOf(FIELD_MODEL_PATH);
	int nUpdatedDateIdx = a_q.record().indexOf(FIELD_UPDATED_DATE);
	int nDeleteFlagIdx = a_q.record().indexOf(FIELD_DELETE_FLAG);

	pInfo->m_nCategoryCode1 = a_q.value(nCategoryCodeIdx1).toInt();
	pInfo->m_nCategoryCode2 = a_q.value(nCategoryCodeIdx2).toInt();
	pInfo->m_nCategoryCode3 = a_q.value(nCategoryCodeIdx3).toInt();
	pInfo->m_sProductID = a_q.value(nProductCodeIdx).toString();
	pInfo->m_sName = a_q.value(nProductNameIdx).toString();
	pInfo->m_sDesc = a_q.value(nDescIdx).toString();
	pInfo->m_fPrice = a_q.value(nPriceIdx).toFloat();
	pInfo->m_sPriceNote = a_q.value(nPriceNoteIdx).toString();
	pInfo->m_fWidth = a_q.value(nWidthIdx).toFloat();
	pInfo->m_fLength = a_q.value(nLengthIdx).toFloat();
	pInfo->m_fHeight = a_q.value(nHeightIdx).toFloat();
	pInfo->m_sImagePath = a_q.value(nImgPathdx).toString();
	pInfo->m_sModelPath = a_q.value(nModelPathIdx).toString();
	pInfo->m_sUpdateDate = a_q.value(nUpdatedDateIdx).toString();
	pInfo->m_nDeleteFlg = a_q.value(nDeleteFlagIdx).toInt();
	return true;
}


void CRoom3DItemMgr::FillDataToCategoryInfo(QSqlQuery& a_q, ITEM_CATEGORY_INFO* a_pCategoryInfo)
{
	while(a_q.next())
	{
		int nProductCodeIdx = a_q.record().indexOf(FIELD_PRODUCT_ID);
		QString sProductID = a_q.value(nProductCodeIdx).toString();
		ITEM_INFO* pItemInfo = GetItemInfo(sProductID);
		if(pItemInfo && pItemInfo->m_nDeleteFlg == 0)
		{
			ITEM_INFO_FOR_UI* pInfo = new ITEM_INFO_FOR_UI();
			pInfo->m_sItemID = sProductID;
			pInfo->m_nCategoryCode = a_pCategoryInfo->m_nCategoryCode;
			pInfo->m_eActionType = ACT_ADD;
			pInfo->m_pItemInfo = GetItemInfo(sProductID);
			a_pCategoryInfo->m_vItems.push_back(pInfo);
		}
	}
}

bool CRoom3DItemMgr::DeleteItemInfo(QString a_sProductID, int a_nCategoryCode)
{
	ITEM_INFO* pInfo = GetItemInfo(a_sProductID);
	ITEM_CATEGORY_INFO* pCategoryInfo;
	int nCount, i;
	ITEM_INFO_FOR_UI* pInfoUI;
	// update database
	bool blInserted;
	pInfo->m_nDeleteFlg = 1;
	UpdateItemRecord(pInfo, blInserted);
	if(blInserted)
	{
		qDebug("[CRoom3DItemMgr::DeleteItemInfo][UpdateItemRecord] Error! \n");
	}
	// update UI and memory data
#define DELETE_ITEM_INFO(a) if(a) { nCount = a->m_vItems.count();\
		pInfoUI = NULL;\
		for(i = 0; i < nCount; i++)\
		{\
			if(a->m_vItems[i]->m_sItemID.compare(a_sProductID) == 0)\
			{\
				a->m_vItems[i]->m_eActionType = ACT_REMOVE;\
				if(a_nCategoryCode == a->m_nCategoryCode)\
					pInfoUI = a->m_vItems[i];\
				g_pApp->GetMainWidget()->UpdateControlState(NULL);\
				a->m_vItems.remove(i);\
				break;\
			}\
		}\
	}
	pCategoryInfo = FindCategoriInfo(pInfo->m_nCategoryCode1);
	DELETE_ITEM_INFO(pCategoryInfo);
	pCategoryInfo = FindCategoriInfo(pInfo->m_nCategoryCode2);
	DELETE_ITEM_INFO(pCategoryInfo);
	pCategoryInfo = FindCategoriInfo(pInfo->m_nCategoryCode3);
	DELETE_ITEM_INFO(pCategoryInfo);
	return true;
}

bool CRoom3DItemMgr::ImportItem(QString a_sSrcFolderPath)
{
	QString sDestPath;
	ITEM_INFO srcInfo;
	if(GetItemInfoFromXML(a_sSrcFolderPath, &srcInfo))
	{
		GetExtractFolderPath(&srcInfo, sDestPath);
		sDestPath.replace("/", QDir::separator());
		QDir dir(a_sSrcFolderPath);
		sDestPath = sDestPath + QDir::separator() + dir.dirName();
		QDir dstDir(sDestPath);
		if(a_sSrcFolderPath.compare(sDestPath, Qt::CaseInsensitive) != 0)
		{
			dstDir.rmpath(sDestPath);
			CopyDirectory(a_sSrcFolderPath, sDestPath);
		}
		OnUpdateItemInfoData(sDestPath, false);
		return true;
	}
	else
	{
		qDebug("[CRoom3DItemMgr::ImportItem][GetItemInfoFromXML]Error! \n");
		return false;
	}
}
/********************************************************************************************/
/************													*****************************/
/************	CRoom3DItemDownloadMgr	Implementation			*****************************/
/************													*****************************/
/********************************************************************************************/


CRoom3DItemDownloadMgr::CRoom3DItemDownloadMgr()
{
	// Initialize http
	m_pHttp = new QHttp(this);

	connect(m_pHttp, SIGNAL(requestFinished(int, bool)),this, SLOT(httpRequestFinished(int, bool)));
	connect(m_pHttp, SIGNAL(dataReadProgress(int, int)),this, SLOT(updateDataReadProgress(int, int)));
	connect(m_pHttp, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
	connect(m_pHttp, SIGNAL(authenticationRequired(const QString &, quint16, QAuthenticator *)),this, SLOT(slotAuthenticationRequired(const QString &, quint16, QAuthenticator *)));

	m_nRequestCount = 0;

	m_pListner = NULL;
}

CRoom3DItemDownloadMgr::~CRoom3DItemDownloadMgr()
{
	ClearItemInfoEx();
}

void CRoom3DItemDownloadMgr::RequestDownloadItemList()
{
	ClearItemInfoEx();
	m_nRequestCount = 0;
	DownloadPage(m_pHttp, ITEM_SERVER_MAIN_URL, REQ_TYPE_ITEM_LIST);
}
void CRoom3DItemDownloadMgr::DownloadPage(QHttp* a_pHttp, QString a_sURL, REQUEST_TYPE a_requestType, void* a_pUserData)
{
	QUrl url(a_sURL);
	QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
	a_pHttp->setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());

	if (!url.userName().isEmpty())
		a_pHttp->setUser(url.userName(), url.password());

	QByteArray path = QUrl::toPercentEncoding(url.path(), "!$&'()*+,;=:@/");
	if (path.isEmpty())
		path = "/";
	RESPONSE_INFO* pResponseInfo = new RESPONSE_INFO();
	pResponseInfo->m_requestType = a_requestType;
	pResponseInfo->m_pUserData = a_pUserData;

	int httpGetId = a_pHttp->get(path, &pResponseInfo->m_Buffer);
	pResponseInfo->m_nRequestID = httpGetId;
	m_pResponseInfos.push_back(pResponseInfo);
	m_nRequestCount++;
}

RESPONSE_INFO* CRoom3DItemDownloadMgr::GetResponseInfo(int a_nRequestID)
{
	QVector<RESPONSE_INFO*>::iterator it = m_pResponseInfos.begin();
	while(it != m_pResponseInfos.end())
	{
		if((*it)->m_nRequestID == a_nRequestID)
			return (*it);
		it++;
	}
	return NULL;
}


void CRoom3DItemDownloadMgr::httpRequestFinished(int requestId, bool error)
{
	RESPONSE_INFO* pResponseInfo = GetResponseInfo(requestId);

	if(pResponseInfo == NULL)
		return;
	switch(pResponseInfo->m_requestType)
	{
	case REQ_TYPE_ITEM_LIST:
		m_nRequestCount--;
		UpdateItemInfoList(pResponseInfo, error);
		Request2dImage();
		emit UpdateItemInfoUI(error);
		break;
	case REQ_TYPE_ITEM_IMAGE:
		m_nRequestCount--;
		UpdateItemInfoImage(pResponseInfo, error);
		break;
	case REQ_TYPE_ITEM_DATA:
		m_nRequestCount--;
		UpdateItemData(pResponseInfo, error);
		break;
	}

	if(m_pListner)
		m_pListner->OnRequestFinish();

	int nIndex = m_pResponseInfos.indexOf(pResponseInfo);
	m_pResponseInfos.remove(nIndex);
	delete pResponseInfo;
}

void CRoom3DItemDownloadMgr::UpdateItemInfoList(RESPONSE_INFO* a_pResponseInfo, bool error)
{
	QBuffer* pBuffer = &a_pResponseInfo->m_Buffer;
	QDomDocument doc;
	doc.setContent(pBuffer->data());
	QDomElement rootDocElem = doc.documentElement();
	QDomNode childNode = rootDocElem.firstChild();
	while(!childNode.isNull()) {
		QDomElement childElem = childNode.toElement(); // try to convert the node to an element.
		if(!childElem.isNull()) {
			// fill all information to update database's record from dom element.
			ITEM_INFO_EX* pItemInfo = new ITEM_INFO_EX();
			pItemInfo->m_nDeleteFlg = 0;
			if(FillDataToItemInfoEx(childElem, pItemInfo))
			{
				m_pItems.push_back(pItemInfo);
			}
			else
				delete pItemInfo;
		}
		childNode = childNode.nextSibling();
	}
}

void CRoom3DItemDownloadMgr::Request2dImage()
{
	m_pHttp->clearPendingRequests();
	m_nRequestCount = 0;
	QVector<ITEM_INFO_EX*>::iterator it = m_pItems.begin();
	while(it != m_pItems.end())
	{
		QString sURL(ITEM_SERVER_ROOT_URL);
		ITEM_INFO_EX* pItemInfo = (*it);
		if(!pItemInfo->m_sImagePath.isEmpty())
		{
			sURL = sURL + "/" + (*it)->m_sImagePath;
			DownloadPage(m_pHttp, sURL, REQ_TYPE_ITEM_IMAGE, pItemInfo);
		}
		it++;
	}
}

void CRoom3DItemDownloadMgr::UpdateItemInfoImage(RESPONSE_INFO* a_pResponseInfo, bool error)
{
	ITEM_INFO_EX* pItemInfo = (ITEM_INFO_EX*) a_pResponseInfo->m_pUserData;
	QPixmap pixmap;
	pixmap.loadFromData(a_pResponseInfo->m_Buffer.data());
	int nIndex = m_pItems.indexOf(pItemInfo);
	emit UpdateItemInfoUIImage(nIndex, pixmap, error);
}

void CRoom3DItemDownloadMgr::UpdateItemData(RESPONSE_INFO* a_pResponseInfo, bool error)
{
	ITEM_INFO_EX* pItemInfo = (ITEM_INFO_EX*) a_pResponseInfo->m_pUserData;
	QByteArray dataBytes = a_pResponseInfo->m_Buffer.data();
	int nLength = dataBytes.length();
	char* pData = dataBytes.data();
	QString sExtractPath;
	GetExtractFolderPath(pItemInfo, sExtractPath);
	QDir dir(sExtractPath);
	if(false == dir.mkpath(sExtractPath))
	{
		return;
	}
	UnZipArchive(pData, nLength, sExtractPath);
	emit UpdateItemInfoData(sExtractPath, error);
}

void CRoom3DItemDownloadMgr::updateDataReadProgress(int bytesRead, int totalBytes)
{
	float fPercent = (float)bytesRead / (float)totalBytes * 100.f;
	if(m_pListner)
		m_pListner->SetPercent(fPercent);
}

void CRoom3DItemDownloadMgr::readResponseHeader(const QHttpResponseHeader &responseHeader)
{

}

void CRoom3DItemDownloadMgr::slotAuthenticationRequired(const QString &hostName, quint16, QAuthenticator *authenticator)
{

}

bool CRoom3DItemDownloadMgr::FillDataToItemInfoEx(QDomElement& a_DomElem, ITEM_INFO_EX* a_pItemInfo)
{
	QDomNode childNode = a_DomElem.firstChild();
	while(!childNode.isNull()) {
		QDomElement childElem = childNode.toElement(); // try to convert the node to an element.
		if(!childElem.isNull()) {
			// fill all information to update database's record from dom element.
			QString strName = childElem.tagName();
			QString strValue = childElem.text();
			if(strName.compare(XML_TAG_PRODUCT_NAME, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_sName = strValue;
			}
			else if(strName.compare(XML_TAG_PRODUCT_CATEGORY1, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_nCategoryCode1 = 0;
			}
			else if(strName.compare(XML_TAG_PRODUCT_ID, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_sProductID = strValue;
			}
			else if(strName.compare(XML_TAG_PRODUCT_DESC, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_sDesc = strValue;
			}
			else if(strName.compare(XML_TAG_PRODUCT_PRICE, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_fPrice = strValue.toFloat();
			}
			else if(strName.compare(XML_TAG_PRODUCT_PRICE_NOTE, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_sPriceNote = strValue;
			}
			else if(strName.compare(XML_TAG_PRODUCT_WIDTH, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_fWidth = strValue.toFloat();
			}
			else if(strName.compare(XML_TAG_PRODUCT_LENGTH, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_fLength = strValue.toFloat();
			}
			else if(strName.compare(XML_TAG_PRODUCT_HEIGHT, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_fHeight = strValue.toFloat();
			}
			else if(strName.compare(XML_TAG_PRODUCT_IMG_PATH, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_sImagePath = strValue;
			}
			else if(strName.compare(XML_TAG_PRODUCT_Path, Qt::CaseInsensitive) == 0)
			{
				a_pItemInfo->m_sModelPath = strValue;
			}
			else
			{
				qDebug("[CRoom3DItemMgr::FillDataToItemInfo] error! \n");
			}
		}
		childNode = childNode.nextSibling();
	}
	return true;
}


void CRoom3DItemDownloadMgr::RequestDownloadItemDatas(QVector<ITEM_INFO_EX*>& a_pSelItems)
{
	m_pHttp->clearPendingRequests();
	m_nRequestCount = 0;
	QVector<ITEM_INFO_EX*>::iterator it = a_pSelItems.begin();
	while(it != a_pSelItems.end())
	{
		QString sURL(ITEM_SERVER_ROOT_URL);
		ITEM_INFO_EX* pItemInfo = (*it);
		if(!pItemInfo->m_sModelPath.isEmpty())
		{
			sURL = sURL + "/" + (*it)->m_sModelPath;
			DownloadPage(m_pHttp, sURL, REQ_TYPE_ITEM_DATA, pItemInfo);
		}
		it++;
	}
}

void CRoom3DItemDownloadMgr::RequestDownloadItemDatas(QString a_sURL)
{
	m_pHttp->clearPendingRequests();
	m_nRequestCount = 0;
	DownloadPage(m_pHttp, a_sURL, REQ_TYPE_ITEM_DATA);
}

void CRoom3DItemDownloadMgr::SetListner(CRoom3DWebDlg* pListner)
{
	m_pListner = pListner;
}

void CRoom3DItemDownloadMgr::ClearItemInfoEx()
{
	QVector<ITEM_INFO_EX*>::iterator it = m_pItems.begin();
	while(it != m_pItems.end())
	{
		delete (*it);
		it++;
	}
	m_pItems.clear();
}

#if USE_THREAD
CRoom3DModelLoadThread::CRoom3DModelLoadThread(CRoom3DItemMgr* pParent)
{
	m_pParent = pParent;
}

CRoom3DModelLoadThread::~CRoom3DModelLoadThread()
{

}

void CRoom3DModelLoadThread::MSleep(int nMiliSecond)
{
	this->msleep(nMiliSecond);
}

void CRoom3DModelLoadThread::run()
{
	if(m_pParent)
		m_pParent->LoadModels();
}
#endif