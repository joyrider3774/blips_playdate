#ifndef CSELECTOR_H
#define CSELECTOR_H

#include "CWorldPart.h"

typedef struct CSelector CSelector;
struct CSelector
{
	CWorldParts* PList;
	CWorldPart *Part;
	int Selection;
};

CSelector* CSelector_Create(CWorldParts* List);
void CSelector_SetPosition(CSelector* Selector, int PlayFieldX, int PlayFieldY);
void CSelector_MoveUp(CSelector* Selector);
void CSelector_MoveDown(CSelector* Selector);
void CSelector_MoveLeft(CSelector* Selector);
void CSelector_MoveRight(CSelector* Selector);
void CSelector_IncSelection(CSelector* Selector);
void CSelector_DecSelection(CSelector* Selector);
void CSelector_Draw(CSelector* Selector);
int CSelector_GetSelection(CSelector* Selector);
int CSelector_GetPlayFieldX(CSelector* Selector);
int CSelector_GetPlayFieldY(CSelector* Selector);
void CSelector_Destroy(CSelector* Selector);


#endif