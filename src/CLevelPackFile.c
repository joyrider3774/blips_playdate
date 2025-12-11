#include <stdbool.h>
#include <string.h>
#include "Common.h"
#include "GameFuncs.h"
#include "CLevelPackFile.h"
#include "Defines.h"
#include "pd_api.h"

CLevelPackFile* CLevelPackFile_Create()
{
	CLevelPackFile* Result = (CLevelPackFile*)pd->system->realloc(NULL, sizeof(CLevelPackFile));
	pd->system->logToConsole("sizeof(CLevelPackFile):%d\n", sizeof(CLevelPackFile));
	Result->Loaded = false;
	Result->LevelCount = 0;
	memset(Result->author, 0, MAXAUTHORLEN);
	memset(Result->set, 0, MAXSETLEN);
	return Result;
}

void CLevelPackFile_Destroy(CLevelPackFile* LPackFile)
{
	if(LPackFile)
	{
		pd->system->realloc(LPackFile, 0);
		LPackFile = NULL;
	}
}

bool CLevelPackFile_loadFile(CLevelPackFile* LPackFile, char* filename, int maxWidth, int maxHeight, bool MetaOnly)
{
	bool Result = false;
	LPackFile->LevelCount = 0;
	LPackFile->Loaded = false;
	memset(LPackFile->author, 0, MAXAUTHORLEN);
	memset(LPackFile->set, 0, MAXSETLEN);
	SDFile* Fp = pd->file->open(filename,kFileReadData);
	if(!Fp)
		Fp = pd->file->open(filename, kFileRead);
	if(Fp)
	{
		pd->file->seek (Fp , 0 , SEEK_END);
		long FileSize = pd->file->tell (Fp);
		pd->file->seek(Fp, 0, SEEK_SET);
		char* tmp = pd->system->realloc(NULL, sizeof(char) *(FileSize + 2));
		pd->file->read(Fp, tmp,FileSize);
		tmp[FileSize] = '\0';
		Result = CLevelPackFile_parseText(LPackFile, tmp, maxWidth, maxHeight, MetaOnly);		
		pd->system->realloc(tmp, 0);
		pd->file->close(Fp);
		LPackFile->Loaded = true;
	}

	return Result;
}

bool CLevelPackFile_parseText(CLevelPackFile *LPackFile, char* text, int maxWidth, int maxHeight, bool MetaOnly)
{
	char line[MAXLINELEN] = "";
	char levelField[MAXLEVELFIELDLEN] = "";
	char levelFieldValue[MAXLEVELFIELDDATALEN] = "";
	int linepos;
	char* pchar = text;
	char* pdoublepoint, *pset, *pauthor;
	int y;
	bool inlevel = false;
	LPackFile->LevelCount = 0;
	memset(LPackFile->author, 0, MAXAUTHORLEN);
	memset(LPackFile->set, 0, MAXSETLEN);
	LevelMeta* levelMeta = &(LPackFile->LevelsMeta)[LPackFile->LevelCount];
	while(*pchar)
	{
		linepos = 0;
		while((*pchar != '\n') && (*pchar != '\0'))
		{
			if((*pchar != '\r') && (linepos < MAXLINELEN-1))
			{
				if((*pchar >= 'A') && (*pchar <= 'Z'))
				{
					line[linepos++] = *pchar + 32;
				}
				else
					line[linepos++] = *pchar;
            }
            pchar++;			
		}

		if(*pchar == '\0')
			break;

		pchar++;

		line[linepos] = '\0';

		if(LPackFile->LevelCount == 0)
		{
			if(!LPackFile->set[0])
			{
				pset = strstr(line, "set:");
				if(pset)
				{
					pset+= 4;
					while(*pset == ' ')
						pset++;
					strncpy(LPackFile->set, pset, MAXSETLEN-1);
				}
			}

			if(!LPackFile->author[0])
			{
				pauthor = strstr(line, "author:");
				if(pauthor)
				{
					pauthor+= 7;
					while(*pauthor == ' ')
						pauthor++;
					strncpy(LPackFile->author, pauthor, MAXAUTHORLEN-1);
				}
			}
		}

		//found double point while in a level start a metadata field
		pdoublepoint = strstr(line, ":");			
		if(inlevel && pdoublepoint)
		{
			if(levelField[0])
			{
				char* ptmp = levelFieldValue;
				while(*ptmp == ' ')
					ptmp++;
				if (strcmp(levelField, "title") == 0)
				{
					strncpy(levelMeta->title, ptmp, MAXTITLELEN-1);
				}
				else
				{
					if (strcmp(levelField, "author") == 0)
					{
						strncpy(levelMeta->author, ptmp, MAXAUTHORLEN-1);
					}
					else
					{
						if (strcmp(levelField, "comment") == 0)
						{
							strncpy(levelMeta->comments, ptmp, MAXCOMMENTLEN-1);
						}
					}
				}
			}	
			memset(levelFieldValue, 0, MAXLEVELFIELDDATALEN);
			memset(levelField, 0, MAXLEVELFIELDLEN);
			strncpy(levelField, line, pdoublepoint - &line[0]);
			strncpy(levelFieldValue, pdoublepoint +1, MAXLEVELFIELDDATALEN - 1);
			continue;
		}
		
		//we are in a level but found no empty line and no doublepoint then we are then in a multiline metadata field just append its value
		if(inlevel && linepos && !pdoublepoint && (levelField[0]))
		{
			if(levelFieldValue[0])
				strcat(levelFieldValue, "\n");
			strcat(levelFieldValue, line);
			continue;
		}

		//we are in a level and found a empty line then assume level end
		if(inlevel && !linepos)
		{
			if(levelField[0])
			{
				char* ptmp = levelFieldValue;
				while(*ptmp == ' ')
					ptmp++;
				if (strcmp(levelField, "title") == 0)
				{
					strncpy(levelMeta->title, ptmp, MAXTITLELEN-1);
				}
				else
				{
					if (strcmp(levelField, "author") == 0)
					{
						strncpy(levelMeta->author, ptmp, MAXAUTHORLEN-1);
					}
					else
					{
						if (strcmp(levelField, "comment") == 0)
						{
							strncpy(levelMeta->comments, ptmp, MAXCOMMENTLEN-1);
						}
					}
				}
			}
			//clear them for if condition above conerning level start
			memset(levelFieldValue, 0, MAXLEVELFIELDDATALEN);
			memset(levelField, 0, MAXLEVELFIELDLEN);
			inlevel = false;
			if((levelMeta->maxx+1 <= maxWidth) && 
				(levelMeta->maxy+1 <= maxHeight))
			{
				LPackFile->LevelCount++;
				//don't exceed limits
				if (LPackFile->LevelCount >= MAXLEVELS)
					return true;
				levelMeta = &(LPackFile->LevelsMeta)[LPackFile->LevelCount];
			}
			continue;
		}
		
		//we are not in a level and found a wall and no doublepoint and we are not in a levelfield then assume levelstart
		if (!inlevel && !pdoublepoint && (!levelField[0]))
		{
			if (strchr(line, LPWall))
			{
				if (MetaOnly)
					return true;
				inlevel=true;
				y = 0;
				levelMeta->minx = NrOfCols;
				levelMeta->miny = NrOfRows;
				levelMeta->maxx = 0;
				levelMeta->maxy = 0;
				memset(levelMeta->author, 0, MAXAUTHORLEN);
				memset(levelMeta->title, 0, MAXTITLELEN);
				memset(levelMeta->comments, 0, MAXCOMMENTLEN);
				levelMeta->parts = 0;
			}
		}

		//we are in level and not in a level meta field
		if(inlevel && (!levelField[0]))
		{			
			for(int x = 0; x < linepos; x++)
			{
				if (line[x] == LPFloor)
					continue;
				//DON'T EXCEED MAX ITEMCOUNT!
				if(levelMeta->parts+2 >= MAXITEMCOUNT)
				{
					levelMeta->maxx = 1000;
					break;
				}
				LevelPart* levelPart = &(LPackFile->Levels[LPackFile->LevelCount][LPackFile->LevelsMeta[LPackFile->LevelCount].parts]);
				
				switch(line[x])
				{
					case LPWall:
						if(x < levelMeta->minx)
							levelMeta->minx = x;
						if(x > levelMeta->maxx)
							levelMeta->maxx = x;
						if(y < levelMeta->miny)
							levelMeta->miny = y;
						if(y > levelMeta->maxy)
							levelMeta->maxy = y;
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDWall;
						levelMeta->parts++;
						break;
					case LPBox:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDBox;
						levelMeta->parts++;
						break;
					case LPBomb:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDBomb;
						levelMeta->parts++;
						break;
					case LPDiamond:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDDiamond;
						levelMeta->parts++;
						break;
					case LPBox1:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDBox1;
						levelMeta->parts++;
						break;
					case LPBox2:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDBox2;
						levelMeta->parts++;
						break;
					case LPBoxBomb:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDBoxBomb;
						levelMeta->parts++;
						break;
					case LPBoxWall:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDBoxWall;
						levelMeta->parts++;
						break;
					case LPBreakableWall:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDWallBreakable;
						levelMeta->parts++;
						break;
					case LPPlayer:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDPlayer;
						levelMeta->parts++;
						break;
					case LPPlayer2:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDPlayer2;
						levelMeta->parts++;
						break;
				}
			}
			y++;
		}
	}
	return LPackFile->LevelCount > 0;
}

