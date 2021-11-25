#include "Room3DStep2ToolBox.h"
#include "Define.h"
#include "Common.h"
#include "Room3DApplication.h"
#include "Room3DUtility.h"
#include "Room3DSetScaleDlg.h"

#ifdef _SUPPORT_STEP2_TOOLBOX_CLASSIC

CRoom3DStep2ToolBox::CRoom3DStep2ToolBox(QWidget* parent) : CRoom3DBaseToolBox(parent)
{
	m_pSetFloorBtn = NULL;
	m_pSetScaleBtn = NULL;
	m_pSetFloorCheckBox = NULL;
	m_pSetScaleCheckBox = NULL;
	InitCotrols();
}

CRoom3DStep2ToolBox::~CRoom3DStep2ToolBox(void)
{
	SAFE_DELETE(m_pVerticalLayout);
	SAFE_DELETE(m_pSetFloorComment);
	SAFE_DELETE(m_pSetScaleComment);
}

void CRoom3DStep2ToolBox::InitCotrols()
{
	InitBkWidget();

	// Create Vertical Layout
	m_pVerticalLayout = new QVBoxLayout(m_pBkWidget);
	m_pVerticalLayout->setSpacing(6);
	m_pVerticalLayout->setContentsMargins(TOOLBOX_CONTENTS_MARGIN, TOOLBOX_CONTENTS_MARGIN, TOOLBOX_CONTENTS_MARGIN, TOOLBOX_CONTENTS_MARGIN);
	// Create SetFloor Horizontal Layout.
	m_pSetFloorHorizontalLayout = new QHBoxLayout();
	m_pSetFloorHorizontalLayout->setSpacing(10);
	m_pSetFloorHorizontalLayout->setContentsMargins(10, 8, 10, 0);
	// Create set floor button.
	m_pSetFloorBtn = new CRoom3DImageButton(BUTTON_IMG_SETFLOOR, BTN_SET_FLOOR, Qt::PointingHandCursor, m_pBkWidget);
	m_pSetFloorBtn->setMinimumSize(QSize(0, 30));

	// create commit button for set floor button.
	m_pSetFloorCheckBox = new QCheckBox(m_pBkWidget);
	m_pSetFloorCheckBox->setText(SET_COMMIT);
	m_pSetFloorCheckBox->setMaximumWidth(TOOLBOX2_COMMIT_BTN_WIDTH);
	// add set floor button to layout.
	m_pSetFloorHorizontalLayout->addWidget(m_pSetFloorBtn);
	m_pSetFloorHorizontalLayout->addWidget(m_pSetFloorCheckBox);

	m_pSeparator1 = new QFrame(m_pBkWidget);
	m_pSeparator1->setMinimumHeight(TOOLBOX1_SEPARATOR_HEIGHT);
	m_pSeparator1->setMaximumHeight(TOOLBOX1_SEPARATOR_HEIGHT);
	m_pSeparator1->setFrameShape(QFrame::HLine);
	m_pSeparator1->setFrameShadow(QFrame::Sunken);
	QPalette curPalette = m_pSeparator1->palette();
	curPalette.setColor(QPalette::Dark, TOOLBOX_SEPARATOR_COLOR);
	curPalette.setColor(QPalette::Light, TOOLBOX_SEPARATOR_COLOR);
	m_pSeparator1->setPalette(curPalette);


	m_pSetFloorComment = new QTextEdit(m_pBkWidget);
	m_pSetFloorComment->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_pSetFloorComment->setMaximumHeight(150);
	m_pSetFloorComment->setReadOnly(true);
	m_pSetFloorComment->setWordWrapMode(QTextOption::WrapAnywhere);
	m_pSetFloorComment->setTextInteractionFlags(Qt::NoTextInteraction);
	m_pSetFloorComment->setFrameShape(QFrame::NoFrame);

	// Create set scale layout.
	m_pSetScaleHorizontalLayout = new QHBoxLayout();
	m_pSetScaleHorizontalLayout->setSpacing(10);
	m_pSetScaleHorizontalLayout->setContentsMargins(10, 8, 10, 0);
	// Create set scale button.
	m_pSetScaleBtn = new CRoom3DImageButton(BUTTON_IMG_SETSCALE, BTN_SET_SCALE, Qt::PointingHandCursor, m_pBkWidget);
	m_pSetScaleBtn->setMinimumSize(QSize(0, 30));

	// Create commit check box for scale button.
	m_pSetScaleCheckBox = new QCheckBox(m_pBkWidget);
	m_pSetScaleCheckBox->setText(SET_COMMIT);
	m_pSetScaleCheckBox->setMaximumWidth(TOOLBOX2_COMMIT_BTN_WIDTH);

	m_pSetScaleHorizontalLayout->addWidget(m_pSetScaleBtn);
	m_pSetScaleHorizontalLayout->addWidget(m_pSetScaleCheckBox);


	m_pSetScaleComment = new QTextEdit(m_pBkWidget);
	m_pSetScaleComment->setReadOnly(true);
	m_pSetScaleComment->setWordWrapMode(QTextOption::WrapAnywhere);
	m_pSetScaleComment->setTextInteractionFlags(Qt::NoTextInteraction);
	m_pSetScaleComment->setFrameShape(QFrame::NoFrame);

	// Add set floor widget to vertical layout.
	m_pVerticalLayout->addLayout(m_pSetFloorHorizontalLayout);
	m_pVerticalLayout->addWidget(m_pSetFloorComment);
	m_pVerticalLayout->addWidget(m_pSeparator1);
	// Add scale widget to vertical layout.
	m_pVerticalLayout->addLayout(m_pSetScaleHorizontalLayout);
	m_pVerticalLayout->addWidget(m_pSetScaleComment);

	connect(m_pSetFloorCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnStateChangedOfFloorCheckBox(int)));
	connect(m_pSetScaleCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnStateChangedOfScaleCheckBox(int)));
	connect(m_pSetFloorCheckBox, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pSetFloorBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pSetScaleBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));

	QString strText("");
	strText = strText + STR_HTML_HEADER \
		+ "<b>" + tr("Instruction1") + "</b> <br><br>" \
		+ STR_HTEML_SPACE + STR_HTEML_SPACE + tr("1)SetOrg.") + "<br>" \
		+ STR_HTEML_SPACE + STR_HTEML_SPACE + tr("2)SetAxis.")\
		+ STR_HTML_TAILER;
	m_pSetFloorComment->setText(strText);

	strText = "";
	strText = strText + STR_HTML_HEADER\
		+ "<b>" + tr("Instruction2") + "</b> <br><br>" \
		+ STR_HTEML_SPACE + STR_HTEML_SPACE + tr("1)ChooseHelperAxis.") + "<br>" \
		+ STR_HTEML_SPACE + STR_HTEML_SPACE + tr("2)DrawHelperAxis.") + "<br>" \
		+ STR_HTEML_SPACE + STR_HTEML_SPACE + tr("3)PressCommit.") + "<br>" \
		+ STR_HTML_TAILER;
	m_pSetScaleComment->setText(strText);
}

void CRoom3DStep2ToolBox::OnStateChangedOfFloorCheckBox(int a_nState)
{
	if(m_pSetFloorCheckBox->checkState() == Qt::Checked)
		//if(a_nState == Qt::Checked)
	{
		CRoom3DSetScaleDlg dlg(g_pApp->GetMainWidget());
		if(dlg.exec() == QDialog::Accepted)
		{
			m_pSetScaleBtn->setState(BTN_STATE_NORMAL);
			m_pSetFloorBtn->setState(BTN_STATE_DISABLE);
			m_pSetFloorCheckBox->setEnabled(false);
			g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->SetComplete();
			g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->SetScaleParam(dlg.getScaleAxis(), dlg.getScale());
			g_pApp->m_pMainWidget->ShowGuideDlg(STEP_2_SET_SCALE);
			g_pApp->m_pMainWidget->UpdateControlState();
		}
		else
		{
			m_pSetFloorCheckBox->setCheckState(Qt::Unchecked);
		}
	}
}

void CRoom3DStep2ToolBox::OnStateChangedOfScaleCheckBox(int a_nState)
{
	if(a_nState == Qt::Checked)
	{
		m_pSetScaleCheckBox->setEnabled(false);
		g_pApp->SetAppStep(APP_EDIT_STEP_3);
		g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->SetComplete();
	}
}

void CRoom3DStep2ToolBox::pressedButton(int a_nBtnID)
{
	QString fileName;
	switch(a_nBtnID)
	{
	case BTN_SET_FLOOR:
		{
			g_pApp->m_pMainWidget->ShowGuideDlg(STEP_NONE);
			g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->ResetParameters();
			g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->StartSetFloor();
			UpdateControlState();
		}
		break;
	case BTN_SET_SCALE:
		{
			g_pApp->m_pMainWidget->ShowGuideDlg(STEP_NONE);
			g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->StartSetScale((QWidget*)this);
			UpdateControlState();
		}
		break;
	case BTN_TUTORIAL:
		{
			ShowFunctionImplementMsgBox();
		}
		break;
	}
	g_pApp->GetSceneView()->updateGL();
}

void CRoom3DStep2ToolBox::UpdateControlState()
{
	int nStep = g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->GetStep();
	int nState = g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->GetState();
	bool blComplete = g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->GetStageComplete();
	int nStepIndex = nStep;
	if(nState == CCameraInfo::CS_SETFLOOR)
		nStep = 4;
	else if(nState == CCameraInfo::CS_SETSCALE)
		nStep = 6;
	m_pSetScaleBtn->setState(BTN_STATE_DISABLE);
	m_pSetFloorBtn->setState(BTN_STATE_DISABLE);
	switch(nStep)
	{
	case -1:
	case 0:
		{
			ResetState();
			break;
		}
	case 4:
		{
			if(nState == CCameraInfo::CS_SETFLOOR)
			{
				
				if(blComplete)
				{
					m_pSetScaleBtn->setState(BTN_STATE_NORMAL);
					m_pSetFloorCheckBox->setEnabled(false);
					m_pSetFloorCheckBox->setCheckState(Qt::Checked);
				}
				else
				{
					m_pSetFloorBtn->setState(BTN_STATE_NORMAL);
					m_pSetFloorCheckBox->setEnabled(true);
					m_pSetFloorCheckBox->setCheckState(Qt::Unchecked);
				}
			}
			break;
		}
	case 5:
	case 6:
		{
			
			m_pSetScaleBtn->setState(BTN_STATE_NORMAL);
			if(nStepIndex == 6)
			{
				m_pSetScaleCheckBox->setEnabled(true);
			}
			else
			{
				m_pSetScaleCheckBox->setEnabled(false);
				m_pSetScaleCheckBox->setCheckState(Qt::Unchecked);
			}
			break;
		}
	}
}

void CRoom3DStep2ToolBox::ResetState()
{
	m_pSetFloorBtn->setState(BTN_STATE_NORMAL);
	m_pSetFloorCheckBox->setEnabled(false);
	m_pSetFloorCheckBox->setCheckState(Qt::Unchecked);
	m_pSetScaleBtn->setState(BTN_STATE_DISABLE);
	m_pSetScaleCheckBox->setEnabled(false);
	m_pSetScaleCheckBox->setCheckState(Qt::Unchecked);
}

#else
CRoom3DStep2ToolBox::CRoom3DStep2ToolBox(QWidget* parent) : CRoom3DBaseToolBox(parent)
{
	InitCotrols();
}

CRoom3DStep2ToolBox::~CRoom3DStep2ToolBox(void)
{
	SAFE_DELETE(m_pVerticalLayout);
}


void CRoom3DStep2ToolBox::InitCotrols()
{
	InitBkWidget();

	// Create Vertical Layout
	m_pVerticalLayout = new QVBoxLayout(m_pBkWidget);
	m_pVerticalLayout->setSpacing(6);
	m_pVerticalLayout->setContentsMargins(3, 65, 3, TOOLBOX_CONTENTS_MARGIN);

	// Set corner controls
	m_pSetCornerLayout = new QVBoxLayout();
	m_pSetCornerLayout->setContentsMargins(TOOLBOX_CONTENTS_MARGIN, 0, TOOLBOX_CONTENTS_MARGIN, 0);
	m_pSetCornerBtnLayout = new QHBoxLayout();
	m_pSetCornerBtnLayout->setContentsMargins(0, 0, 0, 0);
	m_pSetCornerBtn = new CRoom3DImageButton(BUTTON_IMG_SETCORNER, BTN_SET_CORNER, Qt::PointingHandCursor, m_pBkWidget);
	m_pSetCornerBtnLayout->addWidget(m_pSetCornerBtn);

	m_pSetCornerComment = new QTextEdit(m_pBkWidget);
	m_pSetCornerComment->setReadOnly(true);
	m_pSetCornerComment->setWordWrapMode(QTextOption::WrapAnywhere);
	m_pSetCornerComment->setTextInteractionFlags(Qt::NoTextInteraction);
	SetDefaultPalette(m_pSetCornerComment);
	m_pSetCornerComment->setFrameShape(QFrame::NoFrame);
	m_pSetCornerComment->setMaximumHeight(65);
	m_pSetCornerLayout->addLayout(m_pSetCornerBtnLayout);
	m_pSetCornerLayout->addWidget(m_pSetCornerComment);

	m_pSeparator1 = new QFrame(m_pBkWidget);
	m_pSeparator1->setMinimumHeight(TOOLBOX1_SEPARATOR_HEIGHT);
	m_pSeparator1->setMaximumHeight(TOOLBOX1_SEPARATOR_HEIGHT);
	m_pSeparator1->setFrameShape(QFrame::HLine);
	m_pSeparator1->setFrameShadow(QFrame::Sunken);
	QPalette curPalette = m_pSeparator1->palette();
	curPalette.setColor(QPalette::Dark, TOOLBOX_SEPARATOR_COLOR);
	curPalette.setColor(QPalette::Light, TOOLBOX_SEPARATOR_COLOR);
	m_pSeparator1->setPalette(curPalette);

	// Set scale controls
	m_pSetScaleLayout = new QVBoxLayout();
	m_pSetScaleLayout->setContentsMargins(TOOLBOX_CONTENTS_MARGIN, 0, TOOLBOX_CONTENTS_MARGIN, 0);
	m_pSetScaleBtnLayout = new QHBoxLayout();
	m_pSetScaleBtnLayout->setContentsMargins(0, 0, 0, 0);
	m_pSetScaleBtn = new CRoom3DImageButton(BUTTON_IMG_SETHEIGHT, BTN_SET_HEIGHT, Qt::PointingHandCursor, m_pBkWidget);
	m_pSetScaleBtnLayout->addWidget(m_pSetScaleBtn);

	m_pSetScaleComment = new QTextEdit(m_pBkWidget);
	m_pSetScaleComment->setReadOnly(true);
	//m_pSetScaleComment->setWordWrapMode(QTextOption::WrapAnywhere);
	//m_pSetScaleComment->setTextInteractionFlags(Qt::NoTextInteraction);
	SetDefaultPalette(m_pSetScaleComment);
	m_pSetScaleComment->setFrameShape(QFrame::NoFrame);
	m_pSetScaleComment->setMaximumHeight(45);

	m_pSetScaleLayout->addLayout(m_pSetScaleBtnLayout);
	m_pSetScaleLayout->addWidget(m_pSetScaleComment);

	m_pSeparator2 = new QFrame(m_pBkWidget);
	m_pSeparator2->setMinimumHeight(TOOLBOX1_SEPARATOR_HEIGHT);
	m_pSeparator2->setMaximumHeight(TOOLBOX1_SEPARATOR_HEIGHT);
	m_pSeparator2->setFrameShape(QFrame::HLine);
	m_pSeparator2->setFrameShadow(QFrame::Sunken);
	curPalette = m_pSeparator2->palette();
	curPalette.setColor(QPalette::Dark, TOOLBOX_SEPARATOR_COLOR);
	curPalette.setColor(QPalette::Light, TOOLBOX_SEPARATOR_COLOR);
	m_pSeparator2->setPalette(curPalette);

	// Calibrate controls
	m_pCalibrateLayout = new QVBoxLayout();
	m_pCalibrateLayout->setContentsMargins(TOOLBOX_CONTENTS_MARGIN, 0, TOOLBOX_CONTENTS_MARGIN, 0);
	m_pCalibrateBtnLayout = new QHBoxLayout();
	m_pCalibrateBtnLayout->setContentsMargins(0, 0, 0, 0);

	m_pCalibrateBtn = new CRoom3DImageButton(BUTTON_IMG_CALIBRATE, BTN_CALIBRATE, Qt::PointingHandCursor, m_pBkWidget);
	m_pCalibrateBtnLayout->addWidget(m_pCalibrateBtn);

	m_pCalibrateComment = new QTextEdit(m_pBkWidget);
	m_pCalibrateComment->setReadOnly(true);
	//m_pCalibrateComment->setWordWrapMode(QTextOption::WrapAnywhere);
	//m_pCalibrateComment->setTextInteractionFlags(Qt::NoTextInteraction);
	SetDefaultPalette(m_pCalibrateComment);
	m_pCalibrateComment->setFrameShape(QFrame::NoFrame);
	m_pCalibrateComment->setMaximumHeight(65);

	m_pFinishSettingLayout = new QHBoxLayout();
	m_pFinishSettingLayout->setContentsMargins(36, 0, 36, 0);
	m_pFinishSettingCheckBox = new QCheckBox(m_pBkWidget);
	m_pFinishSettingCheckBox->setFixedSize(QSize(160,40));
	SetDefaultPalette(m_pFinishSettingCheckBox);
	m_pFinishSettingLayout->addWidget(m_pFinishSettingCheckBox);
	m_pCalibrateLayout->addLayout(m_pCalibrateBtnLayout);
	m_pCalibrateLayout->addWidget(m_pCalibrateComment);
	m_pCalibrateLayout->addLayout(m_pFinishSettingLayout);

	m_pVerticalLayout->addLayout(m_pSetCornerLayout);
	m_pVerticalLayout->addWidget(m_pSeparator1);
	m_pVerticalLayout->addLayout(m_pSetScaleLayout);
	m_pVerticalLayout->addWidget(m_pSeparator2);
	m_pVerticalLayout->addLayout(m_pCalibrateLayout);
	m_pVerticalLayout->addStretch();

	QString strText;
	strText = QString("") + STR_HTML_HEADER + STR_HTML_MID_FONT_BEGIN + tr("Set Corner Comment") + STR_HTML_FONT_END+ STR_HTML_TAILER;
	m_pSetCornerComment->setText(strText);
	strText = QString("") + STR_HTML_HEADER + STR_HTML_MID_FONT_BEGIN + tr("Set Height Comment") + STR_HTML_FONT_END + STR_HTML_TAILER;
	m_pSetScaleComment->setText(strText);
	strText = QString("") + STR_HTML_HEADER + STR_HTML_MID_FONT_BEGIN + tr("Calibrate Comment") + STR_HTML_FONT_END + STR_HTML_TAILER;
	m_pCalibrateComment->setText(strText);

	QFont lblFont = m_pFinishSettingCheckBox->font();
	lblFont.setPointSize(13);
	lblFont.setFamily("Microsoft JhengHei");
	lblFont.setBold(true);
	m_pFinishSettingCheckBox->setFont(lblFont);

	m_pFinishSettingCheckBox->setText(tr("Finish Setting"));
	curPalette = m_pFinishSettingCheckBox->palette();
	curPalette.setColor(QPalette::WindowText, TOOLBOX2_FINISH_COLOR);
	m_pFinishSettingCheckBox->setPalette(curPalette);

	connect(m_pFinishSettingCheckBox, SIGNAL(stateChanged(int)), this, SLOT(OnFinishSetting(int)));
	connect(m_pSetCornerBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pSetScaleBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pCalibrateBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
}


void CRoom3DStep2ToolBox::OnFinishSetting(int a_nState)
{
	if(a_nState == Qt::Checked)
	{
		m_pFinishSettingCheckBox->setEnabled(false);
		g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->SetComplete();
		g_pApp->GetMainWidget()->ShowGuideDlg(STEP_NONE);
		g_pApp->SetAppStep(APP_EDIT_STEP_3);
	}
}

void CRoom3DStep2ToolBox::ResetState()
{
	m_pSetCornerBtn->setState(BTN_STATE_NORMAL);
	m_pSetScaleBtn->setState(BTN_STATE_DISABLE);
	m_pCalibrateBtn->setState(BTN_STATE_DISABLE);
	m_pFinishSettingCheckBox->setEnabled(false);
	m_pFinishSettingCheckBox->setCheckState(Qt::Unchecked);
}

void CRoom3DStep2ToolBox::pressedButton(int a_nBtnID)
{
	QString fileName;
	switch(a_nBtnID)
	{
	case BTN_SET_CORNER:
		{
			g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->ResetParameters();
			g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->StartSetFloor();
			g_pApp->GetMainWidget()->ShowGuideDlg(STEP_2_SET_HEIGHT);
			g_pApp->GetMainWidget()->UpdateControlState();
		}
		break;
	case BTN_SET_HEIGHT:
		{
			CRoom3DSetScaleDlg dlg(g_pApp->GetMainWidget());
			if(dlg.exec() == QDialog::Accepted)
			{
				g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->SetComplete();
				g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->SetScaleParam(dlg.getScaleAxis(), dlg.getScale());
				g_pApp->GetMainWidget()->ShowGuideDlg(STEP_2_CALIBRATE);
				g_pApp->m_pMainWidget->UpdateControlState();
			}
			g_pApp->GetMainWidget()->UpdateControlState();
		}
		break;
	case BTN_CALIBRATE:
		{

			if(g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->StartSetScale((QWidget*)this))
			{
				g_pApp->GetMainWidget()->ShowGuideDlg(STEP_NONE);
				g_pApp->GetMainWidget()->UpdateControlState();
				break;
			}
		}
	}
	g_pApp->GetSceneView()->updateGL();
}

void CRoom3DStep2ToolBox::UpdateControlState()
{
	int nStep = g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->GetStep();
	int nState = g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->GetState();
	bool blComplete = g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->GetStageComplete();
	int nStepIndex = nStep;
	if(nState == CCameraInfo::CS_SETFLOOR)
		nStep = 4;
	else if(nState == CCameraInfo::CS_SETSCALE)
		nStep = 6;
	m_pSetCornerBtn->setState(BTN_STATE_DISABLE);
	m_pSetScaleBtn->setState(BTN_STATE_DISABLE);
	m_pCalibrateBtn->setState(BTN_STATE_DISABLE);
	switch(nStep)
	{
	case -1:
	case 0:
		{
			ResetState();
			break;
		}
	case 4:
		{
			if(nState == CCameraInfo::CS_SETFLOOR)
			{

				if(blComplete)
				{
					m_pCalibrateBtn->setState(BTN_STATE_NORMAL);
				}
				else
				{
					m_pSetScaleBtn->setState(BTN_STATE_NORMAL);
				}
			}
			break;
		}
	case 5:
	case 6:
		{

			m_pCalibrateBtn->setState(BTN_STATE_NORMAL);
			if(nStepIndex == 6)
			{
				m_pFinishSettingCheckBox->setEnabled(true);
				g_pApp->GetMainWidget()->ShowGuideDlg(STEP_2_FINISH_SETTING, QPoint(0,0));
			}
			else
			{
				m_pFinishSettingCheckBox->setEnabled(false);
				m_pFinishSettingCheckBox->setCheckState(Qt::Unchecked);
			}
			break;
		}
	}

	switch(g_pApp->GetMainWidget()->m_nGuideId)
	{
	case STEP_2_SET_CORNER:
		g_pApp->GetMainWidget()->m_pGuidedWidget = m_pSetCornerBtn;
		break;
	case STEP_2_SET_HEIGHT:
		g_pApp->GetMainWidget()->m_pGuidedWidget = m_pSetScaleBtn;
		break;
	case STEP_2_CALIBRATE:
		g_pApp->GetMainWidget()->m_pGuidedWidget = m_pCalibrateBtn;
		break;
	case STEP_2_FINISH_SETTING:
		g_pApp->GetMainWidget()->m_pGuidedWidget = m_pFinishSettingCheckBox;
		break;
	}

}
#endif