#include <QtGui>
#include "Room3DUtility.h"

#define SLIDER_RANGE 8

#include "Room3DTutorialVideoDlg.h"
CRoom3DTutorialVideoDlg::CRoom3DTutorialVideoDlg(const QString &filePath,QWidget* a_pParent) :
playButton(0), m_AudioOutput(Phonon::VideoCategory), m_videoWidget(new Phonon::VideoWidget(this)),
CRoom3DBaseDlg(a_pParent)
{
	QSize buttonSize(34, 28);
	QPalette bpal;
	QColor arrowcolor = bpal.buttonText().color();
	if (arrowcolor == Qt::black)
		arrowcolor = QColor(80, 80, 80);
	bpal.setBrush(QPalette::ButtonText, arrowcolor);

	rewindButton = new QPushButton(this);
	rewindButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

	forwardButton = new QPushButton(this);
	forwardButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
	forwardButton->setEnabled(false);

	playButton = new QPushButton(this);
	playIcon = style()->standardIcon(QStyle::SP_MediaPlay);
	pauseIcon = style()->standardIcon(QStyle::SP_MediaPause);
	playButton->setIcon(playIcon);

	slider = new Phonon::SeekSlider(this);
	slider->setMediaObject(&m_MediaObject);
	volume = new Phonon::VolumeSlider(&m_AudioOutput);

	QVBoxLayout *vLayout = new QVBoxLayout(this);
	vLayout->setContentsMargins(8, 8, 8, 8);

	QHBoxLayout *layout = new QHBoxLayout();


	volume->setFixedWidth(120);

	layout->addWidget(rewindButton);
	layout->addWidget(playButton);
	layout->addWidget(forwardButton);

	layout->addStretch();
	layout->addWidget(volume);

	initVideoWindow();
	vLayout->addWidget(&m_videoWindow);
	QVBoxLayout *buttonPanelLayout = new QVBoxLayout();
	m_videoWindow.hide();
	buttonPanelLayout->addLayout(layout);

	timeLabel = new QLabel(this);
	progressLabel = new QLabel(this);
	QWidget *sliderPanel = new QWidget(this);
	QHBoxLayout *sliderLayout = new QHBoxLayout();
	sliderLayout->addWidget(slider);
	sliderLayout->addWidget(timeLabel);
	sliderLayout->addWidget(progressLabel);
	sliderLayout->setContentsMargins(0, 0, 0, 0);
	sliderPanel->setLayout(sliderLayout);

	buttonPanelLayout->addWidget(sliderPanel);
	buttonPanelLayout->setContentsMargins(0, 0, 0, 0);

	QWidget *buttonPanelWidget = new QWidget(this);
	buttonPanelWidget->setMaximumHeight(80);
	buttonPanelWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	buttonPanelWidget->setLayout(buttonPanelLayout);
	vLayout->addWidget(buttonPanelWidget);

	setLayout(vLayout);

	// Setup signal connections:
	connect(rewindButton, SIGNAL(clicked()), this, SLOT(rewind()));
	connect(playButton, SIGNAL(clicked()), this, SLOT(playPause()));
	connect(forwardButton, SIGNAL(clicked()), this, SLOT(forward()));
	//connect(openButton, SIGNAL(clicked()), this, SLOT(openFile()));

	connect(&m_MediaObject, SIGNAL(totalTimeChanged(qint64)), this, SLOT(updateTime()));
	connect(&m_MediaObject, SIGNAL(tick(qint64)), this, SLOT(updateTime()));
	connect(&m_MediaObject, SIGNAL(finished()), this, SLOT(finished()));
	connect(&m_MediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(stateChanged(Phonon::State, Phonon::State)));
	connect(&m_MediaObject, SIGNAL(bufferStatus(int)), this, SLOT(bufferStatus(int)));
	connect(&m_MediaObject, SIGNAL(hasVideoChanged(bool)), this, SLOT(hasVideoChanged(bool)));

	rewindButton->setEnabled(false);
	playButton->setEnabled(false);

	m_audioOutputPath = Phonon::createPath(&m_MediaObject, &m_AudioOutput);
	Phonon::createPath(&m_MediaObject, m_videoWidget);

	resize(QSize(600, 480));
	setFile(filePath);
	m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);

	setWindowTitle(tr("Tutorial"));
	setModal(false);
	//QFlag flgs = QFlag(Qt::Window | Qt::FramelessWindowHint);
	//this->setWindowFlags(flgs);
}

void CRoom3DTutorialVideoDlg::stateChanged(Phonon::State newstate, Phonon::State oldstate)
{
	Q_UNUSED(oldstate);

	if (oldstate == Phonon::LoadingState) {
	}

	switch (newstate) {
	case Phonon::ErrorState:
		QMessageBox::warning(this, tr("Phonon Mediaplayer"), m_MediaObject.errorString(), QMessageBox::Close);
		if (m_MediaObject.errorType() == Phonon::FatalError) {
			playButton->setEnabled(false);
			rewindButton->setEnabled(false);
		} else {
			m_MediaObject.pause();
		}
		break;
	case Phonon::PausedState:
	case Phonon::StoppedState:
		playButton->setIcon(playIcon);
		if (m_MediaObject.currentSource().type() != Phonon::MediaSource::Invalid){
			playButton->setEnabled(true);
			rewindButton->setEnabled(true);
		} else {
			playButton->setEnabled(false);
			rewindButton->setEnabled(false);
		}
		break;
	case Phonon::PlayingState:
		playButton->setEnabled(true);
		playButton->setIcon(pauseIcon);
		if (m_MediaObject.hasVideo())
			m_videoWindow.show();
		// Fall through
	case Phonon::BufferingState:
		rewindButton->setEnabled(true);
		break;
	case Phonon::LoadingState:
		rewindButton->setEnabled(false);
		break;
	}

}

void CRoom3DTutorialVideoDlg::initVideoWindow()
{
	QVBoxLayout *videoLayout = new QVBoxLayout();
	videoLayout->addWidget(m_videoWidget);
	videoLayout->setContentsMargins(0, 0, 0, 0);
	m_videoWindow.setLayout(videoLayout);
	m_videoWindow.setMinimumSize(100, 100);
}

void CRoom3DTutorialVideoDlg::playPause()
{
	if (m_MediaObject.state() == Phonon::PlayingState)
		m_MediaObject.pause();
	else {
		if (m_MediaObject.currentTime() == m_MediaObject.totalTime())
			m_MediaObject.seek(0);
		m_MediaObject.play();
	}
}

void CRoom3DTutorialVideoDlg::hideEvent( QHideEvent * event )
{
	m_MediaObject.stop();
}

void CRoom3DTutorialVideoDlg::setFile(const QString &fileName)
{
	m_MediaObject.setCurrentSource(Phonon::MediaSource(fileName));
	m_sFilePath = fileName;
	m_MediaObject.play();
}

void CRoom3DTutorialVideoDlg::Play()
{
	if( false == signCheck(m_sFilePath))
	{
		QMessageBox::warning(NULL, tr("TutorialVideo"), tr("Couldn't find video file or the video file isn't unsupported."));
		return;
	}
	this->show();
	rewind();
	m_MediaObject.play();
}

void CRoom3DTutorialVideoDlg::bufferStatus(int percent)
{
	if (percent == 0 || percent == 100)
		progressLabel->setText(QString());
	else {
		QString str = QString::fromLatin1("(%1%)").arg(percent);
		progressLabel->setText(str);
	}
}

void CRoom3DTutorialVideoDlg::updateTime()
{
	long len = m_MediaObject.totalTime();
	long pos = m_MediaObject.currentTime();
	QString timeString;
	if (pos || len)
	{
		int sec = pos/1000;
		int min = sec/60;
		int hour = min/60;
		int msec = pos;

		QTime playTime(hour%60, min%60, sec%60, msec%1000);
		sec = len / 1000;
		min = sec / 60;
		hour = min / 60;
		msec = len;

		QTime stopTime(hour%60, min%60, sec%60, msec%1000);
		QString timeFormat = "m:ss";
		if (hour > 0)
			timeFormat = "h:mm:ss";
		timeString = playTime.toString(timeFormat);
		if (len)
			timeString += " / " + stopTime.toString(timeFormat);
	}
	timeLabel->setText(timeString);
}

void CRoom3DTutorialVideoDlg::rewind()
{
	m_MediaObject.seek(0);
}

void CRoom3DTutorialVideoDlg::forward()
{
	QList<Phonon::MediaSource> queue = m_MediaObject.queue();
	if (queue.size() > 0) {
		m_MediaObject.setCurrentSource(queue[0]);
		forwardButton->setEnabled(queue.size() > 1);
		m_MediaObject.play();
	}
}

void CRoom3DTutorialVideoDlg::finished()
{
}

void CRoom3DTutorialVideoDlg::scaleChanged(QAction *act)
{
	if (act->text() == tr("Scale and crop"))
		m_videoWidget->setScaleMode(Phonon::VideoWidget::ScaleAndCrop);
	else
		m_videoWidget->setScaleMode(Phonon::VideoWidget::FitInView);
}

void CRoom3DTutorialVideoDlg::aspectChanged(QAction *act)
{
	if (act->text() == tr("16/9"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio16_9);
	else if (act->text() == tr("Scale"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioWidget);
	else if (act->text() == tr("4/3"))
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatio4_3);
	else
		m_videoWidget->setAspectRatio(Phonon::VideoWidget::AspectRatioAuto);
}

void CRoom3DTutorialVideoDlg::hasVideoChanged(bool bHasVideo)
{
	m_videoWindow.setVisible(bHasVideo);
}