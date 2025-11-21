#include "pd_api.h"
#include "Pd_helperfuncs.h"
#include "Sound.h"
#include "Common.h"
#include "CWorldPart.h"
#include "SPoint.h"
#include "stdbool.h"

CWorldPart* CWorldPart_Create(const int PlayFieldXin,const int PlayFieldYin, const int TypeId)
{
	CWorldPart* Result = (CWorldPart* ) malloc(sizeof(CWorldPart));
	if (Result)
	{
		Result->PlayFieldX = PlayFieldXin;
		Result->PlayFieldY = PlayFieldYin;
		Result->Xi = 0;
		Result->Yi = 0;
		Result->X = PlayFieldXin * TileWidth;
		Result->Y = PlayFieldYin * TileHeight;
		Result->Type = TypeId;
		Result->MoveDelay = 0;
		Result->MoveDelayCounter = 0;
		Result->IsMoving = false;
		Result->MoveSpeed = 0;
		Result->Image = NULL;
		Result->ParentList = 0;
		Result->AnimPhase = 0;
		Result->Selected = false;
		Result->FirstArriveEventFired = false;
		Result->Z = 0;
		Result->Group = 0;
		Result->PNeedToKill = false;
		Result->BHide = false;
		Result->AnimBase = 0;
		Result->IsDeath = false;
		switch (TypeId)
		{
			case IDPlayer:
				Result->Image = IMGPlayer;
				Result->AnimBase = 4;
				Result->AnimPhase = 4;
				Result->AnimPhases = 4;
				Result->AnimCounter = 0;
				Result->AnimDelay = 3;
				Result->MoveDelay = 0;
				Result->MoveSpeed = 4;
				Result->AnimDelayCounter = 0;
				Result->Z = ZPlayer;
				break;
			case IDBox:
				Result->Image = IMGBox;
				Result->MoveDelay = 0;
				Result->MoveSpeed = 4;
				Result->Z = ZBox;
				break;
			case IDDiamond:
				Result->Image = IMGDiamond;
				Result->Z = ZDiamond;
				break;
			case IDBomb:
				Result->Image = IMGBomb;
				Result->Z = ZBomb;
				break;
			case IDFloor:
				Result->Image = IMGFloor;
				Result->Z = ZFloor;
				break;
			case IDWall:
				Result->Image = IMGWall;
				Result->Z = ZWall;
				break;
			case IDEmpty:
				Result->Image = IMGEmpty;
				break;
			case IDExplosion:
				playGameExplodeSound();

				Result->Image = IMGExplosion;
				Result->Z = ZExplosion;
				Result->AnimPhase = 0;
				Result->AnimPhases = 8;
				Result->AnimCounter = 1;
				Result->AnimDelay = 1;
				Result->MoveDelay = 0;
				Result->MoveSpeed = 4;
				Result->AnimDelayCounter = 0;
				break;
		}
	}
	return(Result);
}


void CWorldPart_Hide(CWorldPart* WorldPart) 
{
	WorldPart->BHide = true;
}



void CWorldPart_Kill(CWorldPart* WorldPart)
{
	WorldPart->PNeedToKill = true;
}

bool CWorldPart_NeedToKill(CWorldPart* WorldPart) 
{
	return WorldPart->PNeedToKill;
}


int CWorldPart_GetGroup(CWorldPart* WorldPart) 
{
	return WorldPart->Group;
}

int CWorldPart_GetType(CWorldPart* WorldPart) 
{
	return WorldPart->Type;
}

int CWorldPart_GetX(CWorldPart* WorldPart) 
{
	return WorldPart->X;
}

int CWorldPart_GetY(CWorldPart* WorldPart) 
{
	return WorldPart->Y;
}

int CWorldPart_GetPlayFieldX(CWorldPart* WorldPart) 
{
	return WorldPart->PlayFieldX;
}

int CWorldPart_GetPlayFieldY(CWorldPart* WorldPart) 
{
	return WorldPart->PlayFieldY;
}

int CWorldPart_GetZ(CWorldPart* WorldPart) 
{
	return WorldPart->Z;
}

int CWorldPart_GetAnimPhase(CWorldPart* WorldPart) 
{
	return WorldPart->AnimPhase;
}

void CWorldPart_SetAnimPhase(CWorldPart* WorldPart, int AnimPhaseIn) 
{ 
	WorldPart->AnimPhase = AnimPhaseIn;
}

void CWorldPart_Event_ArrivedOnNewSpot(CWorldPart* WorldPart)
{
	int Teller=0;
    if(WorldPart->Type == IDPlayer)
	{
		if (WorldPart->ParentList)
		{
			for (Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
			{
				if((WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY))
				{
					if((WorldPart->ParentList->Items[Teller]->Type == IDBomb))
					{
						CWorldParts_Add(WorldPart->ParentList, CWorldPart_Create(WorldPart->PlayFieldX,WorldPart->PlayFieldY, IDExplosion));
						CWorldPart_Kill(WorldPart->ParentList->Items[Teller]);
						WorldPart->IsDeath = true;
						CWorldPart_Hide(WorldPart);
						break;
					}

					if((WorldPart->ParentList->Items[Teller]->Type == IDDiamond))
					{
						CWorldPart_Kill(WorldPart->ParentList->Items[Teller]);
						playGameCollectSound();
					}

				}
			}
		}
	}

	if (WorldPart->Type == IDBox)
	{

		WorldPart->AnimPhase = 0;
		//printf("Arrive Event fired\n");
		if (WorldPart->ParentList)
		{
			//printf("Parent List Set\n");
			for (Teller=0;Teller< WorldPart->ParentList->ItemCount;Teller++)
			{
			if((WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY) &&
				(WorldPart->ParentList->Items[Teller]->Type == IDBomb))
				{
					CWorldParts_Add(WorldPart->ParentList, CWorldPart_Create(WorldPart->PlayFieldX,WorldPart->PlayFieldY,IDExplosion));
					CWorldPart_Kill(WorldPart->ParentList->Items[Teller]);
					CWorldPart_Kill(WorldPart);
					break;
				}
			}
		}
	}	
}

void CWorldPart_Event_BeforeDraw(CWorldPart* WorldPart) 
{
	switch(WorldPart->Type)
	{
		case IDPlayer:
		{
			if (WorldPart->IsMoving)
			{
				WorldPart->AnimPhase = WorldPart->AnimBase + WorldPart->AnimCounter;
				WorldPart->AnimDelayCounter++;
				if (WorldPart->AnimDelayCounter == WorldPart->AnimDelay)
				{
					WorldPart->AnimDelayCounter = 0;
					WorldPart->AnimCounter++;
					if (WorldPart->AnimCounter == WorldPart->AnimPhases)
						WorldPart->AnimCounter = 0;
				}
			}
			break;
		}

		case IDExplosion:
		{
			WorldPart->AnimDelayCounter++;
			if (WorldPart->AnimDelayCounter == WorldPart->AnimDelay)
			{
				WorldPart->AnimDelayCounter = 0;
				WorldPart->AnimPhase++;
				if (WorldPart->AnimPhase == WorldPart->AnimPhases)
					CWorldPart_Kill(WorldPart);
			}
			break;
		}
		default:
			break;
	}
}

void CWorldPart_Event_LeaveCurrentSpot(CWorldPart* WorldPart) 
{

}

void CWorldPart_Event_Moving(CWorldPart* WorldPart, int ScreenPosX,int ScreenPosY,int ScreenXi, int ScreenYi) 
{
	if(WorldPart->Type == IDPlayer)
	{
		if((ScreenPosX > (WorldPart->ParentList->ViewPort->MaxScreenX) - ORIG_WINDOW_WIDTH/2) && (WorldPart->Xi > 0))
			CViewPort_Move(WorldPart->ParentList->ViewPort, WorldPart->Xi,WorldPart->Yi);
		if((ScreenPosX < (WorldPart->ParentList->ViewPort->MaxScreenX) - ORIG_WINDOW_WIDTH/2) && (WorldPart->Xi < 0))
			CViewPort_Move(WorldPart->ParentList->ViewPort, WorldPart->Xi,WorldPart->Yi);
		if((ScreenPosY > (WorldPart->ParentList->ViewPort->MaxScreenY) - ORIG_WINDOW_HEIGHT/2) && (WorldPart->Yi > 0))
			CViewPort_Move(WorldPart->ParentList->ViewPort, WorldPart->Xi,WorldPart->Yi);
		if((ScreenPosY < (WorldPart->ParentList->ViewPort->MaxScreenY) - ORIG_WINDOW_HEIGHT/2) && (WorldPart->Yi < 0))
			CViewPort_Move(WorldPart->ParentList->ViewPort, WorldPart->Xi,WorldPart->Yi);
	}
}

void CWorldPart_SetPosition(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin)
{
	if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
	{
		WorldPart->PlayFieldX=PlayFieldXin;
		WorldPart->PlayFieldY=PlayFieldYin;
		WorldPart->X=PlayFieldXin*TileWidth;
		WorldPart->Y=PlayFieldYin*TileHeight;
		//Event_ArrivedOnNewSpot();
	}
}

void CWorldPart_MoveTo(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin,bool BackWards)
{
	int Teller;
 	if(WorldPart->Type == IDPlayer )
	{
		if(!WorldPart->IsMoving)
		{
			if(CWorldPart_CanMoveTo(WorldPart, PlayFieldXin,PlayFieldYin) || BackWards)
			{
				WorldPart->PlayFieldX = PlayFieldXin;
				WorldPart->PlayFieldY = PlayFieldYin;
				if(WorldPart->X < WorldPart->PlayFieldX*TileWidth)
				{
					WorldPart->Xi = WorldPart->MoveSpeed;
					if (WorldPart->ParentList)
					{
						for(Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
						{
							if((WorldPart->ParentList->Items[Teller]->Type == IDBox) && 
								((WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)))
							{
								CWorldPart_MoveTo(WorldPart->ParentList->Items[Teller], WorldPart->PlayFieldX+1,WorldPart->PlayFieldY,false);
								break;
							}
						}
					}
					WorldPart->AnimBase = 4;

				}
				if(WorldPart->X > WorldPart->PlayFieldX*TileWidth)
				{
					WorldPart->Xi = -WorldPart->MoveSpeed;
					if (WorldPart->ParentList)
					{
						for(Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
						{
							if((WorldPart->ParentList->Items[Teller]->Type == IDBox) && 
							   ((WorldPart->PlayFieldX == WorldPart->ParentList->Items[Teller]->PlayFieldX )  && 
							    (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)))
							{
								CWorldPart_MoveTo(WorldPart->ParentList->Items[Teller], WorldPart->PlayFieldX-1,WorldPart->PlayFieldY,false);
								break;
							}
						}
					}
					WorldPart->AnimBase = 0;
				}
				if(WorldPart->Y > WorldPart->PlayFieldY*TileHeight)
				{
					WorldPart->Yi = -WorldPart->MoveSpeed;
					if (WorldPart->ParentList)
					{
						for(Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
						{
							if((WorldPart->ParentList->Items[Teller]->Type == IDBox) && 
							   ((WorldPart->PlayFieldY == WorldPart->ParentList->Items[Teller]->PlayFieldY)  && 
							    (WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX)))
							{
								CWorldPart_MoveTo(WorldPart->ParentList->Items[Teller], WorldPart->PlayFieldX,WorldPart->PlayFieldY-1,false);
								break;
							}
						}
					}
					WorldPart->AnimBase = 8;
				}
				if(WorldPart->Y < WorldPart->PlayFieldY*TileHeight)
				{
					WorldPart->Yi = WorldPart->MoveSpeed;
					if(WorldPart->ParentList)
					{
						for(Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
						{
							if((WorldPart->ParentList->Items[Teller]->Type == IDBox) && 
							   ((WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)  && 
							    (WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX )))
							{
								CWorldPart_MoveTo(WorldPart->ParentList->Items[Teller], WorldPart->PlayFieldX,WorldPart->PlayFieldY+1,false);
								break;
							}
						}
					}
					WorldPart->AnimBase = 12;
				}
				playGameMoveSound();
				WorldPart->IsMoving = true;
			}
			else
			{
				if (PlayFieldXin > WorldPart->PlayFieldX)
				{
					WorldPart->AnimBase= 4;
				}
				if (PlayFieldXin < WorldPart->PlayFieldX)
				{
					WorldPart->AnimBase = 0;
				}
				if (PlayFieldYin > WorldPart->PlayFieldY)
				{
					WorldPart->AnimBase = 12;
				}
				if (PlayFieldYin < WorldPart->PlayFieldY)
				{
					WorldPart->AnimBase = 8;
				}
				WorldPart->AnimPhase = WorldPart->AnimBase + WorldPart->AnimCounter;
				WorldPart->AnimDelayCounter++;
				if (WorldPart->AnimDelayCounter == WorldPart->AnimDelay)
				{
					WorldPart->AnimDelayCounter = 0;
					WorldPart->AnimCounter++;
					if (WorldPart->AnimCounter == WorldPart->AnimPhases)
						WorldPart->AnimCounter = 0;
				}
			}

		}
	}
	else
	{
		if(!WorldPart->IsMoving)
		{
			if((PlayFieldXin != WorldPart->PlayFieldX) || (PlayFieldYin != WorldPart->PlayFieldY))
				if(CWorldPart_CanMoveTo(WorldPart, PlayFieldXin,PlayFieldYin) || BackWards)
				{
					WorldPart->PlayFieldX = PlayFieldXin;
					WorldPart->PlayFieldY = PlayFieldYin;
					if(WorldPart->X < WorldPart->PlayFieldX*TileWidth)
						WorldPart->Xi = WorldPart->MoveSpeed;
					if(WorldPart->X > WorldPart->PlayFieldX*TileWidth)
						WorldPart->Xi = -WorldPart->MoveSpeed;
					if(WorldPart->Y > WorldPart->PlayFieldY*TileHeight)
						WorldPart->Yi = -WorldPart->MoveSpeed;
					if(WorldPart->Y < WorldPart->PlayFieldY*TileHeight)
						WorldPart->Yi = WorldPart->MoveSpeed;
					WorldPart->IsMoving = true;
					CWorldPart_Event_LeaveCurrentSpot(WorldPart);
				}
		}
	}
}

bool CWorldPart_CanMoveTo(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin) 
{
	int Teller;
	bool Result = true;
	switch(WorldPart->Type)
	{
		case IDPlayer:
		{
			if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
			{
				if (WorldPart->ParentList)
				{
					for (Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
					{
						if((WorldPart->ParentList->Items[Teller]->PlayFieldX == PlayFieldXin) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == PlayFieldYin))
						{
							if(WorldPart->ParentList->Items[Teller]->Type == IDWall)
							{
								Result = false;
								break;
							}
							if(WorldPart->ParentList->Items[Teller]->Type == IDBox)
							{
								if (WorldPart->PlayFieldX > PlayFieldXin)
								{
									Result = CWorldPart_CanMoveTo(WorldPart->ParentList->Items[Teller], PlayFieldXin-1,PlayFieldYin);
								}
								if (WorldPart->PlayFieldX < PlayFieldXin)
								{
									Result = CWorldPart_CanMoveTo(WorldPart->ParentList->Items[Teller], PlayFieldXin+1,PlayFieldYin);
								}
								if (WorldPart->PlayFieldY > PlayFieldYin)
								{
									Result = CWorldPart_CanMoveTo(WorldPart->ParentList->Items[Teller], PlayFieldXin,PlayFieldYin-1);
								}
								if (WorldPart->PlayFieldY < PlayFieldYin)
								{
									Result = CWorldPart_CanMoveTo(WorldPart->ParentList->Items[Teller], PlayFieldXin,PlayFieldYin+1);
								}
								break;
							}
						}
					}
				}
				else
					Result = false;
			}
			break;
		}

		case IDBox:
		{
			if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
			{
				if (WorldPart->ParentList)
				{
					for (Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
						if((WorldPart->ParentList->Items[Teller]->Type == IDWall) || (WorldPart->ParentList->Items[Teller]->Type == IDBox) || (WorldPart->ParentList->Items[Teller]->Type == IDDiamond))
							if((WorldPart->ParentList->Items[Teller]->PlayFieldX == PlayFieldXin) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == PlayFieldYin))
							{
								Result = false;
								break;
							}
				}
			}
			else
				Result = false;
			break;
		}
		default:
			Result = false;
			break;
	}
	return Result;
}

void CWorldPart_Move(CWorldPart* WorldPart)
{
	if (!WorldPart->FirstArriveEventFired)
	{
		CWorldPart_Event_ArrivedOnNewSpot(WorldPart);
		WorldPart->FirstArriveEventFired=true;
	}
	if (WorldPart->IsMoving)
	{
		if (WorldPart->MoveDelayCounter == WorldPart->MoveDelay)
		{
			WorldPart->X += WorldPart->Xi;
			WorldPart->Y += WorldPart->Yi;
			CWorldPart_Event_Moving(WorldPart, WorldPart->X,WorldPart->Y,WorldPart->Xi,WorldPart->Yi);
			if ((WorldPart->X == WorldPart->PlayFieldX * TileWidth) && (WorldPart->Y == WorldPart->PlayFieldY * TileHeight))
			{
				WorldPart->IsMoving = false;
				WorldPart->Xi = 0;
				WorldPart->Yi = 0;
				CWorldPart_Event_ArrivedOnNewSpot(WorldPart);
			}
			WorldPart->MoveDelayCounter = -1;
		}
		WorldPart->MoveDelayCounter++;
	}
}

void CWorldPart_Draw(CWorldPart* WorldPart)
{
	if (!WorldPart->BHide)
	{
		if (WorldPart->Image)
		{
			if (WorldPart->ParentList)
			{
				CWorldPart_Event_BeforeDraw(WorldPart);
				DrawBitmapSrcRec(WorldPart->Image, WorldPart->X - WorldPart->ParentList->ViewPort->MinScreenX, WorldPart->Y - WorldPart->ParentList->ViewPort->MinScreenY, WorldPart->AnimPhase * TileWidth, 0, TileWidth, TileHeight, kBitmapUnflipped);
			}
			else
			{
				DrawBitmapSrcRec(WorldPart->Image, WorldPart->X, WorldPart->Y, WorldPart->AnimPhase * TileWidth, 0, TileWidth, TileHeight, kBitmapUnflipped);
			}
		}

		if (WorldPart->Selected)
		{
			if (WorldPart->ParentList)
			{
				pd->graphics->drawRect(WorldPart->X - 1 - WorldPart->ParentList->ViewPort->MinScreenX, WorldPart->Y - 1 - WorldPart->ParentList->ViewPort->MinScreenY, TileWidth + 1, TileHeight + 1, kColorWhite);
				pd->graphics->drawRect(WorldPart->X - 2 - WorldPart->ParentList->ViewPort->MinScreenX, WorldPart->Y - 2 - WorldPart->ParentList->ViewPort->MinScreenY, TileWidth + 3, TileHeight + 3, kColorWhite);
			}
			else
			{
				pd->graphics->drawRect(WorldPart->X - 1, WorldPart->Y - 1, TileWidth + 1, TileHeight + 1, kColorWhite);
				pd->graphics->drawRect(WorldPart->X - 2, WorldPart->Y - 2, TileWidth + 3, TileHeight + 3, kColorWhite);
			}
		}
	}
}

void CWorldPart_Destroy(CWorldPart* WorldPart)
{
	free(WorldPart);
	WorldPart = NULL;
}
