#ifndef CWORLDPARTS_H
#define CWORLDPARTS_H

#include "pd_api.h"
#include "Pd_helperfuncs.h"
#include "Defines.h"
#include "CViewPort.h"
#include "stdbool.h"
#include "stdbool.h"

typedef struct CWorldPart CWorldPart;
typedef struct CWorldParts CWorldParts;
struct CWorldParts 
{
		CViewPort *ViewPort;
		CWorldPart *Items[NrOfRows*NrOfCols*2];
		CWorldPart* MoveAbleItems[NrOfRows * NrOfCols * 2];
		CWorldPart *Player;
		int ItemCount, MoveAbleItemCount;	
		bool DisableSorting;	
};

CWorldParts* CWorldParts_Create();
void CWorldParts_Sort(CWorldParts* WorldParts);

void CWorldParts_Add(CWorldParts* WorldParts, CWorldPart *WorldPart);
void CWorldParts_CenterVPOnPlayer(CWorldParts* WorldParts);
void CWorldParts_LimitVPLevel(CWorldParts* WorldParts);
void CWorldParts_Move(CWorldParts* WorldParts);
void CWorldParts_Draw(CWorldParts* WorldParts);
void CWorldParts_Remove(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin);
void CWorldParts_Remove_Type(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin,int Type);
void CWorldParts_RemoveAll(CWorldParts* WorldParts);
void CWorldParts_Save(CWorldParts* WorldParts, char *Filename);
void CWorldParts_Load(CWorldParts* WorldParts, char *Filename);
void CWorldParts_DrawFloor(CWorldParts* WorldParts, CWorldPart* Player);
void CWorldParts_Destroy(CWorldParts* WorldParts);
#endif