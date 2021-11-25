//----------------------------------------------//
//	File: Room3DIntroVideoDlg.h						//
//	Desc: 										//
//	Update: 2012.7  (by 001	)					//
//----------------------------------------------//
#pragma once
#include <Phonon\VideoWidget>
#include <Phonon\MediaObject>
#include <Phonon\AudioOutput>
#include <QVBoxLayout>
#include "Room3DImageButton.h"
#include "Room3DBaseDlg.h"

/*!
	\class CRoom3DIntroVideoDlg
	\brief This is class to play video file.(by 001, 2012/7/7).
	\sa 
*/
class CRoom3DIntroVideoDlg : public CRoom3DBaseDlg
{
	Q_OBJECT
public:
	CRoom3DIntroVideoDlg(QWidget* a_pParent = NULL);
	~CRoom3DIntroVideoDlg(void);
	void Play(const QString & a_sMediaPath, int a_nVolume);
signals:
	void OnFinished();
protected:
	void InitMediaObject();
	void resizeEvent( QResizeEvent *e );
protected slots:
	virtual void Finished();
	void StateChanged(Phonon::State newstate, Phonon::State oldstate);
	void OnPressedBtn(int a_nBtnID);

protected:
	QVBoxLayout* m_pVerticalLayout;
	CRoom3DImageButton* m_pSkipBtn;
#ifdef _SUPPORT_PHONON_MOVIE
	Phonon::MediaObject* m_pMediaObject;
	Phonon::AudioOutput* m_pAudioOutput;
	Phonon::VideoWidget *m_pVideoWidget;
#endif
};
