#include "pd_api.h"
#include "Defines.h"
#include "Blips.h"
#include "Common.h"
#include "Pd_helperfuncs.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

int eventHandler(PlaydateAPI* playdate, PDSystemEvent event, uint32_t arg)
{

	if ( event == kEventInit )
	{
		setPDPtr(playdate);
		playdate->display->setRefreshRate(FPS);
		playdate->system->setUpdateCallback(mainLoop, NULL);
		setupGame();
	}

	if (event == kEventPause)
	{
		paused = true;
	}

	if (event == kEventResume)
	{
		paused = false;
	}

	if (event == kEventTerminate)
	{
		terminateGame();
	}
	
	return 0;
}
