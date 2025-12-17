#include <string.h>
#include <stdbool.h>
#include "Common.h"
#include "GameFuncs.h"
#include "Pd_helperfuncs.h"
#include "Sound.h"
#include "pd_api.h"
#include "CWorldPart.h"
#include "CWorldParts.h"
#include "Menus.h"

int buttonIgnoreFramesGame = 25;
bool ResetViewPort = false;
bool ExplosionsFound = false;

bool StageDone()
{
	for (int Teller = 0; Teller < WorldParts->ItemCount; Teller++)
		if (WorldParts->Items[Teller]->Type == IDDiamond)
			return false;
	return true;
}

void RestartLevel()
{
	if (!AskingQuestion)
	{
		playMenuSelectSound();
		pd->graphics->drawBitmap(IMGBackground, 0, 0, kBitmapUnflipped);
#ifdef SDL2API
		CWorldParts_DrawFloor(WorldParts, WorldParts->Player1, WorldParts->Player2);
#endif
		CWorldParts_Draw(WorldParts);
		AskQuestion(IDRestartLevel, "You are about to restart this level\nAre you sure you want to restart?\n\nPress (A) to Restart (B) to Cancel");
	}
}

void ReturnToLevelEditor()
{
	if(!AskingQuestion && LevelEditorMode)
	{
		GameState=GSLevelEditorInit;
		DestroyMenuItems();
		playMenuBackSound();
	}
}

void GameInit()
{
	int Teller;
	buttonIgnoreFramesGame = 25;
	ResetViewPort = false;
	ExplosionsFound = false;
	CreateGameMenuItems();
	if (LevelEditorMode && StageReload)
	{
		CWorldParts_Load(WorldParts, "blips_temp.lev");
	}
	CWorldPart *Player=NULL;
	CWorldPart *Player2 = NULL;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		if (WorldParts->Items[Teller]->Type == IDPlayer)
		{
			Player = WorldParts->Items[Teller];
			break;
		}

		if (WorldParts->Items[Teller]->Type == IDPlayer2)
		{
			Player2 = WorldParts->Items[Teller];
		}
	}
	//should never happen
	if(!Player && !Player2)
	{
		Player = CWorldPart_Create(0,0, IDPlayer);
		CWorldParts_Add(WorldParts,Player);
		CWorldParts_LimitVPLevel(WorldParts);
	}
}

void Game()
{
	char* Msg;
	char* FileName;
	if(GameState == GSGameInit)
	{
		GameInit();
		GameState = GSGame;
	}
	bool response = false;
	int id = -1;
	if(!AskingQuestion)
	{
		if (!WorldParts->Player->IsMoving && StageDone())
		{
			playLevelDoneSound();
			if (LevelEditorMode)
			{
				pd->system->formatString(&Msg, "Congratulations !\nYou Succesfully Solved this level\nDo you want to return to the level editor ?\n\n(A) Leveleditor (B) Play Again");
				AskQuestion(IDSolvedLevelLevelEditorMode, Msg);
				pd->system->realloc(Msg, 0);
			}
			else
			{
				if (SelectedLevel == UnlockedLevels)
				{
					if ( UnlockedLevels < InstalledLevels)
					{
						pd->system->formatString(&Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d\nThe next level has now been unlocked!\n\nPress (A) to continue",SelectedLevel,InstalledLevels);
						AskQuestion(IDSolvedLevelNextUnlocked, Msg);
						pd->system->realloc(Msg, 0);
					}
					else
					{
						pd->system->formatString(&Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d\nlevelpack %s\nis now finished, try out another one!\n\nPress (A) to continue",SelectedLevel,InstalledLevels,LevelPackName);
						AskQuestion(IDSolvedLastLevel, Msg);
						pd->system->realloc(Msg, 0);
							
					}
				}
				else
				{
					pd->system->formatString(&Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d\n\n\nPress (A) to continue",SelectedLevel,InstalledLevels);
					AskQuestion(IDSolvedEarlierLevel, Msg);
					pd->system->realloc(Msg, 0);
				}
			}
		}
		
		if((currButtons & kButtonB) && !(prevButtons & kButtonB))
		{
			if(!LevelEditorMode)
			{
				GameState = GSStageSelectInit;

				playMenuBackSound();
			}
		}

		if (!WorldParts->Player->IsMoving && (((WorldParts->Player1) && !WorldParts->Player1->IsDeath) || ((WorldParts->Player2) && !WorldParts->Player2->IsDeath)))
		{
			if (!(prevButtons & kButtonA) && (currButtons & kButtonA))
			{
				CWorldParts_SwitchPlayers(WorldParts);
				buttonIgnoreFramesGame = 15;
			}
			if ((buttonIgnoreFramesGame <= 0) && (currButtons & kButtonA))
			{
				if (currButtons & kButtonA)
				{
					if (currButtons & kButtonLeft)
						CViewPort_Move(WorldParts->ViewPort,-4, 0);
					if (currButtons & kButtonRight)
						CViewPort_Move(WorldParts->ViewPort, 4, 0);
					if (currButtons & kButtonUp)
						CViewPort_Move(WorldParts->ViewPort, 0, -4);
					if (currButtons & kButtonDown)
						CViewPort_Move(WorldParts->ViewPort, 0, 4);
					ResetViewPort = true;
				}
			}
			else
			{
				buttonIgnoreFramesGame--;
				if (ResetViewPort)
				{
					CWorldParts_LimitVPLevel(WorldParts);
					ResetViewPort = false;
				}
			}

			if (!(currButtons & kButtonA) && (((WorldParts->Player1) && !WorldParts->Player1->IsDeath) || ((WorldParts->Player2) && !WorldParts->Player2->IsDeath)) && (currButtons & kButtonRight))
			{
				buttonIgnoreFramesGame = 0;
				CWorldPart_MoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX + 1, WorldParts->Player->PlayFieldY, false);
			}
			else
			{
				if (!(currButtons & kButtonA) && (((WorldParts->Player1) && !WorldParts->Player1->IsDeath) || ((WorldParts->Player2) && !WorldParts->Player2->IsDeath)) &&  (currButtons & kButtonLeft))
				{
					buttonIgnoreFramesGame = 0;
					CWorldPart_MoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX - 1, WorldParts->Player->PlayFieldY, false);
				}
				else
				{
					if (!(currButtons & kButtonA) && (((WorldParts->Player1) && !WorldParts->Player1->IsDeath) || ((WorldParts->Player2) && !WorldParts->Player2->IsDeath)) && (currButtons & kButtonUp))
					{
						buttonIgnoreFramesGame = 0;
						CWorldPart_MoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX, WorldParts->Player->PlayFieldY - 1, false);
					}
					else
					{
						if (!(currButtons & kButtonA) && (((WorldParts->Player1) && !WorldParts->Player1->IsDeath) || ((WorldParts->Player2) && !WorldParts->Player2->IsDeath)) && (currButtons & kButtonDown))
						{
							buttonIgnoreFramesGame = 0;
							CWorldPart_MoveTo(WorldParts->Player, WorldParts->Player->PlayFieldX, WorldParts->Player->PlayFieldY + 1, false);
						}
					}
				}
			}
		}

		
		if(!AskingQuestion)
		{
			pd->graphics->drawBitmap(IMGBackground, 0, 0, kBitmapUnflipped);
			CWorldParts_Move(WorldParts);
#ifdef SDL2API
			CWorldParts_DrawFloor(WorldParts, WorldParts->Player1, WorldParts->Player2);
#endif
			CWorldParts_Draw(WorldParts);
			if (false)
			{
				char* Tekst;
				pd->system->formatString(&Tekst, "X: %d - Y: %d", WorldParts->Player->PlayFieldX - WorldParts->ViewPort->VPMinX, WorldParts->Player->PlayFieldY - WorldParts->ViewPort->VPMinY);
				pd->graphics->fillRect(ORIG_WINDOW_WIDTH - 60 * UI_WIDTH_SCALE, 0, 60 * UI_WIDTH_SCALE, 17 * UI_HEIGHT_SCALE, kColorWhite);
				pd->graphics->drawRect(ORIG_WINDOW_WIDTH - 60 * UI_WIDTH_SCALE, -1, 61 * UI_WIDTH_SCALE, 18 * UI_HEIGHT_SCALE, kColorBlack);
				drawText(font, Tekst, strlen(Tekst), kASCIIEncoding, 262 * UI_WIDTH_SCALE, 2 * UI_HEIGHT_SCALE);
				pd->system->realloc(Tekst, 0);
			}
			if (((WorldParts->Player1) && WorldParts->Player1->IsDeath) || ((WorldParts->Player2) && WorldParts->Player2->IsDeath))
			{
				ExplosionsFound = false;
				for (int teller = 0; teller < WorldParts->ItemCount; teller++)
				{
					if (WorldParts->Items[teller]->Type == IDExplosion)
					{
						ExplosionsFound = true;
						break;
					}
				}

				if (!ExplosionsFound)
				{
					if (LevelEditorMode)
					{
						AskQuestion(IDPlayerDiedLevelEditorMode, "Too bad you died !\nDo you want to return to the\nlevel editor ?\n\n(A) Leveleditor (B) Play Again");
					}
					else
					{
						AskQuestion(IDPlayerDied, "Too bad you died !\nDo you want to try again?\n\n(A) Try Again (B) Level Selector");
					}
				}
			}
		}
	}
	else
	{
		if(AskQuestionUpdate(&id, &response, false))
		{
			if (id == IDPlayerDied)
			{
				if (response)
				{
					if (LevelEditorMode && StageReload)
					{
						CWorldParts_Load(WorldParts, "blips_temp.lev");
					}
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
				else
				{
					GameState = GSStageSelectInit;
				}
			}

			if (id == IDPlayerDiedLevelEditorMode)
			{
				if(response)
				{
					GameState = GSLevelEditorInit;
				}
				else
				{					
					CWorldParts_Load(WorldParts, "blips_temp.lev");					
				}			
			}

			if(id == IDSolvedLevelLevelEditorMode)
			{
				if (response)
				{
					GameState = GSLevelEditorInit;
				}
				else
				{
					CWorldParts_RemoveAll(WorldParts);
					CWorldParts_Load(WorldParts,"blips_temp.lev");
				}
			}

			if(id == IDSolvedLevelNextUnlocked)
			{
				UnlockedLevels++;
				SelectedLevel++;
				SaveUnlockData();
				GameState = GSStageSelectInit;
			}

			if(id == IDSolvedLastLevel)
			{
				GameState = GSTitleScreenInit;
			}

			if (id == IDSolvedEarlierLevel)
			{
				GameState = GSStageSelectInit;
			}

			if (id == IDRestartLevel)
			{
				if(response)
				{
					if (LevelEditorMode && StageReload)
					{
						CWorldParts_Load(WorldParts, "blips_temp.lev");
					}
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
			}
		}
	}

	if (GameState != GSGame)
	{
		DestroyMenuItems();
	}	
}
