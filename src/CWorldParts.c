#include "pd_api.h"
#include "Pd_helperfuncs.h"
#include "CWorldParts.h"
#include "CWorldPart.h"
#include "stdbool.h"

CWorldParts* CWorldParts_Create()
{
	CWorldParts* Result = (CWorldParts*) malloc(sizeof(CWorldParts));
	if (Result)
	{
		Result->ItemCount = 0;
		Result->MoveAbleItemCount = 0;
		Result->DisableSorting = false;
		Result->ViewPort = CViewPort_Create(0, 0, 25, 15, 0, 0, NrOfCols - 1, NrOfRows - 1);
	}
	return Result;
}

void CWorldParts_CenterVPOnPlayer(CWorldParts* WorldParts)
{
    int Teller=0,PlayerX=-1,PlayerY=-1;
    for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
        if(WorldParts->Items[Teller]->Type == IDPlayer)
        {

            PlayerX = WorldParts->Items[Teller]->PlayFieldX;
            PlayerY = WorldParts->Items[Teller]->PlayFieldY;
            break;
        }
    CViewPort_SetViewPort(WorldParts->ViewPort, PlayerX-12, PlayerY-7,PlayerX+12, PlayerY+7);
}

void CWorldParts_LimitVPLevel(CWorldParts* WorldParts)
{
    int MinX = NrOfCols, MinY = NrOfRows, MaxX = -1, MaxY = -1, Teller = 0;
	for (Teller = 0;Teller<WorldParts->ItemCount;Teller++)
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
    CViewPort_SetVPLimit(WorldParts->ViewPort, MinX,MinY,MaxX,MaxY);
    CWorldParts_CenterVPOnPlayer(WorldParts);
}

void CWorldParts_RemoveAll(CWorldParts* WorldParts)
{
	int Teller;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		CWorldPart_Destroy(WorldParts->Items[Teller]);
		WorldParts->Items[Teller] = NULL;
	}
	WorldParts->ItemCount=0;
	WorldParts->MoveAbleItemCount = 0;
}
void CWorldParts_Remove(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin)
{
	int Teller1,Teller2;
	for (Teller1=0;Teller1<WorldParts->ItemCount;Teller1++)
	{
		if ((Teller1 < WorldParts->MoveAbleItemCount) && (WorldParts->MoveAbleItems[Teller1]->PlayFieldX == PlayFieldXin) && (WorldParts->MoveAbleItems[Teller1]->PlayFieldY == PlayFieldYin))
		{
			for (Teller2 = Teller1; Teller2 < WorldParts->MoveAbleItemCount - 1; Teller2++)
				WorldParts->MoveAbleItems[Teller2] = WorldParts->MoveAbleItems[Teller2 + 1];
			WorldParts->MoveAbleItemCount--;
		}

		if ((WorldParts->Items[Teller1]->PlayFieldX == PlayFieldXin) && (WorldParts->Items[Teller1]->PlayFieldY == PlayFieldYin))
		{
			CWorldPart_Destroy(WorldParts->Items[Teller1]);
			for (Teller2=Teller1;Teller2<WorldParts->ItemCount-1;Teller2++)
				WorldParts->Items[Teller2] = WorldParts->Items[Teller2+1];
			WorldParts->ItemCount--;
			Teller1--;
		}
	}
}

void CWorldParts_Remove_Type(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin,int Type)
{
	int Teller1,Teller2;
	for (Teller1=0;Teller1<WorldParts->ItemCount;Teller1++)
	{
		if ((Teller1 < WorldParts->MoveAbleItemCount) && (WorldParts->MoveAbleItems[Teller1]->PlayFieldX == PlayFieldXin) && (WorldParts->MoveAbleItems[Teller1]->PlayFieldY == PlayFieldYin) && (WorldParts->MoveAbleItems[Teller1]->Type == Type))
		{
			for (Teller2 = Teller1; Teller2 < WorldParts->MoveAbleItemCount - 1; Teller2++)
				WorldParts->MoveAbleItems[Teller2] = WorldParts->MoveAbleItems[Teller2 + 1];
			WorldParts->MoveAbleItemCount--;
		}

		if ((WorldParts->Items[Teller1]->PlayFieldX == PlayFieldXin) && (WorldParts->Items[Teller1]->PlayFieldY == PlayFieldYin) && (WorldParts->Items[Teller1]->Type == Type))
		{
			CWorldPart_Destroy(WorldParts->Items[Teller1]);
			for (Teller2=Teller1;Teller2<WorldParts->ItemCount-1;Teller2++)
				WorldParts->Items[Teller2] = WorldParts->Items[Teller2+1];
			WorldParts->ItemCount--;
			Teller1--;
		}
	}
}

void CWorldParts_Add(CWorldParts* WorldParts, CWorldPart *WorldPart)
{
	if( WorldParts->ItemCount < NrOfRows*NrOfCols*2 )
	{
		WorldPart->ParentList = WorldParts;
		WorldParts->Items[WorldParts->ItemCount] = WorldPart;
		WorldParts->ItemCount++;
		CWorldParts_Sort(WorldParts);
	}

	if ((WorldPart->Type == IDBox) || (WorldPart->Type == IDPlayer))
		if (WorldParts->MoveAbleItemCount < NrOfRows * NrOfCols * 2)
		{
			WorldParts->MoveAbleItems[WorldParts->MoveAbleItemCount++] = WorldPart;
		}

	if (WorldPart->Type == IDPlayer)
	{
		WorldParts->Player = WorldPart;
	}
	
}


void CWorldParts_Sort(CWorldParts* WorldParts)
{
	int Teller1,Teller2,Index;
	CWorldPart *Part;
	if (!WorldParts->DisableSorting)
	{
		for (Teller1 = 1; Teller1 <WorldParts->ItemCount;Teller1++)
		{
			Index = WorldParts->Items[Teller1]->Z;
			Part = WorldParts->Items[Teller1];
			Teller2 = Teller1;
			while ((Teller2 > 0) && (WorldParts->Items[Teller2-1]->Z > Index))
			{
				WorldParts->Items[Teller2] = WorldParts->Items[Teller2 - 1];
				Teller2--;
			}
			WorldParts->Items[Teller2] = Part;
		}
	}

}

void CWorldParts_Save(CWorldParts* WorldParts, char *Filename)
{
	int Teller,BufferPosition=0;
	char *Buffer = (char*) malloc(sizeof(char)* 3*WorldParts->ItemCount);
	if (Buffer)
	{
		for (Teller = 0; Teller < WorldParts->ItemCount; Teller++)
		{
			Buffer[BufferPosition] = (char)WorldParts->Items[Teller]->Type;
			Buffer[BufferPosition + 1] = (char)WorldParts->Items[Teller]->PlayFieldX;
			Buffer[BufferPosition + 2] = (char)WorldParts->Items[Teller]->PlayFieldY;
			BufferPosition += 3;
		}

		SDFile* Fp = pd->file->open(Filename, kFileWrite);
		if (Fp)
		{
			pd->file->write(Fp, Buffer, 3 * WorldParts->ItemCount);
			pd->file->close(Fp);
		}
		free(Buffer);
	}
}

void CWorldParts_Load(CWorldParts* WorldParts, char *Filename)
{
	int X,Y,Type;
	int BufferPosition=0;
	long FileSize;
	SDFile* Fp = pd->file->open(Filename,kFileRead | kFileReadData);
	if (Fp)
	{
		CWorldParts_RemoveAll(WorldParts);
		WorldParts->DisableSorting = true;
		pd->file->seek(Fp, 0, SEEK_END);
		FileSize = pd->file->tell(Fp);
		pd->file->seek(Fp, 0, SEEK_SET);
		char* Buffer = (char*)malloc(sizeof(char) * FileSize);
		if (Buffer)
		{
			int read = pd->file->read(Fp, Buffer, FileSize);
			while (BufferPosition < read)
			{
				Type = (int)Buffer[BufferPosition];
				X = (int)Buffer[BufferPosition + 1];
				Y = (int)Buffer[BufferPosition + 2];
				BufferPosition += 3;
				//ignore floors it's too slow on playdate
				if (Type != IDFloor)
					CWorldParts_Add(WorldParts, CWorldPart_Create(X, Y, Type));
			}
			free(Buffer);
		}
		pd->file->close(Fp);
		WorldParts->DisableSorting=false;
		CWorldParts_Sort(WorldParts);
		CWorldParts_LimitVPLevel(WorldParts);
		CWorldParts_CenterVPOnPlayer(WorldParts);
	}

}

bool CWorldParts_ItemExists(CWorldParts* WorldParts, int PlayFieldXin, int PlayFieldYin, int Type)
{
	for (int Teller1 = 0; Teller1 < WorldParts->ItemCount; Teller1++)
	{
		if ((WorldParts->Items[Teller1]->PlayFieldX == PlayFieldXin) && (WorldParts->Items[Teller1]->PlayFieldY == PlayFieldYin) && (WorldParts->Items[Teller1]->Type == Type))
			return true;
	}
	return false;
}

// Recursive Floodfill function
void FloodFill(CWorldParts* aWorldParts, bool** visited, int X, int Y)
{
	// Check bounds and whether the tile has been visited
	if (X < 0 || X >= NrOfCols || Y < 0 || Y >= NrOfRows || visited[Y][X])
	{
		return;
	}

	if ((X < aWorldParts->ViewPort->VPMinX) || (X > aWorldParts->ViewPort->VPMaxX) || (Y < aWorldParts->ViewPort->VPMinY) || (Y > aWorldParts->ViewPort->VPMaxY))
	{
		visited[Y][X] = true;
		return;
	}

	if (CWorldParts_ItemExists(aWorldParts, X, Y, IDWall))
	{
		return;
	}

	// Mark the tile as visited
	visited[Y][X] = true;

	// Draw the floor tile
	pd->graphics->drawBitmap(IMGFloor, X * TileWidth - aWorldParts->ViewPort->MinScreenX, Y * TileWidth - aWorldParts->ViewPort->MinScreenY, false);

	// Recur for neighboring tiles
	FloodFill(aWorldParts, visited, X + 1, Y);
	FloodFill(aWorldParts, visited, X - 1, Y);
	FloodFill(aWorldParts, visited, X, Y + 1);
	FloodFill(aWorldParts, visited, X, Y - 1);
}

void  CWorldParts_DrawFloor(CWorldParts* WorldParts, CWorldPart* Player)
{
	if (!Player)
		return;
	// Allocate memory for the visited array using malloc
	bool** visited = (bool**)malloc(NrOfRows * sizeof(bool*));
	for (int i = 0; i < NrOfRows; ++i)
	{
		visited[i] = (bool*)malloc(NrOfCols * sizeof(bool));
		for (int j = 0; j < NrOfCols; ++j)
			visited[i][j] = false; // Initialize the array to false
	}

	FloodFill(WorldParts, visited, Player->PlayFieldX, Player->PlayFieldY);

	// Free the allocated memory for the visited array
	for (int i = 0; i < NrOfRows; ++i)
		free(visited[i]);
	free(visited);
}
void CWorldParts_Move(CWorldParts* WorldParts)
{
	int Teller;
	for (Teller=0;Teller<WorldParts->MoveAbleItemCount;Teller++)
	{
		if(WorldParts->MoveAbleItems[Teller]->IsMoving)
			//other items are not moveable and have no effect
			CWorldPart_Move(WorldParts->MoveAbleItems[Teller]);
	}
}

void CWorldParts_Draw(CWorldParts* WorldParts)
{
	for (int Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
	    if(WorldParts->Items[Teller]->PNeedToKill)
	     {
	         CWorldParts_Remove_Type(WorldParts, WorldParts->Items[Teller]->PlayFieldX,WorldParts->Items[Teller]->PlayFieldY,WorldParts->Items[Teller]->Type);
	         //go back one item to prevent skips & segaults
	         Teller--;
	     }
	     else
            if((WorldParts->Items[Teller]->PlayFieldX >= WorldParts->ViewPort->VPMinX) && (WorldParts->Items[Teller]->PlayFieldX-1 <= WorldParts->ViewPort->VPMaxX) &&
                (WorldParts->Items[Teller]->PlayFieldY >= WorldParts->ViewPort->VPMinY) && (WorldParts->Items[Teller]->PlayFieldY-1 <= WorldParts->ViewPort->VPMaxY))
            {
				CWorldPart_Draw(WorldParts->Items[Teller]);
			}
	}
}

void CWorldParts_Destroy(CWorldParts* WorldParts)
{
    CViewPort_Destroy(WorldParts->ViewPort);
	int Teller;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		CWorldPart_Destroy(WorldParts->Items[Teller]);
		WorldParts->Items[Teller] = NULL;
	}
}
