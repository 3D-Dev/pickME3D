#include "Room3DStep1ToolBox.h"
#include "Common.h"
#include "Define.h"
#include <QFileDialog>
#include "Room3DApplication.h"
#include "Room3DUtility.h"

CRoom3DStep1ToolBox::CRoom3DStep1ToolBox(QWidget* parent) : CRoom3DBaseToolBox(parent)
{
	InitControls();
}

CRoom3DStep1ToolBox::~CRoom3DStep1ToolBox(void)
{
	SAFE_DELETE(m_pVerticalLayout);
}

void CRoom3DStep1ToolBox::InitControls()
{
	InitBkWidget();

	m_pVerticalLayout = new QVBoxLayout(m_pBkWidget);
	m_pVerticalLayout->setSpacing(6);
	m_pVerticalLayout->setContentsMargins(3, 55, 3, TOOLBOX_CONTENTS_MARGIN);

	// Add browse button
	m_pBrowseLayout = new QHBoxLayout();
	m_pBrowseLayout->setContentsMargins(TOOLBOX_CONTENTS_MARGIN, 0, TOOLBOX_CONTENTS_MARGIN, 0);
	m_pBrowseBtn = new CRoom3DImageButton(BUTTON_IMG_BROWSE, BTN_BROWSE, Qt::PointingHandCursor, m_pBkWidget);
	m_pBrowseLayout->addWidget(m_pBrowseBtn);

	m_pSeparator = new QFrame(m_pBkWidget);
	m_pSeparator->setMinimumHeight(5);
	m_pSeparator->setMaximumHeight(5);
	m_pSeparator->setFrameShape(QFrame::HLine);
	m_pSeparator->setFrameShadow(QFrame::Sunken);

	QPalette curPalette = m_pSeparator->palette();
	curPalette.setColor(QPalette::Dark, TOOLBOX_SEPARATOR_COLOR);
	curPalette.setColor(QPalette::Light, TOOLBOX_SEPARATOR_COLOR);
	m_pSeparator->setPalette(curPalette);

	// Add comment
	m_pComment = new QTextEdit(m_pBkWidget);
	SetDefaultPalette(m_pComment);
	m_pComment->setFrameShape(QFrame::NoFrame);
	m_pCommentLayout = new QHBoxLayout();
	m_pCommentLayout->setContentsMargins(TOOLBOX_CONTENTS_MARGIN, 0, TOOLBOX_CONTENTS_MARGIN, 0);
	m_pCommentLayout->addWidget(m_pComment);

	m_pVerticalLayout->addLayout(m_pBrowseLayout);
	m_pVerticalLayout->addWidget(m_pSeparator);
	m_pVerticalLayout->addLayout(m_pCommentLayout);


	// connect signal to slots of button
	connect(m_pBrowseBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));

	QString strText("");
	strText = strText + STR_HTML_HEADER + "<dl>"
		+ "<dt>" + STR_HTML_BIG_FONT_BEGIN + "<b>" + tr("Description:") + "</b>" + STR_HTML_FONT_END + "</dt>" \
		+ "<dd><br>" + STR_HTML_MID_FONT_BEGIN + tr("Accepted only file type of *.jpg, *.png and *.bmp") +  + STR_HTML_FONT_END + "</dd><bt><br>" \
		+ "<dt>" + STR_HTML_BIG_FONT_BEGIN + "<b>" + tr("Recommended for the best performance:") + "</b>" + STR_HTML_FONT_END + "</dt>"\
		+ "<dd><br>" + STR_HTML_MID_FONT_BEGIN + tr("- Size of 8*10 inches, landscape photo (25.4*20.32 cm)") + "<br><br>" \
		+ tr("- Please avoid photos captured by wide lens or fisheye function") + STR_HTML_FONT_END + "</dd>" + STR_HTML_TAILER;
	m_pComment->setText(strText);

	m_pComment->setReadOnly(true);
	//m_pComment->setWordWrapMode(QTextOption::WrapAnywhere);
	//m_pComment->setTextInteractionFlags(Qt::NoTextInteraction);
}

void CRoom3DStep1ToolBox::pressedButton(int a_nBtnID)
{
	QString fileName;
	switch(a_nBtnID)
	{
	case BTN_BROWSE:
		{
			g_pApp->m_pMainWidget->UpdateControlState();
			QString sDefaultFolder = g_pApp->GetWorkDir();
			QDir rootDir(sDefaultFolder);
			if(!rootDir.exists(STR_SAMPLE_PHOTO_DIR_NAME))
				rootDir.mkdir(STR_SAMPLE_PHOTO_DIR_NAME);
			rootDir.cd(STR_SAMPLE_PHOTO_DIR_NAME);
			sDefaultFolder = rootDir.absolutePath();
			fileName = QFileDialog::getOpenFileName(this, SELECT_IMAGE, sDefaultFolder, FILTER_IMAGES);
			if(!fileName.isEmpty())
			{
				g_pApp->GetDocument()->UpdateDoc(APP_MSG_LOAD_BACKGROUND_IMG, (long)&fileName);
				g_pApp->SetAppStep(APP_EDIT_STEP_2);
				float fovx = GetFovy(fileName);
				g_pApp->GetDocument()->Get3DSceneMgr()->GetCamera()->SetFOVX(fovx);
			}
		}
		break;
	}
}

void CRoom3DStep1ToolBox::UpdateControlState()
{
	if(g_pApp->GetMainWidget()->m_nGuideId == STEP_1_BROWSE)
	{
		g_pApp->GetMainWidget()->m_pGuidedWidget = m_pBrowseBtn;
	}
}

CRoom3DBaseToolBox::CRoom3DBaseToolBox(QWidget* a_pParent) : QWidget(a_pParent)
{
	m_pBkWidget = NULL;
	m_pBaseLayout = NULL;

}

CRoom3DBaseToolBox::~CRoom3DBaseToolBox()
{
	SAFE_DELETE(m_pBkWidget);
}

void CRoom3DBaseToolBox::InitBkWidget()
{
	m_pBkWidget = new CRoom3DImageWidget(BACK_IMG_TOOLBOXBAR, "", 30, true, this);
	m_pBaseLayout = new QVBoxLayout(this);
	m_pBaseLayout->setContentsMargins(0,0,0,0);
	m_pBaseLayout->addWidget(m_pBkWidget);
}

