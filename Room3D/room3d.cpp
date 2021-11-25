#include "room3d.h"
#include "Common.h"
#include "Define.h"
#include "qmainwindow.h"
#include "Room3DApplication.h"
#include <QPainter>

#define TITLEBAR_BTN_SIZE	QSize(20,20)

Room3D::Room3D(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	g_pApp->SetMainWidget(this);
 	QApplication::setStyle(new NorwegianWoodStyle);
	//ui.setupUi(this);
	InitControls();
	//resize(BASE_WIDTH,BASE_HEIGHT);
	m_pLogoDlg = new CRoom3DLogoDlg(NULL, IMG_SPLASH);
	m_pLogoDlg->finish(this);
	m_pIntoroVideoDlg = new CRoom3DIntroVideoDlg(NULL);
	m_pGuideDlg = new CRoom3DGuideDlg(this);
	QString sTutorialPath = g_pApp->GetResourcePath(STEP1_TUTORIAL_VIDEO_PATH);
	m_pStep1TutorialDlg = new CRoom3DTutorialVideoDlg(sTutorialPath, this);
	sTutorialPath = g_pApp->GetResourcePath(STEP2_TUTORIAL_VIDEO_PATH);
	m_pStep2TutorialDlg = new CRoom3DTutorialVideoDlg(sTutorialPath, this);
	sTutorialPath = g_pApp->GetResourcePath(STEP3_TUTORIAL_VIDEO_PATH);
	m_pStep3TutorialDlg = new CRoom3DTutorialVideoDlg(sTutorialPath, this);
#if USE_THREAD
	m_pWaitingDlg = new CRoom3DWaitingDlg(this);
	m_pWaitingDlg->hide();
#endif
	m_pGuidedWidget = NULL;
}

Room3D::~Room3D()
{
	SAFE_DELETE(m_pMenuBar);
	SAFE_DELETE(m_pStepBar);
	SAFE_DELETE(m_pToolBoxBar);
	SAFE_DELETE(m_pSceneView);
	SAFE_DELETE(m_pCentralWidget);// never free any child of m_pCentralWidget
	SAFE_DELETE(m_pLogoDlg);
	SAFE_DELETE(m_pIntoroVideoDlg);
	SAFE_DELETE(m_pGuideDlg);
	SAFE_DELETE(m_pStep1TutorialDlg);
	SAFE_DELETE(m_pStep2TutorialDlg);
	SAFE_DELETE(m_pStep3TutorialDlg);
#if USE_THREAD
	SAFE_DELETE(m_pWaitingDlg);
#endif
}

void Room3D::InitControls()
{
	this->resize(1024, 700);
	m_pCentralWidget = new CRoom3DImageWidget("", "", 10, true, this);
	m_pCentralWidget->SetBkColor(BK_COLOR_VIEWWRAPPER_START, BK_COLOR_VIEWWRAPPER_END);

	gridLayout = new QGridLayout(m_pCentralWidget);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);

	// set title bar
	m_pTitleBarHorzLayout = new QHBoxLayout();
	m_pTitleBarHorzLayout->setSpacing(0);
	m_pTitleBarHorzLayout->setContentsMargins(0, 0, 0, 0);
	m_pTitleBarLeft = new CRoom3DImageWidget(BACK_IMG_TITLEBAR_LEFT, "", 1, true, m_pCentralWidget);
	m_pTitleBarLeft->setMaximumWidth(TITLE_BAR_LEFT_WIDTH);
	m_pTitleBarLeft->setMinimumWidth(TITLE_BAR_LEFT_WIDTH);
	m_pTitleBarLeft->setMinimumHeight(TITLE_BAR_HEIGHT);

	m_pTitleBarMid = new CRoom3DImageWidget(BACK_IMG_TITLEBAR_MID, "", 1, false, m_pCentralWidget);
	m_pTitleBarMidHorzLayout = new QHBoxLayout(m_pTitleBarMid);
	m_pTitleBarMidHorzLayout->setSpacing(0);
	m_pTitleBarMidHorzLayout->setContentsMargins(0, 0, 0, 0);
	m_pTitlePickMe3D = new CRoom3DImageWidget(IMG_PICKME_3D, "", 1, true, m_pTitleBarMid);
	m_pTitlePickMe3D->setMaximumWidth(TITLE_BAR_PICKME_3D_WIDTH);
	m_pTitlePickMe3D->setMinimumWidth(TITLE_BAR_PICKME_3D_WIDTH);
	m_pTitleBarMidHorzLayout->addWidget(m_pTitlePickMe3D, Qt::AlignHCenter);

	m_pTitleBarRight = new CRoom3DImageWidget(BACK_IMG_TITLEBAR_RIGHT, "", 1, true, m_pCentralWidget);
	m_pTitleBarRight->setMaximumWidth(TITLE_BAR_RIGHT_WIDTH);
	m_pTitleBarRight->setMinimumWidth(TITLE_BAR_RIGHT_WIDTH);
	m_pTitleBarRightHorzLayout = new QHBoxLayout(m_pTitleBarRight);
	m_pTitleBarRightHorzLayout->setSpacing(6);
	m_pTitleBarRightHorzLayout->setContentsMargins(0, 0, 0, 0);
	m_pMinimizeBtn = new CRoom3DImageButton(BUTTON_IMG_MINIMIZE, BTN_MINIMIZE, Qt::PointingHandCursor, this);
	m_pCloseBtn = new CRoom3DImageButton(BUTTON_IMG_CLOSE, BTN_CLOSE, Qt::PointingHandCursor, this);
	m_pTitleBarRightHorzLayout->addSpacing(30);
	m_pTitleBarRightHorzLayout->addWidget(m_pMinimizeBtn);
	m_pTitleBarRightHorzLayout->addWidget(m_pCloseBtn);
	m_pTitleBarRightHorzLayout->addStretch();

	m_pTitleBarHorzLayout->addWidget(m_pTitleBarLeft);
	m_pTitleBarHorzLayout->addWidget(m_pTitleBarMid);
	m_pTitleBarHorzLayout->addWidget(m_pTitleBarRight);

	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSpacing(0);
	frame_2 = new CRoom3DImageWidget(m_pCentralWidget);
	// frame_2->SetBkColor(BK_COLOR_VIEWWRAPPER_START, BK_COLOR_VIEWWRAPPER_END);
	frame_2->setMinimumSize(QSize(500, 100));
	frame_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	verticalLayout = new QVBoxLayout(frame_2);
	verticalLayout->setSpacing(0);
	verticalLayout->setContentsMargins(0, 0, 0, 0);

	verticalLayout1 = new QVBoxLayout();
	verticalLayout1->setSpacing(0);
	verticalLayout1->setContentsMargins(20, 0, 20, 0);
	// Set Step Image
	m_pStepBar = new CRoom3DStepBar(m_pCentralWidget);
	m_pStepBar->setFixedHeight(STEPBAR_HEIGHT);
	// Set view window.
	m_pScenViewWrapper = new CRoom3DSceneViewWrapper(frame_2);
	m_pSceneView = new CRoom3DSceneView(m_pScenViewWrapper);

	verticalLayout1->addWidget(m_pStepBar);
	verticalLayout1->addWidget(m_pScenViewWrapper);

	
	m_pMenuBar = new CRoom3DMenuBar(frame_2);
	m_pMenuBar->setFixedHeight(MENUBAR_HEIGHT);

	verticalLayout->addLayout(verticalLayout1);
	verticalLayout->addWidget(m_pMenuBar);

	m_pToolBoxBar = new CRoom3DToolBoxBar(m_pCentralWidget);
	m_pToolBoxBar->setFixedWidth(TOOLBOX_WIDTH);

	horizontalLayout->addWidget(frame_2);
	horizontalLayout->addWidget(m_pToolBoxBar);



	// bottom edge
	m_pBottomEdge = new CRoom3DImageWidget(BACK_IMG_BOTTEOM_EDGE, "", 1, false, m_pCentralWidget);
	m_pBottomEdge->setFixedHeight(BOTTOM_EDGE_HEIGHT);

	gridLayout->addLayout(m_pTitleBarHorzLayout, 0, 0, 1, 1);
	gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);
	gridLayout->addWidget(m_pBottomEdge, 2, 0, 1, 1);

	this->setCentralWidget(m_pCentralWidget);

	ui.retranslateUi(this);

	QMetaObject::connectSlotsByName(this);

	connect(m_pMinimizeBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pCloseBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));

}

void Room3D::UpdateControlState(void* a_pUserData)
{
	m_pMenuBar->UpdateControlState();
	m_pStepBar->UpdateControlState();
	m_pToolBoxBar->UpdateControlState(a_pUserData);
	m_pSceneView->updateGL();
	UpdateGuideWidget();
}
void Room3D::UpdateView()
{
	if(m_pSceneView)
	{
		m_pSceneView->updateGL();
	}
}

void Room3D::ShowLogoDlg(bool a_blShow)
{
	if(a_blShow)
	{
		m_pLogoDlg->show();
		m_pLogoDlg->setCursor(QCursor(Qt::WaitCursor));
	}
	else
		m_pLogoDlg->hide();
}

void Room3D::ShowGuideDlg(int a_nId, QPoint a_offsetPt)
{
	m_nGuideId = a_nId;
	m_offsetPt = a_offsetPt;
}

void Room3D::resizeEvent(QResizeEvent *event)
{
	UpdateGuideWidget();
	QWidget::resizeEvent(event);
}

void Room3D::UpdateGuideWidget()
{
	switch ( m_nGuideId )
	{
	case STEP_NONE:
		m_pGuideDlg->hide();
		break;
	default:
		{
			QRect buttonRect;
			QPoint buttonPnt;
			if(m_pGuidedWidget && m_pGuidedWidget->parentWidget())
			{
				buttonRect = m_pGuidedWidget->geometry();
				buttonPnt = m_pGuidedWidget->parentWidget()->mapToGlobal(QPoint(buttonRect.left() , buttonRect.center().y()));
				QSize size = m_pGuideDlg->size();
				buttonPnt -= QPoint(size.width(),size.height() / 2);
				buttonPnt += m_offsetPt;
				buttonPnt = this->mapFromGlobal(buttonPnt);
				m_pGuideDlg->move(buttonPnt);
				m_pGuideDlg->show();
			}
		}
	}
}

void Room3D::ShowTutorialVideo()
{
	switch(g_pApp->GetAppStep())
	{
	case APP_EDIT_STEP_1:
		m_pStep1TutorialDlg->Play();
		break;
	case APP_EDIT_STEP_2:
		m_pStep2TutorialDlg->Play();
		break;
	case APP_EDIT_STEP_3:
		m_pStep3TutorialDlg->Play();
		break;
	}
}

void Room3D::HideTutorialVideo()
{
	m_pStep1TutorialDlg->hide();
	m_pStep2TutorialDlg->hide();
	m_pStep3TutorialDlg->hide();
}

#if USE_THREAD
void Room3D::ShowWaitingDlg()
{
	CRoom3DItemMgr* pItemMgr = g_pApp->GetDocument()->GetItemMgr();

	if(pItemMgr == NULL)
		return;

	if(pItemMgr->IsModelLoading())
	{
		if(m_pWaitingDlg)
		{
			m_pWaitingDlg->exec();
		}
	}
}

void Room3D::OnModelLoadFinish()
{
	g_pApp->GetDocument()->Get3DSceneMgr()->UpdateFurnitures();
	if(m_pWaitingDlg && m_pWaitingDlg->isVisible())
	{
		m_pWaitingDlg->accept();
	}
}
#endif


void Room3D::pressedButton(int a_nBtnID)
{
	APP_STEP step;
	switch(a_nBtnID)
	{
	case BTN_MINIMIZE:
		this->setWindowState(windowState() | Qt::WindowMinimized);
		break;
	case BTN_CLOSE:
		g_pApp->quit();
		break;
	//case BTN_LANG:
	//	{
	//		CRoom3DApplication::LANGUAGE_TYPE eLang = g_pApp->GetLanguage();
	//		if(eLang == CRoom3DApplication::Lang_Thailand)
	//			eLang = CRoom3DApplication::Lang_Eng;
	//		else
	//			eLang = CRoom3DApplication::Lang_Thailand;
	//		g_pApp->SetLanguage(eLang);
	//		UpdateControlState();
	//	}
	//	break;
	}
}