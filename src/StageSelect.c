#include <string.h>
#include <stdbool.h>
#include "StageSelect.h"
#include "Common.h"
#include "GameFuncs.h"
#include "Pd_helperfuncs.h"
#include "Sound.h"
#include "Menus.h"
#include "pd_api.h"
#include "Crank.h"
#include "CWorldParts.h"
#include "CLevelPackFile.h"

void StageSelectInit()
{
	char* FileName;
	setCrankMoveThreshold(90);
	CreateStageSelectMenuItems();
	if (SelectedLevel > 0)
	{
		pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
		if (!FileExists(FileName, true))
		{
			pd->system->realloc(FileName, 0);
			pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
		}
		if (FileExists(FileName, true) || FileExists(FileName, false))
			CWorldParts_Load(WorldParts, FileName);
		else
			CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
		pd->system->realloc(FileName, 0);
	}
	else
		CWorldParts_RemoveAll(WorldParts);
	
}

void CreateNewLevel()
{
	if(!AskingQuestion && LevelEditorMode)
	{
		if (InstalledLevels + 1 <= MAXLEVELS)
		{
			SelectedLevel = 0;
			CWorldParts_RemoveAll(WorldParts);
			LevelHasChanged = false;
			DestroyMenuItems();
			GameState = GSLevelEditorInit;
		}
		else
		{
			char* Tekst;
			pd->system->formatString(&Tekst, "The maximum amount (%d) of levels for\nthis levelpack has been reached!\nPlease create a new levelpack or delete a level\n\nPress (A) to continue", MAXLEVELS);
			AskQuestion(IDMaxLevelsLevelPack, Tekst);
			pd->system->realloc(Tekst, 0);
		}
	}
}

void DeleteLevel()
{
	if(!AskingQuestion && LevelEditorMode && (SelectedLevel > 0))
	{
		playMenuSelectSound();
		char* Tekst;
		pd->system->formatString(&Tekst,"Are you sure you want to delete this level:\n%s - Level %d\n\nPress (A) to Delete (B) to Cancel",LevelPackName,SelectedLevel);
		AskQuestion(IDDeleteLevel, Tekst);
		pd->system->realloc(Tekst, 0);
	}
}

void StageSelect()
{
	char* FileName;
	int Teller;
	char *Tekst;
	if (GameState == GSStageSelectInit)
	{
		StageSelectInit();
		GameState = GSStageSelect;
	}
	int CrankMove = crankUpdate();
	int id = -1;
	bool response = false;
	if(!AskingQuestion)
	{
		pd->graphics->drawBitmap(IMGBackground,0,0,kBitmapUnflipped);
		if (SelectedLevel > 0)
		{
			//CWorldParts_Move(WorldParts);
#ifdef SDL2API
			CWorldParts_DrawFloor(WorldParts, WorldParts->Player1, WorldParts->Player2);
#endif
			CWorldParts_Draw(WorldParts);
			if ((InstalledLevels > 0))
			{
				pd->graphics->fillRect(0, ORIG_WINDOW_HEIGHT - 16 * UI_HEIGHT_SCALE, 320 * UI_WIDTH_SCALE, 17 * UI_HEIGHT_SCALE, kColorWhite);
				pd->graphics->drawRect(0, ORIG_WINDOW_HEIGHT - 16 * UI_HEIGHT_SCALE, 320 * UI_WIDTH_SCALE, 17 * UI_HEIGHT_SCALE, kColorBlack);
				if (WorldParts->isLevelPackFileLevel)
				{
					if ((strlen(LevelPackFile->LevelsMeta[SelectedLevel - 1].title) > 0) || (strlen(LevelPackFile->LevelsMeta[SelectedLevel - 1].author) > 0))
					{
						if (strlen(LevelPackFile->LevelsMeta[SelectedLevel - 1].author) > 0)
							pd->system->formatString(&Tekst, "%s by %s", LevelPackFile->LevelsMeta[SelectedLevel - 1].title, LevelPackFile->LevelsMeta[SelectedLevel - 1].author);
						else
							pd->system->formatString(&Tekst, "%s by %s", LevelPackFile->LevelsMeta[SelectedLevel - 1].title, LevelPackFile->author);
						int w = pd->graphics->getTextWidth(font, Tekst, strlen(Tekst), kUTF8Encoding, 0);
						drawText(font, Tekst, strlen(Tekst), kUTF8Encoding, (ORIG_WINDOW_WIDTH - w) / 2, ORIG_WINDOW_HEIGHT - 13 * UI_HEIGHT_SCALE);
						pd->system->realloc(Tekst, 0);
					}
				}
				else
				{
					if (strlen(NormalCreateName) > 0)
						pd->system->formatString(&Tekst, "Level %d by %s", SelectedLevel, NormalCreateName);
					else
						pd->system->formatString(&Tekst, "Leved %d", SelectedLevel);
					int w = pd->graphics->getTextWidth(font, Tekst, strlen(Tekst), kUTF8Encoding, 0);
					drawText(font, Tekst, strlen(Tekst), kUTF8Encoding, (ORIG_WINDOW_WIDTH - w) / 2, ORIG_WINDOW_HEIGHT - 13 * UI_HEIGHT_SCALE);
					pd->system->realloc(Tekst, 0);
				}
			}
		}
		pd->graphics->fillRect(0, 0, 320 * UI_WIDTH_SCALE, 16 * UI_HEIGHT_SCALE, kColorWhite);
		pd->graphics->drawRect(0, -1 * UI_HEIGHT_SCALE, 320 * UI_WIDTH_SCALE, 17 * UI_HEIGHT_SCALE, kColorBlack);

		if(LevelEditorMode)
		{
			if (SelectedLevel == 0)
				pd->system->formatString(&Tekst,"Level Pack: %s -> %d Levels - A:Create Level",LevelPackName,InstalledLevels);
			else
				pd->system->formatString(&Tekst,"Level Pack: %s Level:%d/%d - A:Edit",LevelPackName,SelectedLevel,InstalledLevels);
		}
		else
		{
			if(SelectedLevel <= UnlockedLevels)
				pd->system->formatString(&Tekst,"Level Pack: %s Level:%d/%d - A:Play",LevelPackName,SelectedLevel,InstalledLevels);
			else
				pd->system->formatString(&Tekst,"Level Pack: %s Level:%d/%d - Locked!",LevelPackName,SelectedLevel,InstalledLevels);
		}
		drawText(font,Tekst,strlen(Tekst), kASCIIEncoding, 2*UI_WIDTH_SCALE,1*UI_HEIGHT_SCALE);
		pd->system->realloc(Tekst, 0);

		if ((currButtons & kButtonB) && !(prevButtons & kButtonB))
		{
			if(LevelEditorMode)
				GameState= GSLevelEditorMenuInit;
			else
				GameState= GSTitleScreenInit;
			playMenuBackSound();
			CWorldParts_RemoveAll(WorldParts);			
		}

		if ((currButtons & kButtonA) && !(prevButtons & kButtonA))
		{
			playMenuSelectSound();
			if(LevelEditorMode)
			{
				if (SelectedLevel == 0)
				{
					if (InstalledLevels + 1 <= MAXLEVELS)
					{
						LevelHasChanged = false;
						GameState = GSLevelEditorInit;
					}
					else
					{
						pd->system->formatString(&Tekst, "The maximum amount (%d) of levels for\nthis levelpack has been reached!\nPlease create a new levelpack or delete a level\n\nPress (A) to continue", MAXLEVELS);
						AskQuestion(IDMaxLevelsLevelPack, Tekst);
						pd->system->realloc(Tekst, 0);
					}
				}
				else
				{
					LevelHasChanged = false;
					GameState = GSLevelEditorInit;
				}
			}
			else
			{
				if (SelectedLevel <= UnlockedLevels)
					GameState = GSGameInit;
				else
				{
					pd->system->formatString(&Tekst,"This Level Hasn't been unlocked yet!\nDo you want to play the last unlocked\nlevel %d/%d\n\nPress (A) to Play (B) to Cancel",UnlockedLevels,InstalledLevels);
					AskQuestion(IDLevelNotUnlocked, Tekst);
					pd->system->realloc(Tekst, 0);
				}
			}
		}

		if (CrankMove == CRANKMOVELEFT)
		{
			if (LevelEditorMode)
			{
				if (SelectedLevel != 0)
				{
					playMenuSound();

					SelectedLevel -= 5;
					if (SelectedLevel <= 0)
					{
						SelectedLevel = 0;
						CWorldParts_RemoveAll(WorldParts);
					}
					else
					{
						pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
						if (FileExists(FileName, true))
							CWorldParts_Load(WorldParts, FileName);
						else
						{
							pd->system->realloc(FileName, 0);
							pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
							if (FileExists(FileName, true) || FileExists(FileName, false))
								CWorldParts_Load(WorldParts, FileName);
							else
								CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
						}
						pd->system->realloc(FileName, 0);
					}
				}
			}
			else
			{
				if (SelectedLevel != 1)
				{
					playMenuSound();

					SelectedLevel -= 5;
					if (SelectedLevel < 1)
						SelectedLevel = 1;
					pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
					if (FileExists(FileName, true))
						CWorldParts_Load(WorldParts, FileName);
					else
					{
						pd->system->realloc(FileName, 0);
						pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
						if (FileExists(FileName, true) || FileExists(FileName, false))
							CWorldParts_Load(WorldParts, FileName);
						else
							CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
					}
					pd->system->realloc(FileName, 0);
				}
			}
		}

		if (CrankMove == CRANKMOVERIGHT)
		{
			if (SelectedLevel != InstalledLevels)
			{
				SelectedLevel += 5;
				playMenuSound();
				if (SelectedLevel > InstalledLevels)
					SelectedLevel = InstalledLevels;
				pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
				if (FileExists(FileName, true))
					CWorldParts_Load(WorldParts, FileName);
				else
				{
					pd->system->realloc(FileName, 0);
					pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
					if (FileExists(FileName, true) || FileExists(FileName, false))
						CWorldParts_Load(WorldParts, FileName);
					else
						CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
				}
				pd->system->realloc(FileName, 0);
			}
		}

		if (currButtons & kButtonLeft)
		{
			if (!(prevButtons & kButtonLeft))
				frameLeftStart = framecount;
			if ((framecount - frameLeftStart) % MenuUpdateTicks == 0)
			{
				if (LevelEditorMode)
				{
					if (SelectedLevel != 0)
					{
						playMenuSound();

						SelectedLevel--;
						if (SelectedLevel <= 0)
						{
							SelectedLevel = 0;
							CWorldParts_RemoveAll(WorldParts);
						}
						else
						{
							pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
							if (FileExists(FileName, true))
								CWorldParts_Load(WorldParts, FileName);
							else
							{
								pd->system->realloc(FileName, 0);
								pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
								if (FileExists(FileName, true) || FileExists(FileName, false))
									CWorldParts_Load(WorldParts, FileName);
								else
									CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
							}
							pd->system->realloc(FileName, 0);
						}
					}
				}
				else
				{
					if (SelectedLevel != 1)
					{
						playMenuSound();

						SelectedLevel--;
						if (SelectedLevel < 1)
							SelectedLevel = 1;
						pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
						if (FileExists(FileName, true))
							CWorldParts_Load(WorldParts, FileName);
						else
						{
							pd->system->realloc(FileName, 0);
							pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
							if (FileExists(FileName, true) || FileExists(FileName, false))
								CWorldParts_Load(WorldParts, FileName);
							else
								CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
						}
						pd->system->realloc(FileName, 0);
					}
				}
			}
		}

		if (currButtons & kButtonRight)
		{
			if (!(prevButtons & kButtonRight))
				frameRightStart = framecount;
			if ((framecount - frameRightStart) % MenuUpdateTicks == 0)
			{
				if (SelectedLevel != InstalledLevels)
				{
					SelectedLevel++;
					playMenuSound();
					if (SelectedLevel > InstalledLevels)
						SelectedLevel = InstalledLevels;
					pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
					if (FileExists(FileName, true))
						CWorldParts_Load(WorldParts, FileName);
					else
					{
						pd->system->realloc(FileName, 0);
						pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
						if (FileExists(FileName, true) || FileExists(FileName, false))
							CWorldParts_Load(WorldParts, FileName);
						else
							CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
					}
					pd->system->realloc(FileName, 0);
				}
			}
		}
	}
	else
	{
		if(AskQuestionUpdate(&id, &response, false))
		{
			if( id == IDLevelNotUnlocked)
			{
				if(response)
				{			
					SelectedLevel = UnlockedLevels;
					pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
					if (FileExists(FileName, true))
						CWorldParts_Load(WorldParts, FileName);
					else
					{
						pd->system->realloc(FileName, 0);
						pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
						if (FileExists(FileName, true) || FileExists(FileName, false))
							CWorldParts_Load(WorldParts, FileName);
						else
							CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
					}
					pd->system->realloc(FileName, 0);
					GameState = GSGameInit;
				}
			}

			if (id == IDDeleteLevel)
			{
				if(response)
				{
					pd->system->formatString(&Tekst, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
					//to edit default levels
					if (FileExists(Tekst, true))
					{
						pd->file->unlink(Tekst, false);
						pd->system->formatString(&FileName, "./levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
						//only swap levels if we did not edit a level that also exists as a default level
						if (!FileExists(FileName, false) && (SelectedLevel > LevelPackFile->LevelCount))
						{
							for (Teller = SelectedLevel; Teller < InstalledLevels; Teller++)
							{
								char* file1, * file2;
								pd->system->formatString(&file1, "levelpacks/%s._lev/level%d.lev", LevelPackName, Teller + 1);
								pd->system->formatString(&file2, "levelpacks/%s._lev/level%d.lev", LevelPackName, Teller);
								pd->file->rename(file1, file2);
								pd->system->realloc(file1, 0);
								pd->system->realloc(file2, 0);
							}
						}
						pd->system->realloc(FileName, 0);
						FindLevels();
						if (SelectedLevel > InstalledLevels)
							SelectedLevel = InstalledLevels;
						if (SelectedLevel == 0)
							CWorldParts_RemoveAll(WorldParts);
						else
						{
							pd->system->formatString(&FileName, "levelpacks/%s._lev/level%d.lev", LevelPackName, SelectedLevel);
							if (!FileExists(FileName, true))
							{
								pd->system->realloc(FileName, 0);
								pd->system->formatString(&FileName, "levelpacks/%s/level%d.lev", LevelPackName, SelectedLevel);
							}
							if (FileExists(FileName, true) || FileExists(FileName, false))
								CWorldParts_Load(WorldParts, FileName);
							else
								CWorldParts_LoadFromLevelPackFile(WorldParts, LevelPackFile, SelectedLevel, true);
							pd->system->realloc(FileName, 0);
						}
					}
					pd->system->realloc(Tekst, 0);
				}
			}
		}
	}

	if (GameState != GSStageSelect)
		DestroyMenuItems();
}
