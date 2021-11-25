#include "Room3DToolBoxBar.h"
#include "Common.h"
#include "Define.h"
#include "Room3DApplication.h"
#include "Room3DUtility.h"

CRoom3DToolBoxBar::CRoom3DToolBoxBar(QWidget* parent) : QFrame(parent)
{
	InitControls();
// 	this->setFrameShape(QFrame::Box);
// 	this->setFrameShadow(QFrame::Raised);
}

CRoom3DToolBoxBar::~CRoom3DToolBoxBar(void)
{
	SAFE_DELETE(m_pStep1ToolBox);
	SAFE_DELETE(m_pStep2ToolBox);
	SAFE_DELETE(m_pStep3ToolBox);
	SAFE_DELETE(m_pStackedWidget);
	SAFE_DELETE(m_pLayout);
}

void CRoom3DToolBoxBar::InitControls()
{
	m_pStep1ToolBox = new CRoom3DStep1ToolBox(this);
	m_pStep2ToolBox = new CRoom3DStep2ToolBox(this);
	m_pStep3ToolBox = new CRoom3DStep3ToolBox(this);

	m_pStackedWidget = new QStackedWidget();
	m_pStackedWidget->addWidget(m_pStep1ToolBox);
	m_pStackedWidget->addWidget(m_pStep2ToolBox);
	m_pStackedWidget->addWidget(m_pStep3ToolBox);

	m_pLayout = new QVBoxLayout;
	m_pLayout->setContentsMargins(0,0,5,5);
	m_pLayout->addWidget(m_pStackedWidget);
	setLayout(m_pLayout);
}

void CRoom3DToolBoxBar::UpdateControlState(void* a_pUserData)
{
	APP_STEP eStep = g_pApp->GetAppStep();
	int nCurPage = 0;
	switch(eStep)
	{
	case APP_EDIT_STEP_1:
		nCurPage = 0;
		m_pStep1ToolBox->UpdateControlState();
		break;
	case APP_EDIT_STEP_2:
		nCurPage = 1;
		m_pStep2ToolBox->UpdateControlState();
		break;
	case APP_EDIT_STEP_3:
		nCurPage = 2;
		m_pStep3ToolBox->UpdateControlState(a_pUserData);
		break;
	}
	m_pStackedWidget->setCurrentIndex(nCurPage);
}
