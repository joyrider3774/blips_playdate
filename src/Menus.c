#include "Menus.h"
#include "GameFuncs.h"
#include "Common.h"
#include "CWorldParts.h"
#include "LevelEditor.h"
#include "StageSelect.h"
#include "Game.h"
#include "pd_api.h"

PDMenuItem* menuItem1 = NULL, * menuItem2 = NULL, * menuItem3 = NULL;

void DestroyMenuItems(void)
{
	if (menuItem1)
	{
		pd->system->removeMenuItem(menuItem1);
		menuItem1 = NULL;
	}
	if (menuItem2)
	{
		pd->system->removeMenuItem(menuItem2);
		menuItem2 = NULL;
	}
	if (menuItem3)
	{
		pd->system->removeMenuItem(menuItem3);
		menuItem3 = NULL;
	}
}

void LevelEditorMenuItemCallback(void* userdata)
{
	//play level
	if (userdata == &menuItem1)
	{
		PlayIfNoErrorsFound();
	}

	//clear
	if (userdata == &menuItem2)
	{
		DeleteAllParts();
	}

	//show info
	if (userdata == &menuItem3)
	{
		CenterLevel();
	}
}

void CreateLevelEditorMenuItems()
{
	//play level
	if (menuItem1 == NULL)
	{
		menuItem1 = pd->system->addMenuItem("Play", LevelEditorMenuItemCallback, &menuItem1);
	}

	//clear
	if (menuItem2 == NULL)
	{
		menuItem2 = pd->system->addMenuItem("Clear", LevelEditorMenuItemCallback, &menuItem2);
	}

	//clear
	if (menuItem3 == NULL)
	{
		menuItem3 = pd->system->addMenuItem("Center", LevelEditorMenuItemCallback, &menuItem3);
	}
}

void GameMenuItemCallback(void* userdata)
{
	//restart
	if (userdata == &menuItem1)
	{
		RestartLevel();
	}

	//level editor 
	if (userdata == &menuItem2)
	{
		ReturnToLevelEditor();
	}
}

void StageSelectMenuItemCallback(void* userdata)
{
	//New Level
	if (userdata == &menuItem1)
	{
		CreateNewLevel();
	}

	//delete Level
	if (userdata == &menuItem2)
	{
		DeleteLevel();
	}
}

void CreateStageSelectMenuItems()
{
	if (menuItem1 == NULL)
	{
		if (LevelEditorMode)
		{
			menuItem1 = pd->system->addMenuItem("Create Level", StageSelectMenuItemCallback, &menuItem1);
		}
	}

	if (menuItem2 == NULL)
	{
		if (LevelEditorMode)
		{
			menuItem2 = pd->system->addMenuItem("Delete Level", StageSelectMenuItemCallback, &menuItem2);
		}
	}
}

void CreateGameMenuItems()
{
	//restart
	if (menuItem1 == NULL)
	{
		menuItem1 = pd->system->addMenuItem("Restart Level", GameMenuItemCallback, &menuItem1);
	}

	//level editor
	if (menuItem2 == NULL)
	{
		if (LevelEditorMode)
		{
			menuItem2 = pd->system->addMenuItem("Level Editor", GameMenuItemCallback, &menuItem2);
		}
	}
}

