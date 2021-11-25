#include "Room3DApplication.h"
#include "Common.h"
#include "room3d.h"
#include "Room3DCommandMgr.h"

//#define THAI_LANG

CRoom3DApplication::CRoom3DApplication(int & argc, char ** argv) : m_Mutex(QMutex::Recursive), QApplication(argc, argv)
{
	g_pApp = this;
	m_workDir = applicationDirPath();
	m_workDir.replace("/", QDir::separator());
	m_appStep = APP_STARTING;
	m_pMainWidget = NULL;
	m_pDocument = new CRoom3DDocument();
	m_pCommandMgr = new CRoom3DCommandMgr();
	m_pSoundMgr = new CRoom3DSoundMgr();
	QString sQmPath = GetResourcePath("Room3DApp_en.qm");
	bool blRet = m_translatorEng.load(sQmPath);
	sQmPath = GetResourcePath("Room3DApp_th.qm");
	blRet = m_translatorThai.load(sQmPath);
	setStartDragDistance(1);
	setStartDragTime(0);
	connect(this, SIGNAL(aboutToQuit()), this, SLOT(quitAbout()));
#ifdef THAI_LANG
	m_nLangID = Lang_Thailand;
#else
	m_nLangID = Lang_Eng;
#endif
	SetLanguage(m_nLangID, true);
	//m_nLangID = Lang_Non;
	//if( true == ReadConfiguration() )
	//{
	//	SetLanguage(m_nLangID, true);
	//}
	//else
	//	SetLanguage(Lang_Thailand, true);
}

CRoom3DApplication::~CRoom3DApplication(void)
{
	SaveConfiguration();
	SAFE_DELETE(m_pDocument);
	SAFE_DELETE(m_pCommandMgr);
	SAFE_DELETE(m_pSoundMgr);
}

QString CRoom3DApplication::GetResourcePath(QString a_sRelative)
{
	QString sFullPath = m_workDir + QDir::separator() + RESOURCE_FOLDER_NAME + QDir::separator() + a_sRelative;
	return sFullPath;
}

void CRoom3DApplication::SetAppStep(APP_STEP a_appStep)
{
	if(m_appStep == a_appStep)
		return;
	m_appStep = a_appStep;
	COMMAND_ID cmdID = CMD_NULL;
	switch(a_appStep)
	{
	case APP_LOGO:
		{
			if(m_pMainWidget)
			{
				m_pMainWidget->ShowLogoDlg(true);
			}
			break;
		}
	case APP_INTRO_MOVIE:
		{
			if(m_pMainWidget)
			{
				m_pMainWidget->ShowLogoDlg(false);
			}
			CRoom3DIntroVideoDlg* pIntroVideoDlg = m_pMainWidget->GetIntroVideoDlg();
			if(pIntroVideoDlg)
			{
				QString sPath = GetResourcePath(INTORO_VIDEO_PATH);
				pIntroVideoDlg->Play(sPath, 50);
			}
			break;
		}
	case APP_EDIT_STEP_1:
		{
			if(m_pMainWidget && m_pMainWidget->isVisible() == false)
			{
#ifdef DEBUG
				m_pMainWidget->showFullScreen();
#else
				m_pMainWidget->showFullScreen();
#endif
			}
			m_pMainWidget->ShowGuideDlg(STEP_1_BROWSE);
		}
		cmdID = CMD_STEP1;
		break;
	case APP_EDIT_STEP_2:
		m_pMainWidget->ShowGuideDlg(STEP_2_SET_CORNER);
		cmdID = CMD_STEP2;
		break;
	case APP_EDIT_STEP_3:
		if(m_pMainWidget && m_pMainWidget->isVisible() == false)
		{
#ifdef DEBUG
			m_pMainWidget->show();
#else
			m_pMainWidget->showFullScreen();
#endif
		}		
#if USE_THREAD
		m_pMainWidget->ShowWaitingDlg();
#endif
		m_pMainWidget->ShowGuideDlg(STEP_NONE);
		cmdID = CMD_STEP3;
		break;
	}
	if(m_pCommandMgr)
		m_pCommandMgr->CreateCommand(cmdID);
	if(m_pMainWidget)
	{
		m_pMainWidget->HideTutorialVideo();
		m_pMainWidget->UpdateControlState();
	}
}

void CRoom3DApplication::Undo(bool a_blUndo)
{
	if(m_pDocument->Undo(a_blUndo))
	{
		m_pMainWidget->UpdateView();
		m_pMainWidget->UpdateControlState();
	}
}

void CRoom3DApplication::InitApplication()
{
	CRoom3DIntroVideoDlg* pIntoVideoDlg = GetMainWidget()->GetIntroVideoDlg();
	if(pIntoVideoDlg)
	{
		connect(pIntoVideoDlg, SIGNAL(OnFinished()), this, SLOT(OnMoveEidt1Step()));
	}
	m_pDocument->InitDocument();
}

void CRoom3DApplication::quit()
{
	// QTimer::singleShot(0, qApp, SLOT(quit()));
	QApplication::quit();
}

void CRoom3DApplication::quitAbout()
{

}

void CRoom3DApplication::OnMoveEidt1Step()
{
	SetAppStep(APP_EDIT_STEP_1);
}

void CRoom3DApplication::SetLanguage(LANGUAGE_TYPE a_eLangType, bool a_blUpdate)
{
	m_nLangID = a_eLangType;
	if(a_blUpdate)
	{
		removeTranslator(&m_translatorEng);
		removeTranslator(&m_translatorThai);
		if(m_nLangID == Lang_Thailand)
		{
			installTranslator(&m_translatorThai);
			QLocale myLocal(QLocale::Thai, QLocale::Thailand);
			QLocale::setDefault(myLocal);
		}
		else
		{
			installTranslator(&m_translatorEng);
			QLocale myLocal(QLocale::C, QLocale::AnyCountry);
			QLocale::setDefault(myLocal);
		}
	}
}

bool CRoom3DApplication::SaveConfiguration()
{
	QDomDocument doc;
	QString sFileName;
	sFileName = GetWorkDir() + QDir::separator() + STR_CONFIG_FILE_NAME;
	QFile file(sFileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return false;

	QDomElement elemConfig = doc.createElement("config");
	QDomElement elemLang = doc.createElement("Lang");
	QString sLang;
	sLang.setNum(m_nLangID);
	QDomText txtName = doc.createTextNode(sLang);
	elemLang.appendChild(txtName);
	elemConfig.appendChild(elemLang);
	doc.appendChild(elemConfig);

	QString strXml = doc.toString();
	file.write(strXml.toUtf8().data());
	file.close();
	return true;
}

bool CRoom3DApplication::ReadConfiguration()
{
	QString sFileName;
	sFileName = GetWorkDir() + QDir::separator() + STR_CONFIG_FILE_NAME;
	QDomDocument doc;
	QFile file(sFileName);
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
			QString strName = childElem.tagName();
			if(strName.compare("Lang", Qt::CaseInsensitive) == 0)
			{
				m_nLangID = (LANGUAGE_TYPE)childElem.text().toInt();
			}
		}
		childNode = childNode.nextSibling();
	}
	return true;
}