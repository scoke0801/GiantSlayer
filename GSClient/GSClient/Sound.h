#pragma once

#pragma comment (lib, "fmodex_vc.lib")

#include <fmod.hpp>
#include <fmod_errors.h>


/*
���� ��� ���õ� ������ ������ Ŭ�����Դϴ�.

�⺻������ BGM, EFFECT�� ������ �����ϵ��� �ϴµ�,
BGM�� AddStream�� ���� �߰��ϵ��� �ϸ�
�ٸ� ����� AddSound�� ���� �߰��ϵ��� �մϴ�.

CFrameWork�� ��� ������ CSoundManager Ŭ������ �߰��Ͽ�
�ʿ��� ��쿡 ������ ��ũ�� ���ؼ� �����ϵ��� �մϴ�.

�Ǵ�
CScene�� ��� ������ CSoundManager Ŭ������ �߰��Ͽ�
�ʿ��� ��쿡 �� ��鿡�� �����ϵ��� �մϴ�.

�߰����� �κ��� ���߿�...
*/

typedef enum
{
	SOUND_BGM = 0
	, SOUND_EFFECT
	, SOUND_COUNT
}SoundType;

enum class Sound_Name : int
{
	BGM_TITLE = 0
	, BGM_MAIN_GAME
	, BGM_LOBBY
	, BGM_GAME_LOSE
	, BGM_GAME_WIN
	, EFFECT_BOMB_SET
	, EFFECT_BOMB_POP
	, EFFECT_BOMB_WAVE

};

class CSoundManager
{
public:		//������, �Ҹ���
	CSoundManager();
	~CSoundManager();

public:		//���� �߰�
	void AddStream(char*  szPath, Sound_Name varName);	//��������� �߰��մϴ�.
	void AddSound(char*  szPath, Sound_Name varName);	//ȿ������ �߰��մϴ�.

public:		//���� ���
	void OnUpdate();					//���� ����� ������ �ʵ��� �մϴ�.

	void PlayBgm(Sound_Name varName);		//��������� ����մϴ�.
	void PlayEffect(Sound_Name varName);	//ȿ������ ����մϴ�.
	
	void Stop();		//���� ����� �����մϴ�.
	void SetVolume(float volume);	//������ ũ�⸦ �����մϴ�.
public:		//��Ÿ
	FMOD_SOUND* FindSound(Sound_Name key);	//�ش� Ű ���� �̹� ��� �Ǿ� �ִ����� ã���ϴ�.
private:
	float m_fVolume;	//���� ����� ���� ũ�⸦ �����մϴ�.

	FMOD_SYSTEM *  m_pSystem;
	FMOD_CHANNEL * m_pChannel[SoundType::SOUND_COUNT];

	std::map <Sound_Name, FMOD_SOUND* > m_mapSound;
};