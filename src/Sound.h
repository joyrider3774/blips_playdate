#ifndef SOUND_H
#define SOUND_H

#include "pd_api.h"

#define musTitle 1

#define SFX_SUSTAIN 100

void initSound(void);
void SelectMusic(int musicFile);
void initMusic(void);
void playMenuSound(void);
void playMenuSelectSound(void);
void playErrorSound(void);
void playLevelDoneSound(void);
void playMenuBackSound(void);
void playGameMoveSound(void);
void playGameCollectSound(void);
void playGameExplodeSound(void);
void setMusicOn(int value);
void setSoundOn(int value);
int isMusicOn(void);
int isSoundOn(void);
void stopMusic(void);
void deInitMusic(void);
void deInitSound(void);
SamplePlayer* loadSoundFile(AudioSample* Sample, const char* path);



#endif