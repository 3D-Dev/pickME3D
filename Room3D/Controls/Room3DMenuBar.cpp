#include "Room3DMenuBar.h"
#include "Common.h"
#include "Room3DApplication.h"
#include "Room3DUtility.h"
#include "Room3DUndoMgr.h"
#include "Room3DBrightDlg.h"

#define SHARE_PHOTO_URL "http://www.facebook.com/pages/Pickme3D/143438815796650"

CRoom3DMenuBar::CRoom3DMenuBar(QWidget* parent): QFrame(parent)
{
	InitControls();
// 	this->setFrameShape(QFrame::Box);
// 	this->setFrameShadow(QFrame::Raised);
}

CRoom3DMenuBar::~CRoom3DMenuBar(void)
{
	SAFE_DELETE(m_pHorizontalLayout);
#if USE_FILE_MENU /*002->Button to File Menu*/
	SAFE_DELETE(m_pFileBtn);
	SAFE_DELETE(m_pFileMenu);
	SAFE_DELETE(m_pFileAction);
	SAFE_DELETE(m_pNewAction);
	SAFE_DELETE(m_pSaveAction);
	SAFE_DELETE(m_pOpenAction);
#else
	SAFE_DELETE(m_pNewBtn);
	SAFE_DELETE(m_pOpenBtn);
	SAFE_DELETE(m_pSaveBtn);
#endif
	SAFE_DELETE(m_pUndoBtn);
	SAFE_DELETE(m_pRedoBtn);
	SAFE_DELETE(m_pResetBtn);
	SAFE_DELETE(m_pBrightBtn);
	SAFE_DELETE(m_pSavePhotoBtn);
	SAFE_DELETE(m_pSharePhotoBtn);
	SAFE_DELETE(m_pReportBtn);

	SAFE_DELETE(m_pTutorialBtn);
}

void CRoom3DMenuBar::InitControls()
{
	m_pBkWidget = new CRoom3DImageWidget(this);
	m_pBaseLayout = new QVBoxLayout(this);
	m_pBaseLayout->setContentsMargins(0,0,0,0);
	m_pBaseLayout->addWidget(m_pBkWidget);

	m_pHorizontalLayout = new QHBoxLayout(m_pBkWidget);
	m_pHorizontalLayout->setSpacing(0);
	m_pHorizontalLayout->setContentsMargins(0, 0, 0, 0);

	m_pHorizontalStartSpacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	m_pHorizontalLayout->addItem(m_pHorizontalStartSpacer);

#if USE_FILE_MENU /*002->Button to File Menu*/
	m_pFileBtn = new CRoom3DImageButton(BUTTON_IMG_FILE, BTN_FILE, Qt::PointingHandCursor, this);
	
	m_pFileMenu = new CRoom3DMenu(this);

	m_pFileAction = new CRoom3DAction(this);
	m_pFileAction->setStatic(true);
	m_pFileAction->setText(tr("File"));

	m_pNewAction = new CRoom3DAction(this);
	m_pNewAction->setText(tr("New Project"));

	m_pSaveAction = new CRoom3DAction(this);
	m_pSaveAction->setText(tr("Save Project"));

	m_pOpenAction = new CRoom3DAction(this);
	m_pOpenAction->setText(tr("Open Project"));

	m_pFileMenu->addAction(m_pFileAction);
	m_pFileMenu->addAction(m_pNewAction);
	m_pFileMenu->addAction(m_pSaveAction);
	m_pFileMenu->addAction(m_pOpenAction);

	m_pFileBtn->SetMenu(m_pFileMenu);
#else
	m_pNewBtn = new CRoom3DImageButton(BUTTON_IMG_NEW, BTN_NEW, Qt::PointingHandCursor, this);
	m_pOpenBtn = new CRoom3DImageButton(BUTTON_IMG_OPEN, BTN_OPEN, Qt::PointingHandCursor, this);
	m_pSaveBtn = new CRoom3DImageButton(BUTTON_IMG_SAVE, BTN_SAVE, Qt::PointingHandCursor, this);
#endif
	m_pUndoBtn = new CRoom3DImageButton(BUTTON_IMG_UNDO, BTN_UNDO, Qt::PointingHandCursor, this);
	m_pRedoBtn = new CRoom3DImageButton(BUTTON_IMG_REDO, BTN_REDO, Qt::PointingHandCursor, this);
	m_pResetBtn = new CRoom3DImageButton(BUTTON_IMG_RESET, BTN_RESET, Qt::PointingHandCursor, this);
	m_pBrightBtn = new CRoom3DImageButton(BUTTON_IMG_BRIGHT,BTN_BRIGHT, Qt::PointingHandCursor, this);
#if USE_SHOW_HIDE_FLOOR
	m_pVisibleFloorBtn = new CRoom3DImageButton(BUTTON_IMG_SHOW_FLOOR,BTN_VISIBLE_FLOOR, Qt::PointingHandCursor, this);
#endif
	m_pSavePhotoBtn = new CRoom3DImageButton(BUTTON_IMG_EXPORT, BTN_SAVE_PHOTO, Qt::PointingHandCursor, this);
	m_pSharePhotoBtn = new CRoom3DImageButton(BUTTON_IMG_SHARE_PHOTO, BTN_SHARE_PHOTO, Qt::PointingHandCursor, this);
	m_pImportModelBtn = new CRoom3DImageButton(BUTTON_IMG_IMPORT_MODEL, BTN_IMPORT_MODEL, Qt::PointingHandCursor, this);
	m_pReportBtn = new CRoom3DImageButton(BUTTON_IMG_PURCHASE, BTN_REPORT, Qt::PointingHandCursor, this);

	m_pTutorialBtn = new CRoom3DImageButton(BUTTON_IMG_TUTORIAL,BTN_TUTORIAL, Qt::PointingHandCursor, this);

#if USE_FILE_MENU /*002->Button to File Menu*/
	m_pHorizontalLayout->addWidget(m_pFileBtn);
#else
	m_pHorizontalLayout->addWidget(m_pNewBtn);
	m_pHorizontalLayout->addWidget(m_pOpenBtn);
	m_pHorizontalLayout->addWidget(m_pSaveBtn);
#endif

	m_pHorizontalLayout->addItem(new QSpacerItem(15, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
	m_pHorizontalLayout->addWidget(m_pUndoBtn);
	m_pHorizontalLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
	m_pHorizontalLayout->addWidget(m_pRedoBtn);
	m_pHorizontalLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
	m_pHorizontalLayout->addWidget(m_pResetBtn);
	m_pHorizontalLayout->addItem(new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));
	m_pHorizontalLayout->addWidget(m_pBrightBtn);
	m_pHorizontalLayout->addItem(new QSpacerItem(15, 20, QSizePolicy::Fixed, QSizePolicy::Minimum));

#if USE_SHOW_HIDE_FLOOR
	m_pHorizontalLayout->addWidget(m_pVisibleFloorBtn);
#endif
	m_pHorizontalLayout->addWidget(m_pSavePhotoBtn);
	m_pHorizontalLayout->addWidget(m_pSharePhotoBtn);
	m_pHorizontalLayout->addWidget(m_pImportModelBtn);
	m_pHorizontalLayout->addWidget(m_pReportBtn);
	
	m_pHorizontalMidSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);	m_pHorizontalLayout->addItem(m_pHorizontalMidSpacer);

	m_pHorizontalLayout->addWidget(m_pTutorialBtn);

	m_pHorizontalEndSpacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
	m_pHorizontalLayout->addItem(m_pHorizontalEndSpacer);

#if USE_FILE_MENU /*002->Button to File Menu*/
	connect(m_pFileBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pNewAction, SIGNAL(triggered(bool)), this, SLOT(OnNewProject()));
	connect(m_pSaveAction, SIGNAL(triggered(bool)), this, SLOT(OnSaveProject()));
	connect(m_pOpenAction, SIGNAL(triggered(bool)), this, SLOT(OnOpenProject()));
#else
	connect(m_pNewBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pOpenBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pSaveBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
#endif

	connect(m_pUndoBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pRedoBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pResetBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pBrightBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
#if USE_SHOW_HIDE_FLOOR
	connect(m_pVisibleFloorBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
#endif
	connect(m_pSavePhotoBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pSharePhotoBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pImportModelBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
	connect(m_pReportBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));

	connect(m_pTutorialBtn, SIGNAL(pressed(int)), this, SLOT(pressedButton(int)));
}

#if USE_FILE_MENU /*002->Button to File Menu*/
void CRoom3DMenuBar::OnNewProject()
{
	g_pApp->InitApplication();
	g_pApp->SetAppStep(APP_EDIT_STEP_1);
}

void CRoom3DMenuBar::OnSaveProject()
{
	g_pApp->GetDocument()->SaveDocument();
}

void CRoom3DMenuBar::OnOpenProject()
{
	g_pApp->GetDocument()->ReadDocument();
}
#endif

void CRoom3DMenuBar::pressedButton(int a_nButtonID)
{
	switch(a_nButtonID)
	{
#if USE_FILE_MENU /*002->Button to File Menu*/
	case BTN_FILE:
		{
			// Do nothing, the same processing is treated by QAction Slot function.
		}
		break;
#else
	case BTN_NEW:
		g_pApp->InitApplication();
		g_pApp->SetAppStep(APP_EDIT_STEP_1);
		break;
	case BTN_OPEN:
		g_pApp->GetDocument()->ReadDocument();
		break;
	case BTN_SAVE:
		g_pApp->GetDocument()->SaveDocument();
		break;
#endif
	case BTN_UNDO:
		g_pApp->Undo(true);
		break;
	case BTN_REDO:
		g_pApp->Undo(false);
		break;
	case BTN_RESET:
		{
			CRoom3DBaseCmd* pCmd = g_pApp->GetCommandMgr()->GetCurrentCmd();
			pCmd->ResetDocument();
			g_pApp->GetDocument()->GetUndoMgr()->Clear();
			g_pApp->GetMainWidget()->UpdateView();
			g_pApp->GetMainWidget()->UpdateControlState();

		}
		break;
	case BTN_BRIGHT:
		{
			int nBright, nContrast;
			g_pApp->GetDocument()->GetBrightAndContrast(nBright, nContrast);
			CRoom3DBrightDlg dlg(nBright, nContrast, g_pApp->GetMainWidget());
			int nResult = dlg.exec();
			if(nResult == QDialog::Accepted)
			{
				dlg.GetBrightAndContrast(nBright, nContrast);
				g_pApp->GetDocument()->UpdateDoc(APP_MSG_CHANGE_BK_IMG_BRIGHT_AND_CONTRAST, nBright, nContrast);
			}
		}
		break;
	case BTN_VISIBLE_FLOOR:
		ChangeFloorVisible();
		break;
	case BTN_SAVE_PHOTO:
		ExportToImage();
		break;
	case BTN_SHARE_PHOTO:
		SharePhoto();
		break;
	case BTN_IMPORT_MODEL:
		{
			QString sFilePath;
			sFilePath = QFileDialog::getOpenFileName(g_pApp->GetMainWidget(), tr("Import item"), "", tr("Item File (*.zip)"));
			sFilePath.replace("/", QDir::separator());
			if(!sFilePath.isEmpty())
			{
				// Unzip item file to temporary folder and get item folder path
				bool blSuccess = false;
				QString sFolderPath, sItemFolderPath;
				QDir tempDir = QDir::temp();
				tempDir.mkdir("Room3D");
				tempDir.cd("Room3D");
				sFolderPath = tempDir.absolutePath();
				UnZipArchive(sFilePath, sFolderPath, sItemFolderPath);
				// If this is item file
				if(IsAvailableItemDir(sItemFolderPath))
				{
					// import
					blSuccess = g_pApp->GetDocument()->GetItemMgr()->ImportItem(sItemFolderPath);
				}
				if(blSuccess == false)
				{
					QMessageBox::information(g_pApp->GetMainWidget(),tr("Item import"), tr("This foler doesn't contain any item information."));
				}
				// Delete temporary folder
				DeleteDirectory(sFolderPath);
			}
		}
		break;
	case BTN_REPORT:
		ExportToHTML();
		break;
	case BTN_TUTORIAL:
		g_pApp->GetMainWidget()->ShowTutorialVideo();
		break;
	}
}

bool CRoom3DMenuBar::SharePhoto()
{
	QUrl url(SHARE_PHOTO_URL);
	QString str = url.toEncoded();
	const TCHAR *tstr = (TCHAR *)str.utf16();
	HINSTANCE result = ::ShellExecute(0, NULL, tstr, NULL, NULL, SW_SHOWNORMAL);
	return result != 0;
}

void CRoom3DMenuBar::ChangeFloorVisible()
{
	bool blVisible = g_pApp->GetDocument()->Get3DSceneMgr()->GetFloor()->IsVisible();
	blVisible = !blVisible;
	g_pApp->GetDocument()->Get3DSceneMgr()->GetFloor()->SetVisible(blVisible);
	UpdateControlState();
	g_pApp->GetMainWidget()->UpdateView();
}

void CRoom3DMenuBar::UpdateControlState()
{
	CRoom3DUndoMgr* pUndoMgr = g_pApp->GetDocument()->GetUndoMgr();
	m_pUndoBtn->setState(pUndoMgr->IsUndoAble() ? BTN_STATE_NORMAL : BTN_STATE_DISABLE);
	m_pRedoBtn->setState(pUndoMgr->IsRedoAble() ? BTN_STATE_NORMAL : BTN_STATE_DISABLE);
	m_pBrightBtn->setState(g_pApp->GetDocument()->GetBackImage()->isNull() ? BTN_STATE_DISABLE : BTN_STATE_NORMAL);
	m_pSharePhotoBtn->setState(g_pApp->GetDocument()->GetBackImage()->isNull() ? BTN_STATE_DISABLE : BTN_STATE_NORMAL);
	m_pSavePhotoBtn->setState(g_pApp->GetDocument()->GetBackImage()->isNull() ? BTN_STATE_DISABLE : BTN_STATE_NORMAL);
	if(g_pApp->GetAppStep() == APP_EDIT_STEP_3)
	{
#if USE_SHOW_HIDE_FLOOR
		m_pVisibleFloorBtn->setState(BTN_STATE_NORMAL);
		if(g_pApp->GetDocument()->Get3DSceneMgr()->GetFloor()->IsVisible())
			m_pVisibleFloorBtn->SetPixmap(BUTTON_IMG_HIDE_FLOOR);
		else
			m_pVisibleFloorBtn->SetPixmap(BUTTON_IMG_SHOW_FLOOR);
#endif
		m_pImportModelBtn->setState(BTN_STATE_NORMAL);
		m_pSavePhotoBtn->setState(BTN_STATE_NORMAL);
		m_pReportBtn->setState(BTN_STATE_NORMAL);		
	}
	else
	{
#if USE_SHOW_HIDE_FLOOR
		m_pVisibleFloorBtn->setState(BTN_STATE_DISABLE);
#endif
		m_pImportModelBtn->setState(BTN_STATE_DISABLE);
		m_pSavePhotoBtn->setState(BTN_STATE_DISABLE);
		m_pReportBtn->setState(BTN_STATE_DISABLE);		
	}
#if USE_FILE_MENU /*002->Button to File Menu*/
	m_pSaveAction->setEnabled(g_pApp->GetDocument()->IsModified());
#else
	m_pSaveBtn->setState(g_pApp->GetDocument()->IsModified() ? BTN_STATE_NORMAL : BTN_STATE_DISABLE);
#endif
}

void CRoom3DMenuBar::ExportToImage()
{
	QString fileName;

	fileName = QFileDialog::getSaveFileName(this, tr("Save Image File"), "", tr("Images (*.png)"));
	if(!fileName.isEmpty())
	{
		CRoom3DSceneView* pView = g_pApp->GetMainWidget()->GetSceneView();
		g_pApp->GetDocument()->BeginExporting();
		pView->updateGL();
		QImage image = pView->grabFrameBuffer();
		QFont font("Arial", 12, QFont::Bold);
		QPainter painter(&image);
		QRect rect(0,0,image.width(), image.height());
		painter.setPen(Qt::blue);
		painter.setFont(font);
		painter.drawText(rect, Qt::AlignRight|Qt::AlignBottom, tr("www.pickme3d.com"));
		image.save(fileName, "PNG");
		g_pApp->GetDocument()->EndExporting();
		pView->updateGL();
	}
}

void CRoom3DMenuBar::ExportToHTML()
{
	QString fileName;
	fileName = QFileDialog::getSaveFileName(this, tr("Save HTML File"), "", tr("HTML File (*.htm)"));
	if(!fileName.isEmpty())
	{
		g_pApp->GetDocument()->ExportToHTML(fileName);
	}
}