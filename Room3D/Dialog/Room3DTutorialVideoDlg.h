#pragma once
#include <QtGui/QWidget>
#include <QtGui/QApplication>
#include <QtCore/QTimerEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QIcon>

#include <Phonon/AudioOutput>
#include <Phonon/BackendCapabilities>
#include <Phonon/Effect>
#include <Phonon/EffectParameter>
#include <Phonon/EffectWidget>
#include <Phonon/MediaObject>
#include <Phonon/SeekSlider>
#include <Phonon/VideoWidget>
#include <Phonon/VolumeSlider>
#include "Room3DBaseDlg.h"

class QPushButton;
class QLabel;
class QSlider;
class QTextEdit;

class CRoom3DTutorialVideoDlg :
	public CRoom3DBaseDlg
{
	Q_OBJECT
public:
	CRoom3DTutorialVideoDlg(const QString &, QWidget* a_pParent = NULL);

	void setFile(const QString &text);
	void initVideoWindow();
	void Play();
	public slots:
		void rewind();
		void forward();
		void updateTime();
		void finished();
		void playPause();
		void scaleChanged(QAction *);
		void aspectChanged(QAction *);

		private slots:
			void stateChanged(Phonon::State newstate, Phonon::State oldstate);
			void bufferStatus(int percent);
			void hasVideoChanged(bool);
protected:
	virtual void hideEvent( QHideEvent * event );

private:
	QIcon playIcon;
	QIcon pauseIcon;
	QPushButton *playButton;
	QPushButton *rewindButton;
	QPushButton *forwardButton;
	Phonon::SeekSlider *slider;
	QLabel *timeLabel;
	QLabel *progressLabel;
	Phonon::VolumeSlider *volume;
	QSlider *m_hueSlider;
	QSlider *m_satSlider;
	QSlider *m_contSlider;

	QWidget m_videoWindow;
	Phonon::MediaObject m_MediaObject;
	Phonon::AudioOutput m_AudioOutput;
	Phonon::VideoWidget *m_videoWidget;
	Phonon::Path m_audioOutputPath;
	QString m_sFilePath;
};