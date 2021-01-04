#include "stdafx.h"
#include "Sound.h"

CSoundManager::CSoundManager()
{
	FMOD_System_Create(&m_pSystem);

	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	m_fVolume = 0.5f;
}

CSoundManager::~CSoundManager()
{
	m_mapSound.clear();
	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}

void CSoundManager::AddStream(char*  szPath, Sound_Name varName)
{
	if (FindSound(varName) == NULL)
	{
		FMOD_SOUND* stream;

		//연속 재생 하도록 설정합니다.
		FMOD_System_CreateStream(m_pSystem, szPath, FMOD_DEFAULT & ~FMOD_LOOP_OFF | FMOD_LOOP_NORMAL, 0, &stream);
		if (stream)
		{
			m_mapSound[varName] = stream;
			m_mapSound.insert(std::pair <Sound_Name, FMOD_SOUND* >(varName, stream));
		}
	}
}

void CSoundManager::AddSound(char * szPath, Sound_Name varName)
{
	if (FindSound(varName) == NULL)
	{
		FMOD_SOUND* sound;

		//연속 재생하지 않도록 설정합니다.
		FMOD_System_CreateSound(m_pSystem, szPath, FMOD_DEFAULT, 0, &sound);
		//FMOD_System_CreateSound(m_pSystem, szPath, FMOD_DEFAULT & ~FMOD_LOOP_OFF | FMOD_LOOP_NORMAL, 0, &sound);
		if (sound)
		{
			m_mapSound[varName] = sound;
			m_mapSound.insert(std::pair <Sound_Name, FMOD_SOUND* >(varName, sound));
		}
	}
}

void CSoundManager::OnUpdate()
{
	FMOD_System_Update(m_pSystem);
}

void CSoundManager::PlayBgm(Sound_Name varName)
{
	if (FindSound(varName) != NULL)
	{
		FMOD_Channel_Stop(m_pChannel[SOUND_BGM]);
		FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, m_mapSound[varName], FALSE, &m_pChannel[SOUND_BGM]);
		FMOD_Channel_SetVolume(m_pChannel[SOUND_BGM], m_fVolume);
	}
	
}

void CSoundManager::PlayEffect(Sound_Name varName)
{
	if (FindSound(varName))
	{
		if (FMOD_System_GetChannelsPlaying(m_pSystem, NULL))
		{
			FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, m_mapSound[varName], FALSE, &m_pChannel[SOUND_EFFECT]);
			FMOD_Channel_SetVolume(m_pChannel[SOUND_EFFECT], m_fVolume);
		}
	}
}

void CSoundManager::Stop()
{
	FMOD_Channel_Stop(m_pChannel[SOUND_BGM]);
}

void CSoundManager::SetVolume(float volume)
{
	m_fVolume = volume;
}

FMOD_SOUND* CSoundManager::FindSound(Sound_Name key)
{
	std::map<Sound_Name, FMOD_SOUND*>::iterator iter = m_mapSound.find(key);

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