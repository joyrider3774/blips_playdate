#ifndef CVIEWPORT_H
#define CVIEWPORT_H

typedef struct CViewPort CViewPort;
struct CViewPort
{
    int VPMinX,VPMinY,VPMaxX,VPMaxY,MinScreenX,MinScreenY,MaxScreenX,MaxScreenY,Width,Height,VPLimitMinX,VPLimitMaxX,VPLimitMinY,VPLimitMaxY;
};

CViewPort* CViewPort_Create(int MinX,int MinY, int MaxX,int MaxY,int MinX2,int MinY2,int MaxX2,int MaxY2);
void CViewPort_SetVPLimit(CViewPort* ViewPort, int MinX,int MinY, int MaxX,int MaxY);
void CViewPort_Move(CViewPort* ViewPort, int Xi,int Yi);
void CViewPort_SetViewPort(CViewPort* ViewPort, int MinX,int MinY, int MaxX,int MaxY);
void CViewPort_Destroy(CViewPort* ViewPort);

#endif