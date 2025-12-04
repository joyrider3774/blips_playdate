#include "pd_api.h"
#include "SPoint.h"
#include "Defines.h"
#include "Common.h"
#include "stdbool.h"

LCDBitmap *IMGBackground=NULL,*IMGFloor=NULL,*IMGPlayer=NULL,*IMGPlayer2=NULL,*IMGBox=NULL,*IMGEmpty=NULL,*IMGTitleScreen=NULL,*IMGWall=NULL,
            *IMGIntro1=NULL,*IMGIntro2=NULL,*IMGIntro3=NULL,*IMGBomb=NULL,*IMGDiamond=NULL,*IMGExplosion=NULL;

LCDFont* font,* BigFont,* RobotoMedium;
GameStates GameState = GSTitleScreenInit;
int SelectedMusic = 0, InstalledLevelPacksCount = 0, InstalledLevels = 0, SelectedLevel = 0, SelectedLevelPack = 0;
int UnlockedLevels = 1, GetStringId = -1, GetStringX = 0, GetStringY = 0, AskingQuestionID = -1;
bool LevelEditorMode=false,LevelHasChanged=false,StageReload=false, AskingGetString=false, AskingQuestion=false, showFPS = false;
size_t MaxLenGetString = 0;
CWorldParts *WorldParts;
char *LevelPackName = NULL;
char LevelPackFileName[MaxLevelPackNameLength] = "";
char InstalledLevelPacks[MaxLevelPacks][MaxLevelPackNameLength];
int WINDOW_WIDTH = 400;
int WINDOW_HEIGHT = 240;
char NormalCreateName[MaxLevelPackNameLength];
bool paused = false;
unsigned long framecount = 0, frameUpStart = 0, frameDownStart = 0, frameLeftStart = 0, frameRightStart = 0, frameAStart = 0;
PDButtons currButtons, prevButtons;
int Selection = 0;