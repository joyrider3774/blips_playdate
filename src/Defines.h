#ifndef DEFINES_H
#define DEFINES_H


#define ORIG_WINDOW_WIDTH 400
#define ORIG_WINDOW_HEIGHT 240 
#define TileWidth 16
#define TileHeight 16
#define FPS 30
#define UI_WIDTH_SCALE 1.25
#define UI_HEIGHT_SCALE 1.10

#define NrOfRows 50
#define NrOfCols 50
#define NrOfColsVisible (ORIG_WINDOW_WIDTH / TileWidth)
#define NrOfRowsVisible ((ORIG_WINDOW_HEIGHT / TileHeight))

#define IDEmpty 1
#define IDPlayer 2
#define IDBox 3
#define IDFloor 4
#define IDBomb 5
#define IDWall 6
#define IDDiamond 7
#define IDPlayer2 8
#define IDBox1 9
#define IDBox2 10
#define IDBoxBomb 11
#define IDBoxWall 12
#define IDWallBreakable 13
#define IDExplosion 14

#define ZPlayer 10
#define ZDiamond 3
#define ZWall 4
#define ZBox 6
#define ZBomb 3
#define ZFloor 1
#define ZBoxBomb 5
#define ZExplosion 15

#define MaxLevelPacks 200
#define MaxMusicFiles 26

#define MaxLevelPackNameLength 21

#define IDSolvedLevelLevelEditorMode 1
#define IDSolvedLevelNextUnlocked 2
#define IDSolvedLastLevel 3
#define IDSolvedEarlierLevel 4
#define IDNoPlayer 5
#define IDNoDiamonds 6
#define IDNoLevelsInPack 7
#define IDCurrentLevelNotSaved 8
#define IDRestartLevel 9
#define IDDeleteAllParts 10
#define IDDeleteLevelPack 11
#define IDDeleteLevel 12
#define IDLevelNotUnlocked 13
#define IDPlayerDiedLevelEditorMode 14
#define IDPlayerDied 15

#define MenuUpdateTicks 5
#define LevelEditorUpdateTicks 4

#endif