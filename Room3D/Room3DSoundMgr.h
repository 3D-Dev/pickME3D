#pragma once
#include <QObject>
#include <Phonon\MediaObject>
#include <Phonon\AudioOutput>

enum SOUND_ID
{
	SOUND_NONE,
	SOUND_BUTTON
};

class CRoom3DSound : public QObject
{
	Q_OBJECT
public:
	CRoom3DSound(SOUND_ID a_nSoundID);
	CRoom3DSound(QString a_sMediaSource);
	~CRoom3DSound();
	void Play(int a_nVolume, bool a_blLooping);
	void Stop();
	bool IsPlaying();
	SOUND_ID GetID(){return m_nID;}
signals:
	void finished(CRoom3DSound* a_pSound);
protected slots:
	void StateChanged(Phonon::State a_NewState, Phonon::State a_OldState);
protected:
	void OnFinished();
	void CreateMediaObjects();
	QString GetMediaSourcePath(SOUND_ID a_nSoundID);

protected:
	SOUND_ID		m_nID;
	QString			m_sMediaSource;
	Phonon::MediaObject* m_pMediaObject;
	Phonon::AudioOutput *m_pAudioOutput;
};

class CRoom3DSoundMgr : public QObject
{
	Q_OBJECT
public:
	CRoom3DSoundMgr(void);
	~CRoom3DSoundMgr(void);
	void Play(SOUND_ID a_soundID, int a_nVolume = 100, bool a_blLooping = false);
	void Play(QString a_sSoundPath, int a_nVolume = 100, bool a_blLooping = false);
protected slots:
	void ClearNonPlayingSounds();
protected:
	CRoom3DSound* Find(SOUND_ID a_soundID);
protected:
	QVector<CRoom3DSound*> m_sounds;
};
