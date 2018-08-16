/********************************************************************
* 
* ��Ч�����ֲ���
*********************************************************************/

#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include <iostream>
#include "cocos2d.h"
#include <map>

USING_NS_CC;

class CDSoundEngine;

class SoundManager : public Ref
{
	//��ЧID�б�
    typedef std::map<int, std::string> SoundIdMapType;
    
	//��������ID�б�
	typedef std::map<int, std::string> MusicIdMapType;
public:
	//��ЧID
	enum SoundId
	{
		SOUND_ID_BUTTON,//��ť���
		SOUND_ID_WIN,
		SOUND_ID_FAIL,
		SOUND_ID_BUILDLVUP,
		SOUND_ID_MAKERES,
		SOUND_ID_DECOMPOSE,
		SOUND_ID_ATK,
		SOUND_ID_HURT,
		SOUND_ID_DODGE,
		SOUND_ID_CRIT,
		SOUND_ID_DIE,
		SOUND_ID_BUYRES,
		SOUND_ID_TAKEMISSION,
		SOUND_ID_FINISHMSSION,
		SOUND_ID_STRENTHOK,
		SOUND_ID_STRENTHFAIL,
		SOUND_ID_BREAKUPSUCC,
		SOUND_ID_RECRUITSUCC
	};

	//��������
	enum MusicId
	{
		MUSIC_ID_START,//��ʼ����
		MUSIC_ID_HOME,//��
		MUSIC_ID_MAINMAP,//���ͼ����
		MUSIC_ID_SUBMAP_0,//С��ͼ0
		MUSIC_ID_SUBMAP_1,//С��ͼ1
		MUSIC_ID_SUBMAP_2,//С��ͼ2
		MUSIC_ID_SUBMAP_3,//С��ͼ3
		MUSIC_ID_SUBMAP_4,//С��ͼ4
		MUSIC_ID_FIGHT_0,//ս��
		MUSIC_ID_FIGHT_1,
		MUSIC_ID_FIGHT_2,
		MUSIC_ID_FIGHT_3,
		MUSIC_ID_FIGHT_4
	};
	
	//soundmanage ʵ��
    static SoundManager* getInstance();
    
	//��ʼ��
    Ref* init();
    
	//��������
    void loadSounds();
	//ж������
    void unloadSounds();

	/****************************
	���ű�������
	@param filename ���������ļ�����ResourcesĿ¼��(eg:sounds/bg.mp3)
	****************************/
	void playBackMusic(const char * filename);

	/****************************
	���ű�������
	@param musicid ��������id��MusicId
	****************************/
	void playBackMusic(int musicid);

	/****************************
	ֹͣ���ű�������
	****************************/
	void stopBackMusic();

	/****************************
	��ͣ���ű�������
	****************************/
	void pauseBackMusic();

	/****************************
	�ָ����ű�������
	****************************/
	void resumeBackMusic();

	/****************************
	������Ч
	@param soundId ��ЧID,SoundId enum��ֵ
	@param isloop �Ƿ�ѭ����Ĭ�ϲ�ѭ��
	****************************/
	int playSound(int soundId, bool isloop = false);
    
	/****************************
	ֹͣ��Ч
	@param soundId ��ЧID,SoundId enum��ֵ
	****************************/
	void stopSound(int soundId);

	/****************************
	ֹͣ������Ч
	****************************/
	void stopAllEffectSound();

	/****************************
	��ͣ������Ч
	****************************/
	void pauseAllEffectSound();

	/****************************
	�ָ�������Ч
	****************************/
	void resumeAllEffectSound();
    
	/****************************
	������Ч����
	@param isSoundOn ture:����false:��
	****************************/
    void setIsSoundOn(bool isSoundOn);

	/****************************
	���ñ������ֿ���
	@param isSoundOn ture:����false:��
	****************************/
	void setIsMusicOn(bool isMusicOn);
    
	/****************************
	��ȡ�豸��Ч��С
	****************************/
    float getVolume();

	/****************************
	�����豸������С
	@param volume ������С�����100
	****************************/
    void setVolume(float volume);
    
    void saveVolume();
    int getSaveVolume();

	/****************************
	��ȡ��Ч����
	@return  false:�أ�true:��
	****************************/
	bool getSoundIsOn();

	/****************************
	��ȡ�������ֿ���
	@return  false:�أ�true:��
	****************************/
	bool getMusicIsOn();

	/****************************
	��ȡ���������Ƿ����ڲ���
	@return  false:��true:
	****************************/
	bool getMusicPlayed();
private:
	bool                _isSoundOn;//��Ч����
	bool				_isMusicOn;//�������ֿ���
	bool				_isMusicPlayed;//���������Ƿ����ڲ���
    SoundIdMapType      _soundIds;//��ЧID
	MusicIdMapType		_musicIds;//��������ID
};

#endif
