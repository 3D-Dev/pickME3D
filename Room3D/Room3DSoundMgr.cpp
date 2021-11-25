#include "Room3DSoundMgr.h"
#include "Define.h"
#include "Room3DApplication.h"

CRoom3DSound::CRoom3DSound(QString a_sMediaSource)
{
	m_nID = SOUND_NONE;
	m_sMediaSource = a_sMediaSource;
	CreateMediaObjects();
	m_pMediaObject->setCurrentSource(m_sMediaSource);
}

CRoom3DSound::CRoom3DSound(SOUND_ID a_nSoundID)
{
	CreateMediaObjects();
	m_nID = a_nSoundID;
	QString sMediaSource = GetMediaSourcePath(m_nID);
	m_sMediaSource = sMediaSource;
	m_pMediaObject->setCurrentSource(m_sMediaSource);
}

CRoom3DSound::~CRoom3DSound()
{
	Stop();
	SAFE_DELETE(m_pMediaObject);
	SAFE_DELETE(m_pAudioOutput);
}

void CRoom3DSound::CreateMediaObjects()
{
	m_pMediaObject = new Phonon::MediaObject();
	m_pAudioOutput = new Phonon::AudioOutput(Phonon::VideoCategory);
	Phonon::createPath(m_pMediaObject, m_pAudioOutput);
	connect(m_pMediaObject, SIGNAL(finished()), this, SLOT(OnFinished()));
	connect(m_pMediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(StateChanged(Phonon::State, Phonon::State)));
}

QString CRoom3DSound::GetMediaSourcePath(SOUND_ID a_nSoundID)
{
	QString sPath("");
	switch(a_nSoundID)
	{
	case SOUND_BUTTON:
		sPath = g_pApp->GetResourcePath(BUTTON_SOUND);
		break;
	}
	return sPath;
}

void CRoom3DSound::Play(int a_nVolume, bool a_blLooping)
{
	Stop();
	a_nVolume = max(0, min(a_nVolume, 100));
	float fVolume = a_nVolume / 100.f;
	m_pAudioOutput->setVolume(fVolume);
	m_pMediaObject->setCurrentSource(m_sMediaSource);
	m_pMediaObject->play();
}

void CRoom3DSound::Stop()
{
	if(m_pMediaObject->state() == Phonon::PlayingState)
		m_pMediaObject->stop();

}

bool CRoom3DSound::IsPlaying()
{
	if(m_pMediaObject->state() == Phonon::PlayingState)
		return true;
	else
		return false;
}

void CRoom3DSound::OnFinished()
{
	Stop();
	emit finished(this);
}

void CRoom3DSound::StateChanged(Phonon::State a_NewState, Phonon::State a_OldState)
{
	if (a_OldState == Phonon::LoadingState) {
	}
	switch (a_NewState) {
		 case Phonon::ErrorState:
			 {
				 OnFinished();
				 break;
			 }
			 break;
	}
}

CRoom3DSoundMgr::CRoom3DSoundMgr(void)
{
}

CRoom3DSoundMgr::~CRoom3DSoundMgr(void)
{
	QVector<CRoom3DSound*>::iterator it = m_sounds.begin();
	while(it != m_sounds.end())
	{
		delete (*it);
		it++;
	}
	m_sounds.clear();
}

void CRoom3DSoundMgr::Play(SOUND_ID a_soundID, int a_nVolume, bool a_blLooping)
{
	ClearNonPlayingSounds();
	CRoom3DSound* pSound = Find(a_soundID);
	if(pSound == NULL)
	{
		pSound = new CRoom3DSound(a_soundID);
		m_sounds.push_back(pSound);
	}
	if(pSound)
	{
		pSound->Play(a_nVolume, a_blLooping);
	}
}

void CRoom3DSoundMgr::Play(QString a_sSoundPath, int a_nVolume, bool a_blLooping)
{
	ClearNonPlayingSounds();
	CRoom3DSound* pSound = new CRoom3DSound(a_sSoundPath);
	if(pSound)
	{
		pSound->Play(a_nVolume, a_blLooping);
		m_sounds.push_back(pSound);
	}
}

CRoom3DSound* CRoom3DSoundMgr::Find(SOUND_ID a_soundID)
{
	QVector<CRoom3DSound*>::iterator it = m_sounds.begin();
	while(it != m_sounds.end())
	{
		if((*it)->GetID() == a_soundID)
			return (*it);
		it++;
	}
	return NULL;
}

void CRoom3DSoundMgr::ClearNonPlayingSounds()
{
	int nCount = m_sounds.count();
	for(int i = 0; i < nCount; i++)
	{
		if(m_sounds[i]->GetID() == SOUND_NONE && m_sounds[i]->IsPlaying() == false)
		{
			m_sounds.remove(i);
			i--;
			nCount--;
		}
	}
}

