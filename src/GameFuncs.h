#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H

#include <stdbool.h>
#include <stddef.h>

void UnLoadGraphics();
void FindLevels();
int ord(char chr);
char chr(int ascii);
char* GetString(int Id, int x, int y, char* Msg, size_t MaxLen);
bool getStringUpdate(int* Id, bool* Answered, char* StringBuffer);
void printTitleInfo();
void LoadUnlockData();
void SaveUnlockData();
void AskQuestion(int Id, char* Msg);
bool AskQuestionUpdate(int* Id, bool* Answer, bool MustBeAButton);
void SearchForLevelPacks();
void SaveSettings();
void LoadSettings();
void LoadNormalCreatorName();
void LoadGraphics();

#endif