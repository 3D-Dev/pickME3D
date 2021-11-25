#include "Room3DIntroVideoDlg.h"
#include "Define.h"
#include <QMessageBox>
#include "Common.h"
#include "Room3DApplication.h"
#include "Room3DUtility.h"


CRoom3DIntroVideoDlg::CRoom3DIntroVideoDlg(QWidget* a_pParent) : CRoom3DBaseDlg(a_pParent)
{
	QFlag flgs = QFlag(Qt::Window | Qt::FramelessWindowHint);
	this->setWindowFlags(flgs);
	InitMediaObject();
	this->setWindowTitle(g_pApp->GetMainWidget()->windowTitle());
}

CRoom3DIntroVideoDlg::~CRoom3DIntroVideoDlg(void)
{
	SAFE_DELETE(m_pVerticalLayout);
#ifdef _SUPPORT_PHONON_MOVIE
	SAFE_DELETE(m_pMediaObject);
	SAFE_DELETE(m_pAudioOutput);
	SAFE_DELETE(m_pVideoWidget);
#endif
}

void CRoom3DIntroVideoDlg::InitMediaObject()
{
	m_pVerticalLayout = new QVBoxLayout(this);
	m_pVerticalLayout->setSpacing(0);
	m_pVerticalLayout->setContentsMargins(0, 0, 0, 0);
#ifdef _SUPPORT_PHONON_MOVIE
	m_pMediaObject = new Phonon::MediaObject();
	m_pVideoWidget = new Phonon::VideoWidget(this);
	m_pVideoWidget->setObjectName(QString::fromUtf8("m_pVideoWidget"));
	m_pVerticalLayout->addWidget(m_pVideoWidget);
	m_pAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory);
	connect(m_pMediaObject, SIGNAL(finished()), this, SLOT(Finished()));
	connect(m_pMediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(StateChanged(Phonon::State, Phonon::State)));
	Phonon::createPath(m_pMediaObject, m_pAudioOutput);
	Phonon::createPath(m_pMediaObject, m_pVideoWidget);
	m_pVideoWidget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
#endif

	m_pSkipBtn = new CRoom3DImageButton(BUTTON_IMG_SKIP, 0, Qt::PointingHandCursor, this);
	QBitmap maskButtonBmp = QBitmap(BUTTON_MASK_SKIP);
	m_pSkipBtn->setMask(maskButtonBmp);
	QRect maskRect = maskButtonBmp.rect();
	m_pSkipBtn->setGeometry( maskRect );

	connect(m_pSkipBtn, SIGNAL(pressed(int)), this, SLOT(OnPressedBtn(int)));
}

void CRoom3DIntroVideoDlg::Play(const QString & a_sMediaPath, int a_nVolume)
{
	if( false == signCheck(a_sMediaPath))
	{
		QMessageBox::warning(NULL, tr("IntroVideo"), tr("Couldn't find video file or the video file isn't unsupported."));
		emit OnFinished();
		return;
	}
#ifdef _SUPPORT_PHONON_MOVIE
	m_pMediaObject->setCurrentSource(Phonon::MediaSource(a_sMediaPath));
	m_pAudioOutput->setVolume(a_nVolume);
	m_pMediaObject->play();
#endif
}
void CRoom3DIntroVideoDlg::Finished()
{
#ifdef _SUPPORT_PHONON_MOVIE
	if(m_pMediaObject->state() == Phonon::PlayingState)
		m_pMediaObject->stop();
#endif
	this->hide();
	emit OnFinished();
}

void CRoom3DIntroVideoDlg::StateChanged(Phonon::State a_NewState, Phonon::State a_OldState)
{
#ifdef _SUPPORT_PHONON_MOVIE
	if (a_OldState == Phonon::LoadingState) {
		g_pApp->processEvents();
		QRect deskTopRect = QApplication::desktop()->screenGeometry();
		resize(QSize(INTRO_VIDEO_WIDTH, INTRO_VIDEO_HEIGHT));
		QRect targetRect((deskTopRect.width() - INTRO_VIDEO_WIDTH) / 2 , (deskTopRect.height() - INTRO_VIDEO_HEIGHT) / 2 , INTRO_VIDEO_WIDTH, INTRO_VIDEO_HEIGHT);
		setGeometry(targetRect);
#ifdef DEBUG
		this->show();
#else
		this->showFullScreen();
#endif
	}
	switch (a_NewState) {
		 case Phonon::ErrorState:
			 {
				 m_pMediaObject->stop();
				 Finished();
				 break;
			 }
			 break;
	}
#endif
}

void CRoom3DIntroVideoDlg::OnPressedBtn(int a_nBtnID)
{
	switch(a_nBtnID)
	{
	case 0:
		Finished();
		break;
	}
}

void CRoom3DIntroVideoDlg::resizeEvent( QResizeEvent *e )
{
	QSize dlgSize = this->size();
	m_pSkipBtn->move( dlgSize.rwidth() - 60 ,dlgSize.rheight() - 60 );
// 	m_pSkipBtn->

}