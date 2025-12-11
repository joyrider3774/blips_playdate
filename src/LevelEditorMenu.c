#include <string.h>
#include <stdbool.h>
#include "LevelEditorMenu.h"
#include "Common.h"
#include "GameFuncs.h"
#include "Sound.h"
#include "Pd_helperfuncs.h"
#include "pd_api.h"
#include "Crank.h"
#include "LevToBip.h"

char *PackName,*CreatorName;
char* File1, * File2;
void LevelEditorMenuInit()
{
	setCrankMoveThreshold(180);
	Selection = 1;
}

void LevelEditorMenu()
{
	int Teller;
	char* FileName;
	char* Tekst;
	
	if(GameState == GSLevelEditorMenuInit)
	{
		LevelEditorMenuInit();
		GameState = GSLevelEditorMenu;
	}

	int CrankMove = crankUpdate();
	int id = -1;
	bool response = false;
	if(!AskingQuestion && !AskingGetString)
	{
		pd->graphics->drawBitmap(IMGTitleScreen,0,0,kBitmapUnflipped);
		printTitleInfo();
		pd->graphics->fillRect(40 * UI_WIDTH_SCALE, 62 * UI_HEIGHT_SCALE, 240 * UI_WIDTH_SCALE, 116 * UI_HEIGHT_SCALE, kColorWhite);
		pd->graphics->drawRect(40 * UI_WIDTH_SCALE, 62 * UI_HEIGHT_SCALE, 240 * UI_WIDTH_SCALE, 116 * UI_HEIGHT_SCALE, kColorBlack);
		pd->graphics->drawRect(42 * UI_WIDTH_SCALE, 64 * UI_HEIGHT_SCALE, 236 * UI_WIDTH_SCALE, 112 * UI_HEIGHT_SCALE, kColorBlack);
		pd->system->formatString(&Tekst,"Create New Levelpack\nLoad Selected LevelPack\nDelete Selected Levelpack\nExport to Bip File\n<%s>\nMain Menu",LevelPackName);
		drawText(BigFont, Tekst, strlen(Tekst), kASCIIEncoding, 70 * UI_WIDTH_SCALE, 69 * UI_HEIGHT_SCALE);
		pd->system->realloc(Tekst, 0);
		Tekst = pd->system->realloc(NULL, 100);
		if (Selection > 1)
		{
			strcpy(Tekst,"\n");
			for(Teller=2;Teller<Selection;Teller++)
				strcat(Tekst,"\n");
			strcat(Tekst,">>");
		}
		else
			strcpy(Tekst,">>");
		drawText(BigFont, Tekst, strlen(Tekst), kASCIIEncoding, 45 * UI_WIDTH_SCALE, 69 * UI_HEIGHT_SCALE);
		pd->system->realloc(Tekst, 0);

		int numLeft = ConvertLevToBipContinue();
		if (numLeft > 0)
		{
			pd->system->formatString(&Tekst, "Exporting levels %d remaining", numLeft);
			//-1 is without user input required
			AskQuestion(-1, Tekst);
			pd->system->realloc(Tekst, 0);
			return;
		}
		
		if (numLeft == 0)
		{
			playMenuSelectSound();
			pd->system->formatString(&Tekst, "Levelpack %s was exported\nto following location:\n<gamedata>/%s\n\nPress (A) to continue", LevelPackName, File2);
			AskQuestion(IDBipExported, Tekst);
			pd->system->realloc(Tekst, 0);
			pd->system->realloc(File1, 0);
			pd->system->realloc(File2, 0);
			char* tmp;
			pd->system->formatString(&tmp, "%s", LevelPackName);
			SearchForLevelPacks();
			for (Teller = 0; Teller < InstalledLevelPacksCount; Teller++)
				if (strcmp(tmp, InstalledLevelPacks[Teller]) == 0)
				{
					SelectedLevelPack = Teller;
					if (LevelPackName)
						pd->system->realloc(LevelPackName, 0);
					pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
				}
			pd->system->realloc(tmp, 0);
			return;
		}
		
		
		if ((currButtons & kButtonB) && !(prevButtons & kButtonB))
		{
			playMenuBackSound();
			GameState = GSTitleScreenInit;
		}

		if (currButtons & kButtonLeft)
		{
			if (!(prevButtons & kButtonLeft))
				frameLeftStart = framecount;
			if ((framecount - frameLeftStart) % MenuUpdateTicks == 0)
			{
				if (Selection == 5)
					if (InstalledLevelPacksCount > 0)
					{
						SelectedLevelPack--;
						if (SelectedLevelPack < 0)
							SelectedLevelPack = InstalledLevelPacksCount - 1;
						if (LevelPackName)
							pd->system->realloc(LevelPackName, 0);
						pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, true);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
						playMenuSound();
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
				if (Selection == 5)
					if (InstalledLevelPacksCount > 0)
					{
						SelectedLevelPack++;
						if (SelectedLevelPack > InstalledLevelPacksCount - 1)
							SelectedLevelPack = 0;
						if (LevelPackName)
							pd->system->realloc(LevelPackName, 0);
						pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, true);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
						playMenuSound();
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
				if (Selection < 6)
				{
					Selection++;
					playMenuSound();
				}
			}
		}

		if ((GameState == GSLevelEditorMenu) && (currButtons & kButtonA) && !(prevButtons & kButtonA))
		{
			switch(Selection)
			{
				case 1:
					playMenuSelectSound();
					PackName = GetString(1, 55*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE, "Enter the Levelpack name:", 19);										
					break;
				case 2:
					if (InstalledLevelPacksCount > 0)
					{
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, false);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
						FindLevels();
						SelectedLevel = 0;
						GameState = GSStageSelectInit;
						playMenuSelectSound();
					}
					break;
				case 3:
					if (InstalledLevelPacksCount >0)
					{
						playMenuSelectSound();
						pd->system->formatString(&Tekst,"do you want to delete the selected level pack:\n\"%s\"\nAll Levels in Levelpack will be deleted !!!\n\nPress (A) to Delete, (B) to Cancel",InstalledLevelPacks[SelectedLevelPack]);
						AskQuestion(IDDeleteLevelPack, Tekst);
						pd->system->realloc(Tekst, 0);
					}
					break;
				case 4:
					playMenuSelectSound();
					char* dot = strrchr(LevelPackName, '.');
					bool canContinue = true;
					if (dot)
					{
						if ((strcmp(dot, ".bip") == 0) || (strcmp(dot, ".BIP") == 0) ||
							(strcmp(dot, ".txt") == 0) || (strcmp(dot, ".TXT") == 0))
						{
							canContinue = false;
						}
					}
					if (canContinue)
					{
						//-1 is without user input required
						AskQuestion(-1, "Exporting levels");
						pd->system->formatString(&File1, "levelpacks/%s._lev", LevelPackName);
						pd->system->formatString(&File2, "levelpacks/%s.bip", LevelPackName);
						ConvertLevToBip(File1, File2, LevelPackName, NormalCreateName);
					}
					else
						AskQuestion(IDBipRefuseExport, "Ascii (*.bip, *.txt) levelpacks can not be exported to bip files\nOnly completely self created levelpacks can be exported!\n\nPress (A) to continue");
					break;
				case 5:
					if (InstalledLevelPacksCount > 0)
					{
						SelectedLevelPack++;
						if (SelectedLevelPack > InstalledLevelPacksCount - 1)
							SelectedLevelPack = 0;
						if (LevelPackName)
							pd->system->realloc(LevelPackName, 0);
						pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, true);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
						playMenuSound();
						SaveSettings();
					}
					break;
				case 6:
					playMenuSelectSound();
					GameState = GSTitleScreenInit;
					break;
			}
		}
	}
	else
	{
		if (GetStringId == 2)
		{
			id = -1;
			response = false;
			if (getStringUpdate(&id, &response, CreatorName))
			{
				if (response)
				{
					if (strlen(CreatorName) > 0)
					{
						if (LevelPackName)
							pd->system->realloc(LevelPackName, 0);
						pd->system->formatString(&LevelPackName, "%s", PackName);
						pd->file->mkdir("levelpacks");
						pd->system->formatString(&Tekst, "levelpacks/%s._lev", LevelPackName);
						pd->file->mkdir(Tekst);
						pd->system->realloc(Tekst, 0);
						pd->system->formatString(&FileName, "levelpacks/%s._lev/credits.dat", LevelPackName);
						SDFile* Fp = pd->file->open(FileName, kFileWrite);
						if (Fp)
						{
							pd->file->write(Fp, CreatorName, (unsigned int)(strlen(CreatorName) + 1));
							pd->file->close(Fp);
						}
						pd->system->realloc(FileName, 0);
						SearchForLevelPacks();
						for (Teller = 0; Teller < InstalledLevelPacksCount; Teller++)
							if (strcmp(PackName, InstalledLevelPacks[Teller]) == 0)
							{
								SelectedLevelPack = Teller;
								if (LevelPackName)
									pd->system->realloc(LevelPackName, 0);
								pd->system->formatString(&LevelPackName, "%s", InstalledLevelPacks[SelectedLevelPack]);
							}
					}
				}
				pd->system->realloc(PackName, 0);
				pd->system->realloc(CreatorName, 0);
				if (InstalledLevelPacksCount > 0)
				{
					pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
					CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, true);
					pd->system->realloc(FileName, 0);
					LoadNormalCreatorName();
				}
				LoadNormalCreatorName();
			}
		}

		if (GetStringId == 1)
		{
			id = -1;
			response = false;
			if (getStringUpdate(&id, &response, PackName))
			{
				if (response)
				{
					if (strlen(PackName) > 0)
					{
						CreatorName = GetString(2, 55 * UI_WIDTH_SCALE, 85 * UI_HEIGHT_SCALE, "Enter the Levelpack Creator name:", 19);
					}
					else
						pd->system->realloc(PackName, 0);
				}
				else
					pd->system->realloc(PackName, 0);				
			}
		}

		if(AskQuestionUpdate(&id, &response, false))
		{
			if(id == IDDeleteLevelPack)
			{
				if (response)
				{
					FindLevels();
					char* dot = strrchr(LevelPackName, '.');
					if (dot && ((strcmp(dot, ".bip") == 0) || (strcmp(dot, ".BIP") == 0) ||
						(strcmp(dot, ".txt") == 0) || (strcmp(dot, ".TXT") == 0)))
					{
						pd->system->formatString(&Tekst, "levelpacks/%s", LevelPackName);
						pd->file->unlink(Tekst, true);
					}
					
					pd->system->formatString(&Tekst, "levelpacks/%s._lev", LevelPackName);
					pd->file->unlink(Tekst, true);
					pd->system->realloc(Tekst, 0);
					SearchForLevelPacks();
					if (InstalledLevelPacksCount > 0)
					{
						pd->system->formatString(&FileName, "levelpacks/%s", LevelPackName);
						CLevelPackFile_loadFile(LevelPackFile, FileName, NrOfCols, NrOfRows, true);
						pd->system->realloc(FileName, 0);
						LoadNormalCreatorName();
					}
					LoadNormalCreatorName();
				}
			}
		}
	}
}

