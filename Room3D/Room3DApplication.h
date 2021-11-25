//----------------------------------------------//
//	File: Room3DApplication.h						//
//	Desc: 										//
//	Update: 2012.6  (by 001	)				//
//----------------------------------------------//
#pragma once

#include <QtGui/QApplication>
#include <QTranslator>
#include "Define.h"
#include "Room3DDocument.h"
#include "Room3DSceneView.h"
#include "Room3D.h"
#include "Room3DCommandMgr.h"
#include "Room3DCommandMgr.h"
#include "Room3DSoundMgr.h"
#include <QMutex>

/*!
	\class CRoom3DApplication
	\brief This is application class that contains all ui and data.(by 001, 2012/6/21).
	\sa 
*/
class CRoom3DApplication : public QApplication
{
	Q_OBJECT
public:
	enum LANGUAGE_TYPE
	{
		Lang_Non, 
		Lang_Eng, 
		Lang_Thailand
	};
public:
	CRoom3DApplication(int & argc, char ** argv);
	~CRoom3DApplication(void);
	QString& GetWorkDir()
	{ return m_workDir; }
	QString GetResourcePath(QString a_sRelative);
	
	void SetAppStep(APP_STEP a_appStep);
	APP_STEP GetAppStep(){return m_appStep;}

	void SetMainWidget(Room3D*	a_pMainWidget){m_pMainWidget = a_pMainWidget;};
	Room3D* GetMainWidget(){return m_pMainWidget;};

	CRoom3DDocument* GetDocument()
	{
		return m_pDocument;
	}
	CRoom3DSoundMgr*	GetSoundMgr(){return m_pSoundMgr;}
	CRoom3DCommandMgr* GetCommandMgr(){return m_pCommandMgr;};
	CRoom3DSceneView* GetSceneView(){return m_pMainWidget->GetSceneView();};
	void Undo(bool a_blUndo);
	void InitApplication();

	LANGUAGE_TYPE GetLanguage(){return m_nLangID;}
	void SetLanguage(LANGUAGE_TYPE a_eLangType, bool a_blUpdate = false);

	// thread lock/unlock
	void ThreadLock(){m_Mutex.lock();}
	void ThreadUnLock(){m_Mutex.unlock();}

protected:
	bool SaveConfiguration();
	bool ReadConfiguration();


public slots:
	void quit();
	void quitAbout();
protected slots:
	void OnMoveEidt1Step();

public: //protected:
	QString m_workDir;
	APP_STEP m_appStep;
	Room3D*	m_pMainWidget;
	CRoom3DDocument* m_pDocument;
	CRoom3DCommandMgr* m_pCommandMgr;
	CRoom3DSoundMgr*	m_pSoundMgr;
public:
	QTranslator m_translatorEng;
	QTranslator m_translatorThai;
	LANGUAGE_TYPE m_nLangID;
	QMutex			m_Mutex;
};
