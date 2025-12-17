#include <string.h>
#include <stdbool.h>
#include "LevelEditor.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CSelector.h"
#include "CViewPort.h"
#include "CWorldParts.h"
#include "CWorldPart.h"
#include "Pd_helperfuncs.h"
#include "Crank.h"
#include "Menus.h"
#include "Sound.h"
#include "pd_api.h"

bool ShowPosition=true;
int buttonIgnoreFramesLevelEditor = 25;

CSelector *Selector;

bool LevelErrorsFound()
{
	int Teller,NumDiamond =0,NumPlayer=0;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		if(WorldParts->Items[Teller]->Type == IDDiamond)
			NumDiamond++;
		if(WorldParts->Items[Teller]->Type == IDPlayer || WorldParts->Items[Teller]->Type == IDPlayer2)
			NumPlayer++;
	}
	if (NumPlayer==0)
	{
		playErrorSound();
		AskQuestion(IDNoPlayer, "Can not save this level because there's\nno player in the level! Please add a Player\nand try again or cancel saving.\n\nPress (A) to continue");
		return true;
	}
	if (NumDiamond == 0)
	{
		playErrorSound();
		AskQuestion(IDNoDiamonds, "Can not save this level because there's\nno star in the level! Please add a star\nand try again or cancel saving.\n\nPress (A) to continue");
		return true;
	}
	return false;

}

void PlayIfNoErrorsFound()
{
	if(!AskingQuestion && !LevelErrorsFound())
	{
		playMenuSelectSound();
		CWorldParts_Save(WorldParts, "blips_temp.lev");
		DestroyMenuItems();
		StageReload = true;
		GameState=GSGameInit;
	}
}

void DeleteAllParts()
{
		
	if (!AskingQuestion && (WorldParts->ItemCount > 0))
	{
		playMenuSelectSound();
		AskQuestion(IDDeleteAllParts, "You are about to delete all parts\nin this level, are you sure\nyou want to do this?\n\nPress (A) to delete, (B) to cancel");
		
	}
}

void CenterLevel()
{
	if (!AskingQuestion)
	{
		int MinX = NrOfCols - 1;
		int MinY = NrOfRows - 1;
		int MaxX = 0;
		int MaxY = 0;
		for (int Teller = 0; Teller < WorldParts->ItemCount; Teller++)
		{
			if (WorldParts->Items[Teller]->PlayFieldX < MinX)
				MinX = WorldParts->Items[Teller]->PlayFieldX;
			if (WorldParts->Items[Teller]->PlayFieldY < MinY)
				MinY = WorldParts->Items[Teller]->PlayFieldY;
			if (WorldParts->Items[Teller]->PlayFieldX > MaxX)
				MaxX = WorldParts->Items[Teller]->PlayFieldX;
			if (WorldParts->Items[Teller]->PlayFieldY > MaxY)
				MaxY = WorldParts->Items[Teller]->PlayFieldY;
		}
		int Xi = ((NrOfCols - 1) / 2) - (MaxX + MinX) / 2;
		int Yi = ((NrOfRows - 1) / 2) - (MaxY + MinY) / 2;
		for (int Teller = 0; Teller < WorldParts->ItemCount; Teller++)
		{
			CWorldPart_SetPosition(WorldParts->Items[Teller], WorldParts->Items[Teller]->PlayFieldX + Xi, WorldParts->Items[Teller]->PlayFieldY + Yi);
		}
		CWorldParts_LimitVPLevel(WorldParts);
		//center viewport;
		CViewPort_SetViewPort(WorldParts->ViewPort,(NrOfCols / 2) - 12, (NrOfRows / 2) - 7, (NrOfCols / 2) + 12, (NrOfRows / 2) + 7);
		CSelector_SetPosition(Selector, (NrOfCols / 2), (NrOfRows / 2));
		//if player found center viewport on player !
		CWorldPart* Player1 = NULL, * Player2 = NULL;
		for (int Teller = 0; Teller < WorldParts->ItemCount; Teller++)
		{
			if (WorldParts->Items[Teller]->Type == IDPlayer)
			{
				Player1 = WorldParts->Items[Teller];
			}

			if (WorldParts->Items[Teller]->Type == IDPlayer2)
			{
				Player2 = WorldParts->Items[Teller];
			}
		}
		if(Player1)
			CSelector_SetPosition(Selector, Player1->PlayFieldX, Player1->PlayFieldY);
		if(!Player1 && Player2)
			CSelector_SetPosition(Selector, Player2->PlayFieldX, Player2->PlayFieldY);
		CWorldParts_CenterVPOnPlayer(WorldParts);
		CViewPort_SetVPLimit(WorldParts->ViewPort,0, 0, NrOfCols - 1, NrOfRows - 1);
		if (Xi != 0 || Yi != 0)
			LevelHasChanged = true;
	}
}


void LevelEditorInit()
{
	bool PlayerFound = false;
	setCrankMoveThreshold(90);
	Selector = CSelector_Create(WorldParts);
	CreateLevelEditorMenuItems();
	if (StageReload)
	{
		CWorldParts_Load(WorldParts, "blips_temp.lev");
		pd->file->unlink("blips_temp.lev", false);
		StageReload=false;
	}
	buttonIgnoreFramesLevelEditor = 50;
	CWorldPart* Player1 = NULL, * Player2 = NULL;
	for (int Teller = 0; Teller < WorldParts->ItemCount; Teller++)
	{
		if (WorldParts->Items[Teller]->Type == IDPlayer)
		{
			Player1 = WorldParts->Items[Teller];
		}

		if (WorldParts->Items[Teller]->Type == IDPlayer2)
		{
			Player2 = WorldParts->Items[Teller];
		}
	}
	if (Player1)
	{
		CSelector_SetPosition(Selector, Player1->PlayFieldX, Player1->PlayFieldY);
		PlayerFound = true;
	}
	if (!Player1 && Player2)
	{
		CSelector_SetPosition(Selector, Player2->PlayFieldX, Player2->PlayFieldY);
		PlayerFound = true;
	}

	CViewPort_SetVPLimit(WorldParts->ViewPort, 0, 0, NrOfCols - 1, NrOfRows - 1);
	if (!PlayerFound)
	{
		CViewPort_SetViewPort(WorldParts->ViewPort, (NrOfCols / 2) - 12, (NrOfRows / 2) - 7, (NrOfCols / 2) + 12, (NrOfRows / 2) + 7);
		CSelector_SetPosition(Selector, (NrOfCols / 2), (NrOfRows / 2));
	}
}

void LevelEditor()
{
    int Teller,Teller2;
	bool AnotherPartFound,SamePartFound;
	char* Tekst, *FileName;
	if (GameState == GSLevelEditorInit)
	{
		LevelEditorInit();
		GameState = GSLevelEditor;
	}

	bool response;
	int id;
	if(AskingQuestion)
		buttonIgnoreFramesLevelEditor = 25;
	if (!AskingQuestion)
	{
		pd->graphics->drawBitmap(IMGBackground,0,0,kBitmapUnflipped);
		CWorldParts_Move(WorldParts);
#ifdef SDL2API
		CWorldParts_DrawFloor(WorldParts, WorldParts->Player1, WorldParts->Player2);
#endif		
		CWorldParts_Draw(WorldParts);
		CSelector_Draw(Selector);
		if (ShowPosition)
		{			
			pd->system->formatString(&Tekst,"X: %d - Y: %d",CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector));
			pd->graphics->fillRect(ORIG_WINDOW_WIDTH-60*UI_WIDTH_SCALE,0, 60 * UI_WIDTH_SCALE, 17*UI_HEIGHT_SCALE,kColorWhite);
			pd->graphics->drawRect(ORIG_WINDOW_WIDTH-60*UI_WIDTH_SCALE,-1, 61 * UI_WIDTH_SCALE, 18*UI_HEIGHT_SCALE,kColorBlack);
			drawText(font,Tekst,strlen(Tekst),kASCIIEncoding, 262*UI_WIDTH_SCALE,2*UI_HEIGHT_SCALE);
			pd->system->realloc(Tekst, 0);
		}

		int crankMove = crankUpdate();
		buttonIgnoreFramesLevelEditor--;
		if (buttonIgnoreFramesLevelEditor < 0)
			buttonIgnoreFramesLevelEditor = 0;
		if ((buttonIgnoreFramesLevelEditor <= 0) && (currButtons & kButtonA))
		{
			if (!(prevButtons & kButtonA))
				frameAStart = framecount;
			if ((framecount - frameAStart) % LevelEditorUpdateTicks == 0)
			{
				SamePartFound = false;
				for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
				{
					if ((WorldParts->Items[Teller]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
						(WorldParts->Items[Teller]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
					{
						if (WorldParts->Items[Teller]->Type == CSelector_GetSelection(Selector))
						{
							SamePartFound = true;
						}
						if (CSelector_GetSelection(Selector) == IDEmpty)
						{
							LevelHasChanged = true;
							break;
						}
					}
				}
				if (CSelector_GetSelection(Selector) != IDEmpty)
					if (!LevelHasChanged)
						LevelHasChanged = !SamePartFound;
				switch (CSelector_GetSelection(Selector))
				{
				case IDEmpty:
					for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
					{
						if ((WorldParts->Items[Teller]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
							(WorldParts->Items[Teller]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
						{
							if (WorldParts->Items[Teller]->Type == IDFloor)
							{
								AnotherPartFound = false;
								for (Teller2 = Teller + 1; Teller2 < WorldParts->ItemCount; Teller2++)
								{
									if ((WorldParts->Items[Teller2]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
										(WorldParts->Items[Teller2]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
									{
										CWorldParts_Remove_Type(WorldParts, WorldParts->Items[Teller2]->PlayFieldX, WorldParts->Items[Teller2]->PlayFieldY, WorldParts->Items[Teller2]->Type);
										AnotherPartFound = true;
										break;
									}
								}
								if (!AnotherPartFound)
								{
									CWorldParts_Remove_Type(WorldParts, WorldParts->Items[Teller]->PlayFieldX, WorldParts->Items[Teller]->PlayFieldY, IDFloor);
									break;
								}
							}
							else
							{
								CWorldParts_Remove(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector));
								break;
							}
						}
					}

					break;
				case IDBox:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox));
					break;
				case IDBox1:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1));
					break;
				case IDBox2:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2));
					break;
				case IDBoxBomb:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb));
					break;
				case IDBoxWall:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall));
					break;
				case IDPlayer:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2);
					for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
					{
						if (WorldParts->Items[Teller]->Type == IDPlayer)
							CWorldParts_Remove_Type(WorldParts, WorldParts->Items[Teller]->PlayFieldX, WorldParts->Items[Teller]->PlayFieldY, IDPlayer);
					}
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer));
					break;
				case IDPlayer2:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
					{
						if (WorldParts->Items[Teller]->Type == IDPlayer2)
							CWorldParts_Remove_Type(WorldParts, WorldParts->Items[Teller]->PlayFieldX, WorldParts->Items[Teller]->PlayFieldY, IDPlayer2);
					}
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2));
					break;
				case IDWall:
					CWorldParts_Remove(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector));
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall));
					break;
				case IDWallBreakable:
					CWorldParts_Remove(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector));
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable));
					break;
				case IDFloor:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDFloor);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDFloor));
					break;
				case IDBomb:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb));
					break;
				case IDDiamond:
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDWallBreakable);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox1);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBox2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBoxWall);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDPlayer2);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDBomb);
					CWorldParts_Remove_Type(WorldParts, CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond);
					CWorldParts_Add(WorldParts, CWorldPart_Create(CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector), IDDiamond));
					break;
				}
			}
		}

		if (currButtons & kButtonLeft)
		{
			if (!(prevButtons & kButtonLeft))
				frameLeftStart = framecount;
			if ((framecount - frameLeftStart) % LevelEditorUpdateTicks == 0)
			{
				buttonIgnoreFramesLevelEditor = 0;
				CSelector_MoveLeft(Selector);
				if (CSelector_GetPlayFieldX(Selector) < WorldParts->ViewPort->VPMinX + 3)
					CViewPort_Move(WorldParts->ViewPort, -TileWidth, 0);
			}
		}

		if (currButtons & kButtonRight)
		{
			if (!(prevButtons & kButtonRight))
				frameRightStart = framecount;
			if ((framecount - frameRightStart) % LevelEditorUpdateTicks == 0)
			{
				buttonIgnoreFramesLevelEditor = 0;
				CSelector_MoveRight(Selector);
				if (CSelector_GetPlayFieldX(Selector) > WorldParts->ViewPort->VPMaxX - 3)
					CViewPort_Move(WorldParts->ViewPort, TileWidth, 0);
			}
		}

		if (currButtons & kButtonUp)
		{
			if (!(prevButtons & kButtonUp))
				frameUpStart = framecount;
			if ((framecount - frameUpStart) % LevelEditorUpdateTicks == 0)
			{
				buttonIgnoreFramesLevelEditor = 0;
				CSelector_MoveUp(Selector);
				if (CSelector_GetPlayFieldY(Selector) < WorldParts->ViewPort->VPMinY + 3)
					CViewPort_Move(WorldParts->ViewPort, 0, -TileHeight);
			}
		}

		if (currButtons & kButtonDown)
		{
			if (!(prevButtons & kButtonDown))
				frameDownStart = framecount;
			if ((framecount - frameDownStart) % LevelEditorUpdateTicks == 0)
			{
				buttonIgnoreFramesLevelEditor = 0;
				CSelector_MoveDown(Selector);
				if (CSelector_GetPlayFieldY(Selector) > WorldParts->ViewPort->VPMaxY - 3)
					CViewPort_Move(WorldParts->ViewPort, 0, TileHeight);
			}
		}

		if (crankMove == CRANKMOVELEFT)
		{
			buttonIgnoreFramesLevelEditor = 0;
			CSelector_DecSelection(Selector);
		}

		if (crankMove == CRANKMOVERIGHT)
		{
			buttonIgnoreFramesLevelEditor = 0;
			CSelector_IncSelection(Selector);
		}


		if ((currButtons & kButtonB) && !(prevButtons & kButtonB))
		{
			if (LevelHasChanged)
			{
				playMenuSelectSound();
				AskQuestion(IDCurrentLevelNotSaved, "The current level isn't saved yet!\nDo you want to save it now ?\n\nPress (A) to save, (B) to cancel saving");
			}
			else
			{
				GameState = GSStageSelectInit;
				playMenuBackSound();
			}
		}		
	}
	else
	{
		if(AskQuestionUpdate(&id, &response, false))
		{
			if (id == IDDeleteAllParts)
			{
				if(response)
				{
					LevelHasChanged = true;
					CWorldParts_RemoveAll(WorldParts);
				}
			}

			if (id == IDCurrentLevelNotSaved)
			{
				if(response)
				{
					pd->graphics->drawBitmap(IMGBackground,0,0,kBitmapUnflipped);
					CWorldParts_Move(WorldParts);
#ifdef SDL2API
					CWorldParts_DrawFloor(WorldParts, WorldParts->Player1, WorldParts->Player2);
#endif
					CWorldParts_Draw(WorldParts);
					CSelector_Draw(Selector);
					if (ShowPosition)
					{
						pd->system->formatString(&Tekst, "X: %d - Y: %d", CSelector_GetPlayFieldX(Selector), CSelector_GetPlayFieldY(Selector));
						pd->graphics->fillRect(ORIG_WINDOW_WIDTH - 50 * UI_WIDTH_SCALE, 0, 50 * UI_WIDTH_SCALE, 13 * UI_HEIGHT_SCALE, kColorWhite);
						pd->graphics->drawRect(ORIG_WINDOW_WIDTH - 50 * UI_WIDTH_SCALE, -1, 51 * UI_WIDTH_SCALE, 14 * UI_HEIGHT_SCALE, kColorBlack);
						drawText(font, Tekst, strlen(Tekst), kASCIIEncoding, 272 * UI_WIDTH_SCALE, 2 * UI_HEIGHT_SCALE);
						pd->system->realloc(Tekst, 0);
					}
					if (!LevelErrorsFound())
					{						
						pd->file->mkdir("levelpacks");
						pd->system->formatString(&FileName,"levelpacks/%s._lev", LevelPackName);
						pd->file->mkdir(FileName);
						pd->system->realloc(FileName, 0);
						if (SelectedLevel==0)
							pd->system->formatString(&FileName,"levelpacks/%s._lev/level%d.lev",LevelPackName,InstalledLevels+1);
						else
							pd->system->formatString(&FileName,"levelpacks/%s._lev/level%d.lev",LevelPackName,SelectedLevel);

						CWorldParts_Save(WorldParts, FileName);
						pd->system->realloc(FileName, 0);
						FindLevels();
						if (SelectedLevel==0)
							SelectedLevel = InstalledLevels;
						LevelHasChanged=false;
						GameState = GSStageSelectInit;									
					}

				}
				else
					GameState = GSStageSelectInit;
			}
		}
	}

	if (GameState != GSLevelEditor)
	{
		DestroyMenuItems();
		CSelector_Destroy(Selector);
	}
}
