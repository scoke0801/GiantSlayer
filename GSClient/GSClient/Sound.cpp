#include "stdafx.h"
#include "Sound.h"

CSoundManager::CSoundManager()
{
	FMOD_RESULT       result; 
	void* extradriverdata(nullptr);
	result = FMOD::System_Create(&m_pSystem); 
	//if (result != FMOD_OK) return;                
	// 첫번째로 꼭 꼭 !! 시스템 객체를 먼저 생성시켜야 한다.

	result = m_pSystem->getVersion(&version);

	result = m_pSystem->init(32, FMOD_INIT_NORMAL, extradriverdata); 
	// 시스템 초기화 : chaneel 32개까지 사용하겠다.
	//if (result != FMOD_OK) return;

	m_fVolume = 0.5f;
}

CSoundManager::~CSoundManager()
{
	m_mapSound.clear(); 

	m_pSystem->close();
	m_pSystem->release();
}

void CSoundManager::AddStream(char*  szPath, Sound_Name varName)
{
	if (FindSound(varName) == NULL)
	{
		FMOD::Sound* stream;

		//연속 재생 하도록 설정합니다. 
		FMOD_RESULT result = m_pSystem->createStream(szPath, FMOD_DEFAULT & ~FMOD_LOOP_OFF | FMOD_LOOP_NORMAL, 0, &stream); 
		
		if (stream)
		{
			m_mapSound[varName] = stream;
			m_mapSound.insert(std::pair <Sound_Name, FMOD::Sound* >(varName, stream));
		}
	}
}

void CSoundManager::AddSound(char * szPath, Sound_Name varName)
{
	if (FindSound(varName) == NULL)
	{
		FMOD::Sound* sound;

		//연속 재생하지 않도록 설정합니다.
		FMOD_RESULT result = m_pSystem->createStream(szPath, FMOD_LOOP_OFF, 0, &sound);
		
		if (sound)
		{
			m_mapSound[varName] = sound;
			m_mapSound.insert(std::pair <Sound_Name, FMOD::Sound* >(varName, sound));
		}
	}
}

void CSoundManager::OnUpdate()
{
	m_pSystem->update(); 
}

void CSoundManager::PlayBgm(Sound_Name varName)
{
	if (FindSound(varName) != NULL)
	{
		m_pChannel[SOUND_BGM]->stop();  
		// 재생. 단 이때 딱 한번만 실행되므로 제대로 사운드가 끝까지 재생되지 않는다.  무한루프 안에서 시스템 객체를 계~~속 업데이트 시켜줘야 함.
		m_pSystem->playSound(m_mapSound[varName], 0, FALSE, &m_pChannel[SOUND_BGM]); 
		m_pChannel[SOUND_BGM]->setVolume(m_fVolume); 
	}
	
}

void CSoundManager::PlayEffect(Sound_Name varName)
{
	if (FindSound(varName))
	{
		if (FMOD_OK == m_pSystem->getChannelsPlaying(NULL, NULL))
		{
			
			m_pSystem->playSound(m_mapSound[varName], 0, FALSE, &m_pChannel[SOUND_EFFECT]);  
			m_pChannel[SOUND_EFFECT]->setVolume(m_fVolume); 
		}
	}
}

void CSoundManager::Stop()
{
	m_pChannel[SOUND_BGM]->stop(); 
}

void CSoundManager::SetVolume(float volume)
{
	m_fVolume = volume;
}

FMOD::Sound* CSoundManager::FindSound(Sound_Name key)
{
	std::map<Sound_Name, FMOD::Sound*>::iterator iter = m_mapSound.find(key);

	if (m_mapSound.find(key) == m_mapSound.end())
	{
		return NULL;		//찾는 값이 없다면 NULL을 반환합니다.
	}
	else
	{
		//return  m_mapSound.find(key)->second;
		return iter->second;//값을 찾았다면 찾은 값을 반환합니다.
	}
}