#ifndef LEVTOBIP_H
#define LEVTOBIP_H

typedef struct Level Level;
struct Level {
    int map[NrOfRows][NrOfCols];
    int min_x, min_y;
    int max_x, max_y;
    int player_count;
} ;

void ConvertLevToBip(char* dir_path, char* output_file, char* collectionName, char* author);
int ConvertLevToBipContinue();

#endif