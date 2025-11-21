#include <stdbool.h>
#include "pd_api.h"
#include "Pd_helperfuncs.h"
#include "Common.h"
#include "Intro.h"
#include "CWorldParts.h"
#include "GameFuncs.h"
#include "StageSelect.h"
#include "Credits.h"
#include "TitleScreen.h"
#include "LevelEditor.h"
#include "LevelEditorMenu.h"
#include "Game.h"
#include "Sound.h"
#include "Options.h"

void setupGame()
{
	//Reset Globals first

	GameState = GSIntroInit;
	Selection = 0;
	InstalledLevelPacksCount = 0;
	InstalledLevels = 0;
	SelectedLevel = 0;
	SelectedLevelPack = 0;
	UnlockedLevels = 1;
	GetStringId = -1;
	GetStringX = 0;
	GetStringY = 0;
	LevelEditorMode = false;
	LevelHasChanged = false;
	StageReload = false;
	LevelPackName = NULL;
	AskingQuestionID = -1;
	MaxLenGetString = 0;
	AskingQuestion = false;
	AskingGetString = false;
	showFPS = false;
	paused = false;
	framecount = 0;
	//normal game stuff

	srand(pd->system->getCurrentTimeMilliseconds());
	font = loadFontAtPath("fonts/Roboto-Black-12");
	BigFont = loadFontAtPath("fonts/Roboto-Black-16");
	RobotoMedium = loadFontAtPath("fonts/Roboto-Black-24");
	WorldParts = CWorldParts_Create();

	//always create levelpack folder in data folder so people can find it
	pd->file->mkdir("levelpacks");

	SearchForLevelPacks();
	FindLevels();
	initSound();
	initMusic();
	LoadSettings();
	LoadGraphics();
}

void terminateGame()
{
	SaveSettings();
	UnLoadGraphics();
	deInitSound();
	deInitMusic();
	font = NULL;
	BigFont = NULL;
	RobotoMedium = NULL;
	CWorldParts_RemoveAll(WorldParts);
	CWorldParts_Destroy(WorldParts);
}


int mainLoop(void* ud)
{
	if (paused)
		return 1;
	prevButtons = currButtons;
	pd->system->getButtonState(&currButtons, NULL, NULL);
	switch (GameState)
	{
		case GSIntroInit:
		case GSIntro:
			Intro();
			break;
		case GSTitleScreenInit:
		case GSTitleScreen:
			TitleScreen();
			break;
		case GSCreditsInit:
		case GSCredits:
			Credits();
			break;
		case GSGameInit:
		case GSGame:
			Game();
			break;
		case GSStageSelectInit:
		case GSStageSelect:
			StageSelect();
			break;
		case GSLevelEditorInit:
		case GSLevelEditor:
			LevelEditor();
			break;
		case GSLevelEditorMenuInit:
		case GSLevelEditorMenu:
			LevelEditorMode = true;
			LevelEditorMenu();
			break;
		case GSOptionsInit:
		case GSOptions:
			Options();
			break;
	}

	if (showFPS)
		pd->system->drawFPS(0, 0);
	framecount++;
	return 1;
}