#include "Common.h"
#include "Defines.h"
#include "GameFuncs.h"

int IntroScreenNr = 0;

void IntroInit()
{
	IntroScreenNr = 0;
}

void Intro()
{
	if (GameState == GSIntroInit)
	{
		IntroInit();
		GameState = GSIntro;
	}
	
	if (currButtons & kButtonDown)
		showFPS = true;

	if((currButtons & kButtonA) || (currButtons & kButtonB))
	{
		GameState = GSTitleScreenInit;
	}

	if (framecount % 80 == 0)
	{
		IntroScreenNr++;
		if (IntroScreenNr > 3)
		{
			GameState = GSTitleScreenInit;
			return;
		}
	}

	switch(IntroScreenNr)
	{
		case 1 :
			pd->graphics->drawBitmap(IMGIntro1,0,0, kBitmapUnflipped);
			break;
		case 2 :
			pd->graphics->drawBitmap(IMGIntro2, 0, 0, kBitmapUnflipped);
			break;
		case 3 :
			pd->graphics->drawBitmap(IMGIntro3, 0, 0, kBitmapUnflipped);
			break;
    }
}