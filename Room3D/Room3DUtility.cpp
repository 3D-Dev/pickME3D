#include "Room3DUtility.h"
#include "jhead.h"
#include <math.h>
#include <QColor>
#include <QMessageBox>
#include "Room3DApplication.h"
#include "unzip.h"
#include <QDomDocument>
#include "Room3DItemMgr.h"

int getBrightValue(int nVal, int a_nBrightness)
{
	int nOffset = 255 * (a_nBrightness - 50) / 50;
	return min(max(nVal + nOffset, 0), 255);
}

// brighten the image by adding nOffset to every pixel
void BrightenImage(QImage* pImg, int a_nBrightness)
{
	int nWidth = pImg->width();
	int nHeight = pImg->height();
	a_nBrightness = min(max(a_nBrightness, MIN_BRIGHTNESS), MAX_BRIGHTNESS);

	for( int i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			QRgb value = pImg->pixel(j,i);
			int nRed = qRed(value);
			int nGreen = qGreen(value);
			int nBlue = qBlue(value);
			int nAlpha = qAlpha(value);

			nRed = getBrightValue(nRed, a_nBrightness);
			nGreen = getBrightValue(nGreen, a_nBrightness);
			nBlue = getBrightValue(nBlue, a_nBrightness);

			QRgb newValue = qRgba(nRed, nGreen, nBlue, nAlpha);
			pImg->setPixel(j, i, newValue);
		}
	}
}

int getConstrastValue(int nVal, int a_nContrast, int a_nBaseColor = 128)
{
	a_nBaseColor = min(max(0, a_nBaseColor), 255);
	nVal = (nVal - a_nBaseColor) * a_nContrast * 2 / MAX_CONTRAST + a_nBaseColor;
	return min(max(nVal, 0), 255);
}

// constrast the image by expanding color value between nMin and nMax
void ContrastImage(QImage* pImg, int a_nContrast)
{
	int nWidth = pImg->width();
	int nHeight = pImg->height();
	a_nContrast = min(max(a_nContrast, MIN_CONTRAST), MAX_CONTRAST);
	//QImage newImage(nWidth, nHeight, QImage::FormatRGB888);
	for( int i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			QRgb value = pImg->pixel(j,i);
			int nRed = qRed(value);
			int nGreen = qGreen(value);
			int nBlue = qBlue(value);
			int nAlpha = qAlpha(value);
			nRed = getConstrastValue(nRed, a_nContrast);
			nGreen = getConstrastValue(nGreen, a_nContrast);
			nBlue = getConstrastValue(nBlue, a_nContrast);
			QRgb newValue = qRgba(nRed, nGreen, nBlue, nAlpha);
			pImg->setPixel(j, i, newValue);
		}
	}
}

// constrast the image by expanding color value between nMin and nMax
void PressedImage(QImage* pImg, int a_nBrightness, int nMargin)
{
	int nWidth = pImg->width();
	int nHeight = pImg->height();

	for( int i = 0; i < nHeight; i++ )
	{
		for( int j = 0; j < nWidth; j++ )
		{
			QRgb value = pImg->pixel(j, i);
			int nRed = qRed(value);
			int nGreen = qGreen(value);
			int nBlue = qBlue(value);
			int nAlpha = qAlpha(value);
			
			if(i < nMargin || j < nMargin)
			{
				nRed = getBrightValue(nRed, a_nBrightness);
				nGreen = getBrightValue(nGreen, a_nBrightness);
				nBlue = getBrightValue(nBlue, a_nBrightness);
			}
			/*else if( i >= nHeight - nMargin - 1 || j >= nWidth - nMargin - 1)
			{
				nRed = getBrightValue(nRed, nOffset);
				nGreen = getBrightValue(nGreen, nOffset);
				nBlue = getBrightValue(nBlue, nOffset);
			}*/
			QRgb newValue = qRgba(nRed, nGreen, nBlue, nAlpha);
			pImg->setPixel(j, i, newValue);
		}
	}
}


void MakeButtonStateImg(QImage* a_pImg, BUTTON_STATE a_btnState)
{
	int nWidth = a_pImg->width();
	int nHeight = a_pImg->height();
	
	switch(a_btnState)
	{
	case BTN_STATE_DISABLE:
		ContrastImage(a_pImg, 80);
		break;
	case BTN_STATE_NORMAL:
		break;
	case BTN_STATE_PRESSED:
		PressedImage(a_pImg, 42, 5);
		break;
	case BTN_STATE_FOCUS:
		BrightenImage(a_pImg, 80);
		break;
	}
}

float ProcessFile(const char * FileName)
{
	int Modified = FALSE;
	ReadMode_t ReadMode;

	if (strlen(FileName) >= PATH_MAX-1){
		// Protect against buffer overruns in strcpy / strcat's on filename
		ErrFatal("filename too long");
	}

	ReadMode = READ_METADATA;

	ResetJpgfile();

	// Start with an empty image information structure.
	memset(&ImageInfo, 0, sizeof(ImageInfo));
	ImageInfo.FlashUsed = -1;
	ImageInfo.MeteringMode = -1;
	ImageInfo.Whitebalance = -1;

	float fFovy = 0.0f;

	strncpy(ImageInfo.FileName, FileName, PATH_MAX);

	if (!ReadJpegFile(FileName, READ_METADATA)) 
		return fFovy;
	float fLength = ImageInfo.FocalLength;
	float fWidth = ImageInfo.CCDWidth;
	
	if(fWidth == 0.0f)
		fWidth = 5.7f;	
	if(fWidth != 0.f && fLength != 0)
		fFovy = 2 * atan(fWidth / (2 * fLength));
	fFovy = fFovy * 180 / 3.14159264;
	return fFovy;
}

float GetFovy(QString a_strPath)
{
	float retVal = 0.0f;
	char *path = new char[1024];
	memset(path, 0, 1024);
	memcpy(path, a_strPath.toUtf8().data(), a_strPath.toUtf8().length());
	SlashToNative(path);
	retVal = ProcessFile(path);
	delete path;
	return retVal;
}

void ShowFunctionImplementMsgBox()
{
	QMessageBox msg(QMessageBox::Information, QString("Information"), MSG_NOT_IMPLEMENTED, QMessageBox::Ok, g_pApp->GetMainWidget());
	msg.exec();
}

bool GetItemInfoFromXML(QString a_sItemFolerPath, ITEM_INFO_EX* a_pItemInfo)
{
	// Update items's record from item folder
	QString itemInfoXMLPath = a_sItemFolerPath + QDir::separator() + ITEM_INFO_XML_FILE_NAME;
	if(QFile::exists(itemInfoXMLPath))
	{
		QDomDocument doc;
		QFile file(itemInfoXMLPath);
		if (!file.open(QIODevice::ReadOnly))
			return false;
		if (!doc.setContent(&file)) {
			file.close();
			return false;
		}
		file.close();
		QDomElement rootDocElem = doc.documentElement();
		QDomNode childNode = rootDocElem.firstChild();
		while(!childNode.isNull()) {
			QDomElement childElem = childNode.toElement(); // try to convert the node to an element.
			if(!childElem.isNull()) {
				FillDataToItemInfo(childElem, a_pItemInfo);
			}
			childNode = childNode.nextSibling();
		}
		return true;
	}
	else
		return false;
}

void FillDataToItemInfo(QDomElement& a_DomElem, ITEM_INFO_FOR_DB_UPDATE* a_pItemInfo)
{
	QString strName = a_DomElem.tagName();
	bool blConvertOK;
	CRoom3DItemMgr* pItemMgr = g_pApp->GetDocument()->GetItemMgr();
	if(strName.compare(XML_TAG_PRODUCT_NAME, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_sName = a_DomElem.text();
	}
	else if(strName.compare(XML_TAG_PRODUCT_CATEGORY1, Qt::CaseInsensitive) == 0)
	{
		bool bNewAdded;
		a_pItemInfo->m_nCategoryCode1 = pItemMgr->GetCategoryCode(a_DomElem.text(), bNewAdded);
	}
	else if(strName.compare(XML_TAG_PRODUCT_CATEGORY2, Qt::CaseInsensitive) == 0)
	{
		bool bNewAdded;
		a_pItemInfo->m_nCategoryCode2 = pItemMgr->GetCategoryCode(a_DomElem.text(), bNewAdded);
	}
	else if(strName.compare(XML_TAG_PRODUCT_CATEGORY3, Qt::CaseInsensitive) == 0)
	{
		bool bNewAdded;
		a_pItemInfo->m_nCategoryCode3 = pItemMgr->GetCategoryCode(a_DomElem.text(), bNewAdded);
	}
	else if(strName.compare(XML_TAG_PRODUCT_ID, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_sProductID = a_DomElem.text();
	}
	else if(strName.compare(XML_TAG_PRODUCT_DESC, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_sDesc = a_DomElem.text();
	}
	else if(strName.compare(XML_TAG_PRODUCT_PRICE, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_fPrice = a_DomElem.text().toFloat(&blConvertOK);
	}
	else if(strName.compare(XML_TAG_PRODUCT_PRICE_NOTE, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_sPriceNote = a_DomElem.text();
	}
	else if(strName.compare(XML_TAG_PRODUCT_WIDTH, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_fWidth = a_DomElem.text().toFloat(&blConvertOK);
	}
	else if(strName.compare(XML_TAG_PRODUCT_LENGTH, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_fLength = a_DomElem.text().toFloat(&blConvertOK);
	}
	else if(strName.compare(XML_TAG_PRODUCT_HEIGHT, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_fHeight = a_DomElem.text().toFloat(&blConvertOK);
	}
	else if(strName.compare(XML_TAG_PRODUCT_UPDATE_YEAR, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_sUpdateDate = AdjustDateString(a_DomElem.text(), 0);
	}
	else if(strName.compare(XML_TAG_PRODUCT_UPDATE_MONTH, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_sUpdateDate = a_pItemInfo->m_sUpdateDate + "/" + AdjustDateString(a_DomElem.text(), 1);
	}
	else if(strName.compare(XML_TAG_PRODUCT_UPDATE_DAY, Qt::CaseInsensitive) == 0)
	{
		a_pItemInfo->m_sUpdateDate = a_pItemInfo->m_sUpdateDate + "/" + AdjustDateString(a_DomElem.text(), 2);
	}
	else
	{
		qDebug("[CRoom3DItemMgr::FillDataToItemInfo] error! \n");
	}
}

void GetExtractFolderPath(ITEM_INFO_EX* a_pItemInfo, QString& a_sPath)
{
	// Try to find a folder path that contains already a_pItemInfo
	QString sDestPath("");
	if(a_pItemInfo)
	{
		// update database from folder structure.
		QString sItemRootPath = g_pApp->GetResourcePath(ITEM_ROOT_PATH);
		QDir rootDir(sItemRootPath);
		QStringList sCategoryFolders = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		QStringList::const_iterator it = sCategoryFolders.constBegin();
		// iterate category path
		while (it != sCategoryFolders.constEnd())
		{
			QString sCategoryFolderPath = (*it);
			sCategoryFolderPath = sItemRootPath + QString(QDir::separator()) + sCategoryFolderPath;

			rootDir.setPath(sCategoryFolderPath);
			QStringList sItemPaths = rootDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
			QStringList::const_iterator itItem = sItemPaths.constBegin();
			// iterate item path
			while (itItem != sItemPaths.constEnd())
			{
				QString sItemPath = (*itItem);
				sItemPath = sCategoryFolderPath + QDir::separator() + sItemPath;
				ITEM_INFO tempInfo;
				if(GetItemInfoFromXML(sItemPath, &tempInfo))
				{
					if(tempInfo.m_sProductID.compare(a_pItemInfo->m_sProductID) == 0)
					{
						QDir dstDir(sItemPath);
						dstDir.cdUp();
						sDestPath = dstDir.absolutePath();
						goto NEXT_LBL;
					}
				}
				itItem++;
			}
			it++;
		}
	}
NEXT_LBL:
	if(!sDestPath.isEmpty())
	{
		a_sPath = sDestPath;
		return;
	}
	QString sCategoryPath = "default";
// 	if(a_pItemInfo != NULL && !a_pItemInfo->m_sCategoryName.isEmpty())
// 		sCategoryPath = a_pItemInfo->m_sCategoryName;

	QString sRootPath= g_pApp->GetResourcePath("Items");
	QDir root(sRootPath);
	if(false == root.exists(sCategoryPath))
	{
		root.mkdir(sCategoryPath);
	}
	sRootPath = sRootPath + QDir::separator() + sCategoryPath;
	root.setPath(sRootPath);
	QString subFolderName;
	int nNum = 0;
	subFolderName = sCategoryPath + "_" + QString().setNum(nNum);
	while(root.exists(subFolderName))
	{
		subFolderName = sCategoryPath + "_" + QString().setNum(nNum);
		nNum++;
	}
	root.mkdir(subFolderName);
	sRootPath = sRootPath + QDir::separator() + subFolderName;
	a_sPath = sRootPath;
}

bool UnZipArchive(char* a_pBuffer, int a_nLength, QString& a_sDestRootPath)
{
	QChar* sPath = a_sDestRootPath.data();

	ZIPENTRY ze; 
	HZIP hz = OpenZip(a_pBuffer, a_nLength, NULL);
	SetUnzipBaseDir(hz, (WCHAR*)sPath);
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int zi=0; zi<numitems; zi++)
	{ 
		GetZipItem(hz,zi,&ze);
		UnzipItem(hz,zi,ze.name);
	}
	CloseZip(hz);
	return numitems > 0 ? true : false;
}

bool UnZipArchive(QString a_srcFilePath, QString a_sDestRootPath, QString& a_sFirstDirPath)
{
	QChar* sDstPath = a_sDestRootPath.data();
	QChar* sSrcPath = a_srcFilePath.data();
	a_sFirstDirPath = a_sDestRootPath;

	ZIPENTRY ze; 
	HZIP hz = OpenZip((WCHAR*)sSrcPath, NULL);
	SetUnzipBaseDir(hz, (WCHAR*)sDstPath);
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	bool blFindDir = false;
	for (int zi=0; zi<numitems; zi++)
	{ 
		GetZipItem(hz,zi,&ze);
		if(blFindDir == false)
		{
			QString sName = QString::fromUtf16(ze.name);
			int nPos = sName.indexOf("/");
			if(nPos > 0)
			{
				sName = sName.left(nPos);
				a_sFirstDirPath = a_sFirstDirPath + "/" + sName;
				blFindDir = true;
			}
		}
		UnzipItem(hz,zi,ze.name);
	}
	CloseZip(hz);
	return numitems > 0 ? true : false;
}

bool signCheck(const QString& a_sFilePath)
{
	const char* mimicode = "PickmE3D";
	int mimicodeLen = strlen( mimicode );
	bool result = false;
	QFile* checkfile = new QFile( a_sFilePath );
	char data[MAX_PATH] = {0,};
	if ( checkfile->open( QIODevice::ReadOnly ) )
	{
		qint64 fileSize = checkfile->size();
		checkfile->seek( fileSize - mimicodeLen );
		checkfile->read( data, mimicodeLen);
		if ( strcmp( data, mimicode ) == 0 )
			result = true;
		else
			result = false;
	}
	else
		result = false;
	return result;
}

// a_nType: 0 -> yeare, 1 -> month, 2 -> day
QString AdjustDateString(QString a_sDate, int a_nType)
{
	QString sDate = a_sDate;
	int nCount = sDate.lastIndexOf(".");
	sDate = sDate.left(nCount);
	switch(a_nType)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	}
	return sDate;
}



bool CopyDirectory(QString a_sSrcDirPath, QString a_sDestDirpath)
{
	QDir destDir(a_sDestDirpath);
	if(destDir.exists() == false)
		destDir.mkpath(a_sDestDirpath);
	QDir srcDir(a_sSrcDirPath);
	QString sDestPath = a_sDestDirpath;

	QFileInfoList sItemInfoList = srcDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList::iterator it = sItemInfoList.begin();
	while(it != sItemInfoList.end())
	{
		QFileInfo fileInfo = (*it);
		if(fileInfo.isDir())
		{
			sDestPath = sDestPath + QDir::separator() + fileInfo.fileName();
			CopyDirectory(fileInfo.absoluteFilePath(), sDestPath);
		}
		else if(fileInfo.isFile())
		{
			QString sDestFilePath = sDestPath + QDir::separator() + fileInfo.fileName();
			QString sSrcFilePath = fileInfo.absoluteFilePath();
			if(QFile::exists(sDestFilePath))
			{
				QFile::remove(sDestFilePath);
			}
			QFile::copy(sSrcFilePath, sDestFilePath);
		}
		it++;
	}

	return true;
}

void DeleteDirectory(QString a_FolderPath)
{
	QDir srcDir(a_FolderPath);
	if(srcDir.exists() == false)
		return;
	QFileInfoList sItemInfoList = srcDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	QFileInfoList::iterator it = sItemInfoList.begin();
	while(it != sItemInfoList.end())
	{
		QFileInfo fileInfo = (*it);
		if(fileInfo.isDir())
		{
			DeleteDirectory(fileInfo.absoluteFilePath());
		}
		else if(fileInfo.isFile())
		{
			QString sSrcFilePath = fileInfo.absoluteFilePath();
			if(QFile::exists(sSrcFilePath))
			{
				QFile::remove(sSrcFilePath);
			}
		}
		it++;
	}
	QString sDirName = srcDir.dirName();
	srcDir.cdUp();
	srcDir.rmdir(sDirName);


}


bool IsAvailableItemDir(QString a_sItemDirPath)
{
	QString itemInfoXMLPath = a_sItemDirPath + QDir::separator() + ITEM_INFO_XML_FILE_NAME;
	if(QFile::exists(itemInfoXMLPath))
	{
		return true;
	}
	return false;
}

bool IsAvailbaleItemZipFile(QString a_sItemFilePath)
{
	QChar* sPath = a_sItemFilePath.data();

	ZIPENTRY ze; 
	HZIP hz = OpenZip((WCHAR*)sPath, NULL);
	//SetUnzipBaseDir(hz, (WCHAR*)sPath);
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int zi=0; zi<numitems; zi++)
	{ 
		GetZipItem(hz,zi,&ze);
		QString sItemName = QString::fromUtf16(ze.name);
		if(sItemName.compare(QString(ITEM_INFO_XML_FILE_NAME)) == 0)
			return true;
	}
	CloseZip(hz);
	return false;
}


void SetStepLabelPalette(QWidget* a_pWidget)
{
	QPalette curPalette = a_pWidget->palette();
	curPalette.setColor(QPalette::WindowText, STEPBAR_COMMENT_COLOR);
	a_pWidget->setPalette(curPalette);
}

void SetDefaultLineEditPalette(QWidget* a_pWidget)
{
	QPalette curPalette = a_pWidget->palette();
	// curPalette.setColor(QPalette::WindowText, QColor(255,255,255));
	curPalette.setColor(QPalette::Base, QColor(255,252,227));
	a_pWidget->setPalette(curPalette);

}

void SetDefaultPalette(QWidget* a_pWidget)
{
	QPalette curPalette = a_pWidget->palette();
	curPalette.setColor(QPalette::WindowText, QColor(255,255,255));
	curPalette.setColor(QPalette::Button, QColor(13,15,11));
	curPalette.setColor(QPalette::Light, QColor(19,22,16));
	curPalette.setColor(QPalette::Midlight, QColor(16,18,13));
	curPalette.setColor(QPalette::Dark, QColor(7,0,5));
	curPalette.setColor(QPalette::Mid, QColor(8,10,7));
	curPalette.setColor(QPalette::Text, QColor(255,255,255));
	curPalette.setColor(QPalette::BrightText, QColor(255,255,255));
	curPalette.setColor(QPalette::ButtonText, QColor(255,255,255));
	curPalette.setColor(QPalette::Base, QColor(12,14,10, 0));
	curPalette.setColor(QPalette::Window, QColor(13,15,11));
	curPalette.setColor(QPalette::Shadow, QColor(0,0,0));
	curPalette.setColor(QPalette::Highlight, QColor(49, 106, 197));
	curPalette.setColor(QPalette::HighlightedText, QColor(255,255,255));
	curPalette.setColor(QPalette::Link, QColor(0,0,255));
	curPalette.setColor(QPalette::LinkVisited, QColor(255,0,255));
	curPalette.setColor(QPalette::AlternateBase, QColor(6,7,5));
	curPalette.setColor(QPalette::NoRole, QColor(0,0,0));
	curPalette.setColor(QPalette::ToolTipBase, QColor(255,255,255));
	curPalette.setColor(QPalette::ToolTipText, QColor(0,0,0));

	a_pWidget->setPalette(curPalette);

}


void UpdateSearchInfoFromComboIndex(int a_nComboIndex , ITEM_SEARCH_INFO* a_pSearchInfo)
{
	a_pSearchInfo->m_nPriceComboIndex = a_nComboIndex;
	switch(a_nComboIndex)
	{
	case 1:
		a_pSearchInfo->m_fFromPrice = 0.f;
		a_pSearchInfo->m_fToPrice = 500.f;
		break;
	case 2:
		a_pSearchInfo->m_fFromPrice = 501.f;
		a_pSearchInfo->m_fToPrice = 3000.f;
		break;
	case 3:
		a_pSearchInfo->m_fFromPrice = 3001.f;
		a_pSearchInfo->m_fToPrice = 10000;
		break;
	case 4:
		a_pSearchInfo->m_fFromPrice = 10001.f;
		a_pSearchInfo->m_fToPrice = 50000.f;
		break;
	case 5:
		a_pSearchInfo->m_fFromPrice = 50001.f;
		a_pSearchInfo->m_fToPrice = 0xFFFFFFFF;
		break;
	default:
		a_pSearchInfo->m_fFromPrice = 0.f;
		a_pSearchInfo->m_fToPrice = 0xFFFFFFFF;
		break;
	}
}