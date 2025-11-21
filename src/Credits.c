#include <string.h>
#include <stdbool.h>
#include "Credits.h"
#include "Common.h"
#include "GameFuncs.h"
#include "Sound.h"
#include "Pd_helperfuncs.h"
#include "pd_api.h"


char *CreditsTekst;
	
void CreditsInit()
{
	SelectMusic(musTitle);
	if(InstalledLevelPacksCount > 0)
	{
		if(strlen(NormalCreateName) > 0)		
		{
			pd->system->formatString(&CreditsTekst,"Blips was created by Willems Davy\nWillems Soft 2006-2025.\nHttps://joyrider3774.itch.io\n\nLevelpack %s\ncreated by %s",LevelPackName,NormalCreateName);
		}
		else
			pd->system->formatString(&CreditsTekst, "Blips was created by Willems Davy\nWillems Soft 2006-2025\nHttps://joyrider3774.itch.io");

	}
	else
		pd->system->formatString(&CreditsTekst, "Blips was created by Willems Davy\nWillems Soft 2006-2025\nHttps://joyrider3774.itch.io");
}

void Credits()
{
	
	if (GameState == GSCreditsInit)
	{
		CreditsInit();
		GameState = GSCredits;
	}

	if (((currButtons & kButtonA) && !(prevButtons & kButtonA)) ||
		((currButtons & kButtonB) && !(prevButtons & kButtonB)))
	{
		playMenuBackSound();
		GameState = GSTitleScreenInit;
	}
	pd->graphics->drawBitmap(IMGTitleScreen, 0, 0, kBitmapUnflipped);
	pd->graphics->fillRect(40 * UI_WIDTH_SCALE, 70 * UI_HEIGHT_SCALE, 240 * UI_WIDTH_SCALE, 100 * UI_HEIGHT_SCALE, kColorWhite);
	pd->graphics->drawRect(40 * UI_WIDTH_SCALE, 70 * UI_HEIGHT_SCALE, 240 * UI_WIDTH_SCALE, 100 * UI_HEIGHT_SCALE, kColorBlack);
	pd->graphics->drawRect(41 * UI_WIDTH_SCALE, 71.5 * UI_HEIGHT_SCALE, 238 * UI_WIDTH_SCALE, 97.5 * UI_HEIGHT_SCALE, kColorBlack);

	drawText(font,CreditsTekst,strlen(CreditsTekst), kASCIIEncoding, 45*UI_WIDTH_SCALE,75*UI_HEIGHT_SCALE);

	printTitleInfo();

	if (GameState != GSCredits)
	{
		pd->system->realloc(CreditsTekst, 0);
	}
}
