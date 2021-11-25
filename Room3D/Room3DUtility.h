//----------------------------------------------//
//	File: Room3DUtility.h								//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once

#include "Common.h"
#include "Define.h"
#include <QImage>
#include <QString>
#include "Room3DItemMgr.h"
struct ITEM_SEARCH_INFO;

void MakeButtonStateImg(QImage* a_pImg, BUTTON_STATE a_btnState);
float GetFovy(QString a_strPath);
void ShowFunctionImplementMsgBox();
void BrightenImage(QImage* pImg, int a_nBrightness);
void ContrastImage(QImage* pImg, int a_nContrast);

void GetExtractFolderPath(ITEM_INFO_EX* a_pItemInfo, QString& a_sPath);
bool UnZipArchive(char* a_pBuffer, int a_nLength, QString& a_sDestRootPath);
bool UnZipArchive(QString a_srcFilePath, QString a_sDestRootPath, QString& a_sFirstDirPath);
bool signCheck(const QString& a_sFilePath);
QString AdjustDateString(QString a_sDate, int a_nType);
bool CopyDirectory(QString a_sSrcDirPath, QString a_sDestDirpath);
void DeleteDirectory(QString a_FolderPath);
bool IsAvailableItemDir(QString a_sItemDirPath);
bool IsAvailbaleItemZipFile(QString a_sItemFilePath);
bool GetItemInfoFromXML(QString a_sItemFolerPath, ITEM_INFO_EX* a_pItemInfo);
void FillDataToItemInfo(QDomElement& a_DomElem, ITEM_INFO_FOR_DB_UPDATE* a_pItemInfo);

void SetDefaultPalette(QWidget* a_pWidget);
void SetDefaultLineEditPalette(QWidget* a_pWidget);
void SetStepLabelPalette(QWidget* a_pWidget);

void UpdateSearchInfoFromComboIndex(int a_nComboIndex , ITEM_SEARCH_INFO* a_pSearchInfo);
