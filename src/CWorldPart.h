#ifndef CWORLDPART_H
#define CWORLDPART_H

#include "pd_api.h"
#include "Common.h"
#include "CWorldParts.h"
#include "SPoint.h"

typedef struct CWorldParts CWorldParts;
typedef struct CWorldPart CWolrdPart;
struct CWorldPart
{
	int MoveDelayCounter;
	bool FirstArriveEventFired,PNeedToKill;
	int Type,MoveSpeed,MoveDelay,Xi,Yi,X,Y,AnimPhase,PlayFieldX,PlayFieldY,Z,Group;
	bool BHide;
	int AnimBase,AnimCounter,AnimDelay,AnimDelayCounter,AnimPhases;
	LCDBitmap * Image;

	CWorldParts *ParentList;
	bool IsMoving;
	bool Selected;
	bool IsDeath;
};

CWorldPart* CWorldPart_Create(const int PlayFieldXin,const int PlayFieldYin, const int TypeId);
void CWorldPart_Hide(CWorldPart* WorldPart);

void CWorldPart_Kill(CWorldPart* WorldPart);
bool CWorldPart_NeedToKill(CWorldPart* WorldPart);
int CWorldPart_GetGroup(CWorldPart* WorldPart);
int CWorldPart_GetType(CWorldPart* WorldPart);
int CWorldPart_GetX(CWorldPart* WorldPart);
int CWorldPart_GetY(CWorldPart* WorldPart);
int CWorldPart_GetPlayFieldX(CWorldPart* WorldPart);
int CWorldPart_GetPlayFieldY(CWorldPart* WorldPart);
int CWorldPart_GetZ(CWorldPart* WorldPart);
int CWorldPart_GetAnimPhase(CWorldPart* WorldPart);
void CWorldPart_SetAnimPhase(CWorldPart* WorldPart, int AnimPhaseIn);
void CWorldPart_SetPosition(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin);


void CWorldPart_Event_ArrivedOnNewSpot(CWorldPart* WorldPart);
void CWorldPart_Event_BeforeDraw(CWorldPart* WorldPart);
void CWorldPart_Event_LeaveCurrentSpot(CWorldPart* WorldPart);
void CWorldPart_Event_Moving(CWorldPart* WorldPart, int ScreenPosX,int ScreenPosY,int ScreenXi, int ScreenYi);
void CWorldPart_MoveTo(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin,bool BackWards);
bool CWorldPart_CanMoveTo(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin);
void CWorldPart_Move(CWorldPart* WorldPart);
void CWorldPart_Draw(CWorldPart* WorldPart);

void CWorldPart_Destroy(CWorldPart* WorldPart);


#endif