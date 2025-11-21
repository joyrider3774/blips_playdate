#include <string.h>
#include <stdbool.h>
#include "Defines.h"
#include "TitleScreen.h"
#include "Common.h"
#include "GameFuncs.h"
#include "Pd_helperfuncs.h"
#include "Sound.h"
#include "pd_api.h"
#include "Crank.h"

void OptionsInit()
{
	setCrankMoveThreshold(180);
	Selection = 1;
}


void Options()
{
    int Teller;
	char* Tekst;

	if (GameState == GSOptionsInit)
	{
		OptionsInit();
		GameState = GSOptions;
	}
	int CrankMove = crankUpdate();
	int id = -1;
	bool response = false;
	pd->graphics->drawBitmap(IMGTitleScreen,0,0,kBitmapUnflipped);
	printTitleInfo();

	pd->graphics->fillRect(40*UI_WIDTH_SCALE,70*UI_HEIGHT_SCALE,240*UI_WIDTH_SCALE,100*UI_HEIGHT_SCALE,kColorWhite);
	pd->graphics->drawRect(40*UI_WIDTH_SCALE,70*UI_HEIGHT_SCALE,240*UI_WIDTH_SCALE,100*UI_HEIGHT_SCALE,kColorBlack);
	pd->graphics->drawRect(42*UI_WIDTH_SCALE,72*UI_HEIGHT_SCALE,237*UI_WIDTH_SCALE,97*UI_HEIGHT_SCALE,kColorBlack);
	pd->system->formatString(&Tekst,"Music: %s\nSound: %s\nMain Menu",isMusicOn() ? "On" : "Off", isSoundOn() ? "On" : "Off");
	drawText(BigFont,Tekst,strlen(Tekst),kASCIIEncoding,70*UI_WIDTH_SCALE,77*UI_HEIGHT_SCALE);
	pd->system->realloc(Tekst, 0);
	Tekst = pd->system->realloc(NULL, 100);
	if (Selection > 1)
	{			
		strcpy(Tekst," \n");
		for(Teller=2;Teller<Selection;Teller++)
			strcat(Tekst,"\n");
		strcat(Tekst,">>");
	}
	else
		strcpy(Tekst,">>");
	drawText(BigFont,Tekst,strlen(Tekst),kASCIIEncoding, 45*UI_WIDTH_SCALE,77*UI_HEIGHT_SCALE);
	pd->system->realloc(Tekst, 0);
		
	if (currButtons & kButtonLeft)
	{
		if (!(prevButtons & kButtonLeft))
			frameLeftStart = framecount;
		if ((framecount - frameLeftStart) % MenuUpdateTicks == 0)
		{
			if (Selection == 1)
			{
				playMenuSelectSound();
				setMusicOn(!isMusicOn());
				SaveSettings();
			}

			if (Selection == 2)
			{
				playMenuSelectSound();
				setSoundOn(!isSoundOn());
				SaveSettings();
			}
		}
	}
		
	if (currButtons & kButtonRight)
	{
		if (!(prevButtons & kButtonRight))
			frameRightStart = framecount;
		if ((framecount - frameRightStart) % MenuUpdateTicks == 0)
		{
			if (Selection == 1)
			{
				playMenuSelectSound();
				setMusicOn(!isMusicOn());
				SaveSettings();
			}

			if (Selection == 2)
			{
				playMenuSelectSound();
				setSoundOn(!isSoundOn());
				SaveSettings();
			}
		}
	}
	if ((currButtons & kButtonUp) || (CrankMove == CRANKMOVELEFT))
	{
		if (!(prevButtons & kButtonUp) && !(CrankMove == CRANKMOVELEFT))
			frameUpStart = framecount;
		if (((framecount - frameUpStart) % MenuUpdateTicks == 0) || (CrankMove == CRANKMOVELEFT))
		{
			if (Selection > 1)
			{
				Selection--;
				playMenuSound();
			}
		}
	}

	if ((currButtons & kButtonDown) || (CrankMove == CRANKMOVERIGHT))
	{
		if (!(prevButtons & kButtonDown) && !(CrankMove == CRANKMOVERIGHT))
			frameDownStart = framecount;
		if (((framecount - frameDownStart) % MenuUpdateTicks == 0) || (CrankMove == CRANKMOVERIGHT))
		{
			if (Selection < 3)
			{
				Selection++;
				playMenuSound();
			}
		}
	}
	
	if ((currButtons & kButtonB) && !(prevButtons & kButtonB))
	{
		playMenuBackSound();
		GameState = GSTitleScreenInit;
	}

	if ((currButtons & kButtonA) && !(prevButtons & kButtonA))
	{
		switch(Selection)
		{
			case 1:
				if (Selection == 1)
				{
					playMenuSelectSound();
					setMusicOn(!isMusicOn());
					SaveSettings();
				}
				break;
			case 2:
				if (Selection == 2)
				{
					playMenuSelectSound();
					setSoundOn(!isSoundOn());
					SaveSettings();
				}
				break;
			case 3:
				GameState=GSTitleScreenInit;
				playMenuSelectSound();
				break;
			default:
				break;
		}
	}
	
}
