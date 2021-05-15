#include "stdafx.h"
#include "Sound.h"

CSoundManager::CSoundManager()
{
	FMOD_RESULT       result; 
	void* extradriverdata(nullptr);
	result = FMOD::System_Create(&m_pSystem); 
	//if (result != FMOD_OK) return;                
	// ù��°�� �� �� !! �ý��� ��ü�� ���� �������Ѿ� �Ѵ�.

	result = m_pSystem->getVersion(&version);

	result = m_pSystem->init(32, FMOD_INIT_NORMAL, extradriverdata); 
	// �ý��� �ʱ�ȭ : chaneel 32������ ����ϰڴ�.
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

		//���� ��� �ϵ��� �����մϴ�. 
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

		//���� ������� �ʵ��� �����մϴ�.
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
		// ���. �� �̶� �� �ѹ��� ����ǹǷ� ����� ���尡 ������ ������� �ʴ´�.  ���ѷ��� �ȿ��� �ý��� ��ü�� ��~~�� ������Ʈ ������� ��.
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
		return NULL;		//ã�� ���� ���ٸ� NULL�� ��ȯ�մϴ�.
	}
	else
	{
		//return  m_mapSound.find(key)->second;
		return iter->second;//���� ã�Ҵٸ� ã�� ���� ��ȯ�մϴ�.
	}
}