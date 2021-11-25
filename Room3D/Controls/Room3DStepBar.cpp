#include "Room3DStepBar.h"
#include "Common.h"
#include "Room3DApplication.h"
#include "Room3DUtility.h"

// #define MANUAL_STEP_SWITCH
CRoom3DStepBar::CRoom3DStepBar(QWidget* parent) : QFrame(parent)
{
	InitControls();
// 	this->setFrameShape(QFrame::Box);
// 	this->setFrameShadow(QFrame::Raised);
}

CRoom3DStepBar::~CRoom3DStepBar(void)
{
	SAFE_DELETE(m_pMainLayout);
//	SAFE_DELETE(m_pLangBtn);
}


void CRoom3DStepBar::InitControls()
{
	m_pMainLayout = new QHBoxLayout(this);
	m_pMainLayout->setSpacing(0);
	m_pMainLayout->setContentsMargins(0, 0, 0, 0);
	m_pBkWidget = new CRoom3DImageWidget(BUTTON_IMG_STEP_COMMENT, "", 0, true, this);
	m_pMainLayout->addWidget(m_pBkWidget);

	m_pBkLayout = new QHBoxLayout(m_pBkWidget);
	m_pBkLayout->setSpacing(0);
	m_pBkLayout->setContentsMargins(0, 0, 0, 0);

	m_pStepBtn = new CRoom3DImageWidget(BUTTON_IMG_STEP1, "", 0, true, this);
	m_pStepBtn->setMinimumWidth(STEPBAR_BTN_WIDTH);
	m_pStepBtn->setMaximumWidth(STEPBAR_BTN_WIDTH);


	m_pStepComment = new CRoom3DImageWidget("", "", 0, false, this);
	m_pStepCommentLayout = new QVBoxLayout(m_pStepComment);
	m_pStepCommentLayout->setSpacing(2);
	m_pStepCommentLayout->setContentsMargins(6, 4, 0, 4);
	m_pStep1CommentLbl = new QLabel();
	SetStepLabelPalette(m_pStep1CommentLbl);
	m_pStep1CommentLbl->setText(tr("Step1CommentText"));
	QFont lblFont = m_pStep1CommentLbl->font();
	lblFont.setFamily(COMMENT_FONT_FAMILY);
	lblFont.setPointSize(12);
	m_pStep1CommentLbl->setFont(lblFont);
	m_pStep2CommentLbl_1 = new QLabel();
	lblFont.setPointSize(8);
	SetStepLabelPalette(m_pStep2CommentLbl_1);
	m_pStep2CommentLbl_1->setText(tr("Step2Comment1Text"));
	m_pStep2CommentLbl_1->setFont(lblFont);
	m_pStep2CommentLbl_2 = new QLabel();
	SetStepLabelPalette(m_pStep2CommentLbl_2);
	m_pStep2CommentLbl_2->setText(tr("Step2Comment2Text"));
	m_pStep2CommentLbl_2->setFont(lblFont);
	m_pStep3CommentLbl_1 = new QLabel();
	SetStepLabelPalette(m_pStep3CommentLbl_1);
	m_pStep3CommentLbl_1->setText(tr("Step3Comment1Text"));
	m_pStep3CommentLbl_1->setFont(lblFont);
	m_pStep3CommentLbl_2 = new QLabel();
	SetStepLabelPalette(m_pStep3CommentLbl_2);
	m_pStep3CommentLbl_2->setText(tr("Step3Comment2Text"));
	m_pStep3CommentLbl_2->setFont(lblFont);

	m_pStepCommentLayout->addWidget(m_pStep1CommentLbl);
	m_pStepCommentLayout->addWidget(m_pStep2CommentLbl_1);
	m_pStepCommentLayout->addWidget(m_pStep2CommentLbl_2);
	m_pStepCommentLayout->addWidget(m_pStep3CommentLbl_1);
	m_pStepCommentLayout->addWidget(m_pStep3CommentLbl_2);
	
	m_pBkLayout->addWidget(m_pStepBtn);
	m_pBkLayout->addWidget(m_pStepComment);

//	connect(m_pLangBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
}

void CRoom3DStepBar::UpdateControlState()
{
	m_pStep1CommentLbl->hide();
	m_pStep2CommentLbl_1->hide();
	m_pStep2CommentLbl_2->hide();
	m_pStep3CommentLbl_1->hide();
	m_pStep3CommentLbl_2->hide();
	QString sStepImgPath = "";
	switch(g_pApp->GetAppStep())
	{
	case APP_EDIT_STEP_1:
		sStepImgPath = BUTTON_IMG_STEP1;
		m_pStep1CommentLbl->show();
		break;
	case APP_EDIT_STEP_2:
		sStepImgPath = BUTTON_IMG_STEP2;
		m_pStep2CommentLbl_1->show();
		m_pStep2CommentLbl_2->show();
		break;
	case APP_EDIT_STEP_3:
		sStepImgPath = BUTTON_IMG_STEP3;
		m_pStep3CommentLbl_1->show();
		m_pStep3CommentLbl_2->show();
		break;
	}
	m_pStepBtn->SetImage(sStepImgPath, "", 10, true);
}