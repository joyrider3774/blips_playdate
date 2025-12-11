#include <string.h>
#include <stdbool.h>
#include "Credits.h"
#include "Common.h"
#include "GameFuncs.h"
#include "Sound.h"
#include "Pd_helperfuncs.h"
#include "pd_api.h"
#include "CLevelPackFile.h"


char *CreditsTekst, *CreditsTekst2, *CreditsTekst3;
	
void CreditsInit()
{
	SelectMusic(musTitle);
	pd->system->formatString(&CreditsTekst, "Blips was created by Willems Davy (2006-2025)\nHttps://joyrider3774.itch.io");
	pd->system->formatString(&CreditsTekst2, "Original games by Bryant Brownell\nbryant.brownell@gmail.com");
	if (InstalledLevelPacksCount > 0)
	{
		if (!LevelPackFile->Loaded)
		{
			pd->system->formatString(&CreditsTekst3, "Levelpack %s\ncreated by %s", LevelPackName, NormalCreateName);
		}
		else
			pd->system->formatString(&CreditsTekst3, "Levelpack %s\ncreated by %s", LevelPackName, LevelPackFile->author);
	}
	else
		pd->system->formatString(&CreditsTekst3, "Levelpack %s\ncreated by unknown author", LevelPackName);
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
	pd->graphics->drawRect(42 * UI_WIDTH_SCALE, 72 * UI_HEIGHT_SCALE, 237 * UI_WIDTH_SCALE, 97 * UI_HEIGHT_SCALE, kColorBlack);

	drawText(font,CreditsTekst,strlen(CreditsTekst), kASCIIEncoding, 45*UI_WIDTH_SCALE,75*UI_HEIGHT_SCALE);
	drawText(font, CreditsTekst2, strlen(CreditsTekst2), kASCIIEncoding, 45 * UI_WIDTH_SCALE, 107 * UI_HEIGHT_SCALE);
	drawText(font, CreditsTekst3, strlen(CreditsTekst3), kASCIIEncoding, 45 * UI_WIDTH_SCALE, 139 * UI_HEIGHT_SCALE);
	
	printTitleInfo();

	if (GameState != GSCredits)
	{
		pd->system->realloc(CreditsTekst, 0);
		pd->system->realloc(CreditsTekst2, 0);
		pd->system->realloc(CreditsTekst3, 0);
	}
}
