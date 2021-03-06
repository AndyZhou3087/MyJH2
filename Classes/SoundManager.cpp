#include "SoundManager.h"
#include "SimpleAudioEngine.h"
#include "Resource.h"

using namespace CocosDenshion;

static SoundManager* g_sharedManager = NULL;

SoundManager* SoundManager::getInstance()
{
	//获取instance;
    if(!g_sharedManager)
    {
        g_sharedManager = new SoundManager();
        g_sharedManager->init();

    }
    
    return g_sharedManager;
}

Ref* SoundManager::init()
{
	_isSoundOn = UserDefault::getInstance()->getBoolForKey("issoundon", true);
	_isMusicOn = UserDefault::getInstance()->getBoolForKey("ismusicon", true);
	_isMusicPlayed = false;
    return this;
}

void SoundManager::loadSounds()
{
    // TODO :插入soundid列表
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_BUTTON, ResourcePath::makeSoundPath("button.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_WIN, ResourcePath::makeSoundPath("win.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_FAIL, ResourcePath::makeSoundPath("fail.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_BUILDLVUP, ResourcePath::makeSoundPath("buildlvup.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_MAKERES, ResourcePath::makeSoundPath("makeres.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_DECOMPOSE, ResourcePath::makeSoundPath("decompose.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_ATK, ResourcePath::makeSoundPath("atk.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_HURT, ResourcePath::makeSoundPath("hurt.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_DODGE, ResourcePath::makeSoundPath("dodge.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_CRIT, ResourcePath::makeSoundPath("crit.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_DIE, ResourcePath::makeSoundPath("die.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_BUYRES, ResourcePath::makeSoundPath("buyres.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_TAKEMISSION, ResourcePath::makeSoundPath("takemission.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_FINISHMSSION, ResourcePath::makeSoundPath("finishmission.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_STRENTHOK, ResourcePath::makeSoundPath("strenthok.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_STRENTHFAIL, ResourcePath::makeSoundPath("strenthfail.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_BREAKUPSUCC, ResourcePath::makeSoundPath("breakupsucc.mp3")));
	_soundIds.insert(SoundIdMapType::value_type((int)SOUND_ID_RECRUITSUCC, ResourcePath::makeSoundPath("recruitsucc.mp3")));
	
	
	//插入背景音乐列表
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_START, ResourcePath::makeSoundPath("startbg.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_HOME, ResourcePath::makeSoundPath("homebg.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_MAINMAP, ResourcePath::makeSoundPath("mainmapbg.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_SUBMAP_0, ResourcePath::makeSoundPath("mapbg1.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_SUBMAP_1, ResourcePath::makeSoundPath("mapbg2.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_SUBMAP_2, ResourcePath::makeSoundPath("mapbg3.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_SUBMAP_3, ResourcePath::makeSoundPath("mapbg4.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_SUBMAP_4, ResourcePath::makeSoundPath("mapbg5.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_0, ResourcePath::makeSoundPath("fightbg1.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_1, ResourcePath::makeSoundPath("fightbg2.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_2, ResourcePath::makeSoundPath("fightbg3.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_3, ResourcePath::makeSoundPath("fightbg4.mp3")));
	_musicIds.insert(SoundIdMapType::value_type((int)MUSIC_ID_FIGHT_4, ResourcePath::makeSoundPath("fightbg5.mp3")));

	//预加载音效
	SoundIdMapType::iterator it;
	for (it = _soundIds.begin(); it != _soundIds.end(); it++)
	{
		std::string soundName = it->second;
		SimpleAudioEngine::getInstance()->preloadEffect(soundName.c_str());
	}
}

void SoundManager::unloadSounds()
{
	_soundIds.clear();
	_musicIds.clear();
}

int SoundManager::playSound(int soundId, bool isloop)
{

	if(!_isSoundOn)
	{
	    return -1;
	}
    
    SoundIdMapType::iterator it = _soundIds.find(soundId);
    if (it == _soundIds.end()) {
        return -1;
    }
    
    std::string soundName = it->second;
    SimpleAudioEngine* soundEngine = SimpleAudioEngine::getInstance();
	return soundEngine->playEffect(soundName.c_str(), isloop);
}

bool SoundManager::getSoundIsOn()
{
	return _isSoundOn;
}

bool SoundManager::getMusicIsOn()
{
	return _isMusicOn;
}

bool SoundManager::getMusicPlayed()
{
	return _isMusicPlayed;
}

void SoundManager::playBackMusic(const char * filename)
{
	if (_isMusicOn)
    {
		_isMusicPlayed = true;
        SimpleAudioEngine::getInstance()->playBackgroundMusic(filename,true);
    }
}

void SoundManager::playBackMusic(int musicid)
{
	if (_isMusicOn)
	{
		MusicIdMapType::iterator it = _musicIds.find(musicid);
		if (it == _musicIds.end()) {
			return;
		}

		_isMusicPlayed = true;

		std::string musciName = it->second;
		SimpleAudioEngine::getInstance()->playBackgroundMusic(musciName.c_str(), true);
	}
}

void SoundManager::stopBackMusic()
{
	SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}


void SoundManager::pauseBackMusic()
{
	SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void SoundManager::resumeBackMusic()
{
	if (_isMusicOn)
	{
		SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	}
}

void SoundManager::setIsMusicOn(bool isMusicOn)
{
	_isMusicOn = isMusicOn;
	UserDefault::getInstance()->setBoolForKey("ismusicon", _isMusicOn);
}

void SoundManager::setIsSoundOn(bool isSoundOn)
{
    _isSoundOn = isSoundOn;
	UserDefault::getInstance()->setBoolForKey("issoundon", _isSoundOn);
}

void SoundManager::stopSound(int soundId)
{
	if (_isSoundOn)
		SimpleAudioEngine::getInstance()->stopEffect(soundId);

}
void SoundManager::stopAllEffectSound()
{
	if (_isSoundOn)
		SimpleAudioEngine::getInstance()->stopAllEffects();
}
void SoundManager::pauseAllEffectSound()
{
	if (_isSoundOn)
		SimpleAudioEngine::getInstance()->pauseAllEffects();
}
void SoundManager::resumeAllEffectSound()
{
	if (_isSoundOn)
		SimpleAudioEngine::getInstance()->resumeAllEffects();
}

float SoundManager::getVolume()
{
    return SimpleAudioEngine::getInstance()->getEffectsVolume();
}

void SoundManager::setVolume(float volume)
{
    SimpleAudioEngine* soundEngine = SimpleAudioEngine::getInstance();
    soundEngine->setEffectsVolume(volume);
    soundEngine->setBackgroundMusicVolume(volume);
}

void SoundManager::saveVolume()
{
    UserDefault::getInstance()->setFloatForKey("volume", getVolume());
}

int SoundManager::getSaveVolume()
{
    return UserDefault::getInstance()->getFloatForKey("volume", getVolume());
}
