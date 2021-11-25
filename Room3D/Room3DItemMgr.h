//----------------------------------------------//
//	File: Room3DItemMgr.h						//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once
#include <QDateTime>
#include <QVector>
#include "Global.h"
#include "Model.h"
#include "TexturePool.h"
#include <QMap>
#include <QDomElement>
#include <QSqlQuery>
#include <QHttpResponseHeader>
#include <QBuffer>
#include <QPixmap>
#include "Room3DWebDlg.h"
#include <QThread>

enum ACTION_TYPE
{
	ACT_NONE,
	ACT_ADD,
	ACT_REMOVE,
	ACT_UPDATE
};

struct ITEM_CATEGORY_INFO;
typedef struct ITEM_INFO
{
	int		m_nCategoryCode1;
	int		m_nCategoryCode2;
	int		m_nCategoryCode3;
	QString m_sProductID;
	QString m_sName; // product name
	QString m_sDesc;
	float	m_fPrice;
	QString m_sPriceNote;
	float	m_fWidth;
	float	m_fLength;
	float	m_fHeight;
	QString m_sImagePath;
	QString m_sModelPath;
	QString	m_sUpdateDate;
	CModel* m_pModel;
	int		m_nDeleteFlg; // 0 -> not delete , 1 -> delete
} ITEM_INFO, *PITEM_INFO;

typedef ITEM_INFO ITEM_INFO_FOR_DB_UPDATE;

struct ITEM_INFO_FOR_UI
{
	QString m_sItemID;
	int m_nCategoryCode;
	ACTION_TYPE	m_eActionType;
	ITEM_INFO* m_pItemInfo;
};

typedef struct ITEM_CATEGORY_INFO
{
	QString m_sCategoryName;
	int		m_nCategoryCode;
	QVector<ITEM_INFO_FOR_UI*> m_vItems;
	ACTION_TYPE	m_eActionType;
} ITEM_CATEGORY_INFO;

struct ITEM_SEARCH_INFO
{
	int		m_nCategoryCode;
	int		m_nPriceComboIndex;
	float	m_fFromPrice;
	float	m_fToPrice;
} ;

enum REQUEST_TYPE
{
	REQ_TYPE_ITEM_LIST,
	REQ_TYPE_ITEM_IMAGE,
	REQ_TYPE_ITEM_DATA
};

struct RESPONSE_INFO
{
	REQUEST_TYPE	m_requestType;
	int m_nRequestID;
	QBuffer m_Buffer;
	void*	m_pUserData;
};

typedef ITEM_INFO_FOR_DB_UPDATE		ITEM_INFO_EX;

class CRoom3DItemDownloadMgr : public QObject
{
	Q_OBJECT
public:
	CRoom3DItemDownloadMgr();
	~CRoom3DItemDownloadMgr();
public:
	QVector<ITEM_INFO_EX*>& GetItemInfoList(){return m_pItems;}
	void RequestDownloadItemList();
	bool IsLastResponse(){return m_nRequestCount > 0 ? false : true;}
	void RequestDownloadItemDatas(QVector<ITEM_INFO_EX*>& a_pSelItems);
	void RequestDownloadItemDatas(QString a_sURL);
	void SetListner(CRoom3DWebDlg* pListner);
signals:
	void UpdateItemInfoUI(bool error);
	void UpdateItemInfoUIImage(int nRowIndex, QPixmap& a_pixmap, bool error);
	void UpdateItemInfoData(QString a_ItemDataPath, bool error);
protected slots:
	void httpRequestFinished(int requestId, bool error);
	void updateDataReadProgress(int bytesRead, int totalBytes);
	void readResponseHeader(const QHttpResponseHeader &responseHeader);
	void slotAuthenticationRequired(const QString &hostName, quint16, QAuthenticator *authenticator);

protected:
	void DownloadPage(QHttp* a_pHttp, QString a_sURL, REQUEST_TYPE a_requestType, void* a_pUserData = NULL);
	void UpdateItemInfoList(RESPONSE_INFO* a_pResponseInfo, bool error);
	bool FillDataToItemInfoEx(QDomElement& a_DomElem, ITEM_INFO_EX* a_pItemInfo);
	void Request2dImage();
	void UpdateItemInfoImage(RESPONSE_INFO* a_pResponseInfo, bool error);
	void UpdateItemData(RESPONSE_INFO* a_pResponseInfo, bool error);
	RESPONSE_INFO* GetResponseInfo(int a_nRequestID);
	void ClearItemInfoEx();

protected:
	QHttp*					m_pHttp;
	QVector<ITEM_INFO_EX*>  m_pItems;
	QVector<RESPONSE_INFO*> m_pResponseInfos;
	int						m_nRequestCount;
	CRoom3DWebDlg*			m_pListner;

};

#if USE_THREAD
/*!
	\class CRoom3DModelLoadThread
	\brief This is class to load model items.(by 001, 2012/6/21).
	\sa 
*/
class CRoom3DItemMgr;

class CRoom3DModelLoadThread : public QThread
{
	Q_OBJECT
public:
	CRoom3DModelLoadThread(CRoom3DItemMgr* pParent);
	~CRoom3DModelLoadThread();
public:
	void MSleep(int nMiliSecond);
protected:
	void run();

protected:
	CRoom3DItemMgr* m_pParent;
};
#endif

/*!
	\class CRoom3DItemMgr
	\brief This is class to manage furnitur items.(by 001, 2012/6/21).
	\sa 
*/
class CRoom3DItemMgr : public QObject
{
	Q_OBJECT
public:
	CRoom3DItemMgr(void);
	~CRoom3DItemMgr(void);
	void InitItemMgr();
	void LoadModels();
	QVector<ITEM_CATEGORY_INFO*>& GetItemCategoryInfos() {return m_CategoryInfos;};
public:
	bool		Search(ITEM_SEARCH_INFO* a_pSearchInfo);
	CTexturePool* GetTexturePool() { return m_pTexturePool; }
	CRoom3DItemDownloadMgr* GetItemDownloadMgr(){return m_pItemDownloadMgr;}
	ITEM_INFO* GetItemInfo(QString a_sProductID);

#if USE_THREAD
	bool IsModelLoading();
#endif
	bool DeleteItemInfo(QString a_sProductID, int a_nCategoryCode); // delete item
	bool ImportItem(QString a_sSrcFolderPath);
	int GetCategoryCode(QString& a_sCategoryName, bool& a_blNewAdded);

protected slots:
	void OnUpdateItemInfoData(QString a_ItemDataPath, bool error);
#if USE_THREAD
	void OnModelLoadFinish();
#endif
protected:
	bool OpenDatabase();
	bool CloseDatabase();

	// to read item info from database
	void UpdateItemInfos();
	bool UpdateCategoryInfos();
	bool UpdateCategoryInfo(ITEM_CATEGORY_INFO* a_pInfo);
	void ClearCategoryInfos();
	void ClearCategoryInfo(ITEM_CATEGORY_INFO* a_pCategoryInfo);
	void ClearModelInfos();
	void FillDataToCategoryInfo(QSqlQuery& a_q, ITEM_CATEGORY_INFO* a_pCategoryInfo);

	ITEM_CATEGORY_INFO* FindCategoriInfo(int a_nCategoryCode);

	// to update database
	void UpdateAllDatabase();
	void UpdateDatabaseFromCategoryFolder(QString& a_sCategoryFolderPath);
	bool UpdateDatabase(QString& a_sItemDirPath, bool& a_blInserted, QString& a_sProductID);
	bool UpdateItemInfoFromDB(ITEM_INFO* a_pInfo, QString a_sProductID);
	bool UpdateItemInfoFromDB(QSqlQuery& a_q, ITEM_INFO* a_pInfo);

	bool UpdateItemRecord(ITEM_INFO_FOR_DB_UPDATE* a_pItemInfo, bool& a_blInserted);
	bool IsAvailableItemInfo(ITEM_INFO_FOR_DB_UPDATE* a_pItemInfo);

	void ResetActions();
	void SetItemsActions(ITEM_CATEGORY_INFO* a_pCategoryInfo, ACTION_TYPE a_actionType);
	ITEM_INFO_FOR_UI* GetItemInfoForUI(ITEM_CATEGORY_INFO* a_pCategoryInfo, QString a_sProductID);
protected:
	QMap<QString, int>					m_categoryMap;
	QMap<QString, ITEM_INFO*>			m_ItemMap;

	QVector<CModel*> m_Models;
	QVector<ITEM_INFO*> m_vItems;
	QVector<ITEM_CATEGORY_INFO*> m_CategoryInfos;
	CTexturePool* m_pTexturePool;
	CRoom3DItemDownloadMgr* m_pItemDownloadMgr;
	// for downloading items from server

#if USE_THREAD
	CRoom3DModelLoadThread* m_pModelLoadThread;
#endif
};

