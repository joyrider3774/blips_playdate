#include <string.h>
#include <stdbool.h>
#include "Credits.h"
#include "Common.h"
#include "GameFuncs.h"
#include "Sound.h"
#include "Pd_helperfuncs.h"
#include "pd_api.h"
	
void InstructionsInit()
{
	SelectMusic(musTitle);	
}

void Instructions()
{
	
	if (GameState == GSInstructionsInit)
	{
		InstructionsInit();
		GameState = GSInstructions;
	}

	if (((currButtons & kButtonA) && !(prevButtons & kButtonA)) ||
		((currButtons & kButtonB) && !(prevButtons & kButtonB)))
	{
		playMenuBackSound();
		GameState = GSTitleScreenInit;
	}
	pd->graphics->drawBitmap(IMGInstructions, 0, 0, kBitmapUnflipped);

}
