#ifndef COMMON_H
#define COMMON_H

#include "pd_api.h"
#include "Defines.h"
#include "CWorldParts.h"

typedef enum GameStates GameStates;
enum GameStates {GSTitleScreen,GSCredits,GSIntro,GSGame,GSStageSelect,GSLevelEditor,GSLevelEditorMenu, GSOptions,
    GSTitleScreenInit, GSCreditsInit, GSIntroInit, GSGameInit, GSStageSelectInit, GSLevelEditorInit, GSLevelEditorMenuInit, GSOptionsInit
};

extern LCDBitmap *IMGBackground, *IMGFloor, *IMGPlayer, *IMGBox, *IMGEmpty, *IMGTitleScreen, *IMGWall,
            *IMGIntro1, *IMGIntro2, *IMGIntro3, *IMGBomb, *IMGDiamond, *IMGExplosion;

extern LCDFont* font,* BigFont,* RobotoMedium;
extern GameStates GameState;
extern int SelectedMusic, InstalledLevelPacksCount, InstalledLevels, SelectedLevel;
extern int SelectedLevelPack, UnlockedLevels, GetStringId, GetStringX, GetStringY, AskingQuestionID;
extern bool LevelEditorMode, LevelHasChanged, StageReload, AskingGetString, AskingQuestion, showFPS;
extern size_t MaxLenGetString;
extern CWorldParts *WorldParts;
extern int StartScreenX, StartScreenY;
extern char NormalCreateName[MaxLevelPackNameLength];
extern char LevelPackFileName[MaxLevelPackNameLength];
extern char InstalledLevelPacks[MaxLevelPacks][MaxLevelPackNameLength];
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern bool paused;
extern char* LevelPackName;
extern PDButtons currButtons, prevButtons;
extern int Selection;
extern unsigned long framecount, frameUpStart, frameDownStart, frameLeftStart, frameRightStart, frameAStart;
extern CWorldPart *Player;
#endif