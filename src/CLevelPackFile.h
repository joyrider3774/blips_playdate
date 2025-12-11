#ifndef CLEVELPACKFILE_H
#define CLEVELPACKFILE_H

#include <stdbool.h>

#include "Defines.h"

#define MAXLEVELTEXTSIZE 500000

#define MAXLEVELS 200
#define MAXITEMCOUNT (NrOfCols*NrOfRows) + 2

#define MAXCOMMENTLEN 150
#define MAXSETLEN 50
#define MAXAUTHORLEN 75
#define MAXTITLELEN 50
#define MAXLEVELFIELDDATALEN 2000
#define MAXLEVELFIELDLEN 100
#define MAXLINELEN 500

#define LPFloor ' '
#define LPPlayer '@'
#define LPBox '$'
#define LPBomb '*'
#define LPWall '#'
#define LPDiamond 'd'
#define LPPlayer2 '&'
#define LPBox1 '1'
#define LPBox2 '2'
#define LPBoxBomb 'b'
#define LPBoxWall 'w'
#define LPBreakableWall 'x'



typedef struct LevelPart LevelPart;
struct LevelPart
{
	int id;
	int x;
	int y;
};

typedef struct LevelMeta LevelMeta;
struct LevelMeta
{
	int minx;
	int miny;
	int maxx;
	int maxy;
	int parts;
	char author[MAXAUTHORLEN];
	char comments[MAXCOMMENTLEN];
	char title[MAXTITLELEN];
};

typedef struct CLevelPackFile CLevelPackFile;
struct CLevelPackFile
{
	LevelPart Levels[MAXLEVELS][MAXITEMCOUNT];
	LevelMeta LevelsMeta[MAXLEVELS];
	char author[MAXAUTHORLEN];
	char set[MAXSETLEN];
	int LevelCount;
	bool Loaded;
};

CLevelPackFile* CLevelPackFile_Create();
void CLevelPackFile_Destroy(CLevelPackFile* LevelPackFile);
bool CLevelPackFile_parseText(CLevelPackFile* LevelPackFile, char* text, int maxWidth, int maxHeight, bool MetaOnly);
bool CLevelPackFile_loadFile(CLevelPackFile* LevelPackFile, char* filename, int maxWidth, int maxHeight, bool MetaOnly);

#endif