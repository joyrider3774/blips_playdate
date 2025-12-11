#include <limits.h>
#include <stdlib.h>
#include "Defines.h"
#include "pd_api.h"
#include "Common.h"
#include "LevToBip.h"

char** _filenames = NULL;
int _file_count = 0;
int _capacity = 100;
SDFile* _outputfile;
int _level_num = 0;
char* _author;
Level level;

// ASCII representation mapping
char tile_to_char(int tile) 
{
    switch (tile) {
    case IDPlayer: return '@';
    case IDBox: return '$';
    case IDBomb: return '*';
    case IDWall: return '#';
    case IDDiamond: return 'd';
    case IDPlayer2: return '&';
    case IDBox1: return '1';
    case IDBox2: return '2';
    case IDBoxBomb: return 'b';
    case IDBoxWall: return 'w';
    case IDWallBreakable: return 'x';
    default: return ' ';  // Empty space for anything else
    }
}

bool load_lev_file(const char* filename, Level* level) 
{
    SDFile* f = pd->file->open(filename, kFileReadData);
    if (!f) {
        return false;
    }

    pd->file->seek(f, 0, SEEK_END);
    int file_size = pd->file->tell(f);
    pd->file->seek(f, 0, SEEK_SET);

    char* buffer = pd->system->realloc(NULL, file_size);
    size_t bytes_read = pd->file->read(f, buffer, file_size);
    pd->file->close(f);

    if (bytes_read < 3) {
        pd->system->realloc(buffer, 0);
        return false;
    }

    // Initialize map to all spaces (empty)
    for (int y = 0; y < NrOfRows; y++) {
        for (int x = 0; x < NrOfCols; x++) {
            level->map[y][x] = 0;  // 0 means empty space (not a tile)
        }
    }

    // First pass: find min/max coordinates (skip IDFloor only)
    int min_x = INT_MAX, min_y = INT_MAX;
    int max_x = INT_MIN, max_y = INT_MIN;

    for (size_t i = 0; i + 2 < bytes_read; i += 3) 
    {
        int tile = buffer[i];
        int x = buffer[i + 1];
        int y = buffer[i + 2];

        // Skip only IDFloor (4)
        if (tile == IDFloor) 
            continue;

        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
    }

    // If no valid tiles found
    if (min_x == INT_MAX) 
    {
        pd->system->realloc(buffer, 0);
        return false;
    }

    // Normalize to start from 0
    level->min_x = 0;
    level->min_y = 0;
    level->max_x = max_x - min_x;
    level->max_y = max_y - min_y;
    level->player_count = 0;

    // Second pass: populate map using normalized coordinates
    // Skip IDFloor(4) AND unknown tile IDs to prevent overwriting valid tiles

    for (size_t i = 0; i + 2 < bytes_read; i += 3) 
    {
        int tile = buffer[i];
        int x = buffer[i + 1];
        int y = buffer[i + 2];

        // Skip IDFloor (4) as per game code
        if (tile == IDFloor) 
            continue;

        // Skip unknown tile IDs - only process known tiles
        if (tile != IDPlayer && tile != IDBox &&
            tile != IDBomb && tile != IDWall && tile != IDDiamond &&
            tile != IDPlayer2 && tile != IDBox1 && tile != IDBox2 &&
            tile != IDBoxBomb && tile != IDBoxWall && tile != IDWallBreakable) 
        {
            continue;  // Skip unknown tiles (14+)
        }

        // Normalize coordinates by subtracting min values
        int norm_x = x - min_x;
        int norm_y = y - min_y;

        if (norm_x >= 0 && norm_x < NrOfCols && norm_y >= 0 && norm_y < NrOfRows) 
        {
            // Place tile at normalized position
            // If there are multiple tiles at same position, last one wins
            level->map[norm_y][norm_x] = tile;

            // Count players
            if (tile == IDPlayer || tile == IDPlayer2) 
            {
                level->player_count++;
            }
        }
    }

    pd->system->realloc(buffer, 0);
    return true;
}

void pdfprintf(SDFile* f, const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    char* Buf;
    pd->system->vaFormatString(&Buf, format, argptr);
    va_end(argptr);
    pd->file->write(f, Buf, (sizeof(char) * (unsigned int) strlen(Buf)));
    pd->system->realloc(Buf, 0);
}

void write_level_ascii(SDFile* f, const Level* level, int level_num, const char* author, const char* filename) 
{
    // Write level number
    pdfprintf(f, "%d\n", level_num);

    // Write level map - ALL rows from min to max
    for (int y = level->min_y; y <= level->max_y; y++) {
        for (int x = level->min_x; x <= level->max_x; x++) {
            pdfprintf(f, "%c", tile_to_char(level->map[y][x]));
        }
        pdfprintf(f, "\n");
    }

    // Extract level name from filename (e.g., "level1.lev" -> "level 1")
    char level_name[256];
    const char* base = strrchr(filename, '/');
    if (base) base++; else base = filename;

    const char* dot = strrchr(base, '.');
    if (dot) 
    {
        size_t len = dot - base;
        strncpy(level_name, base, len);
        level_name[len] = '\0';
        
        size_t i = 0;
        while (level_name[i])
        {
            if (level_name[i] >= '0' && level_name[i] <= '9')
            {
                size_t j = strlen(level_name) + 1;
                if (j < 255)
                {
                    level_name[j + 1] = '\0';
                    while (j > i)
                    {
                        level_name[j] = level_name[j - 1];
                        j--;
                    }
                    level_name[j] = ' ';
                    break;
                }
            }
            i++;
        }

    }
    else 
    {
        strcpy(level_name, base);
    }

    pdfprintf(f, "Title: %s\n", level_name);
    pdfprintf(f, "Author: %s\n", author);
    pdfprintf(f, "Comment:\n");
    pdfprintf(f, "\n");
}

int compare_filenames(const void* a, const void* b) {
    const char* str_a = *(const char**)a;
    const char* str_b = *(const char**)b;

    // Extract just the filename without path
    const char* name_a = strrchr(str_a, '/');
    const char* name_b = strrchr(str_b, '/');
    name_a = name_a ? name_a + 1 : str_a;
    name_b = name_b ? name_b + 1 : str_b;

    // Natural sort: compare numeric parts as numbers
    while (*name_a && *name_b) {
        // If both are digits, compare numerically
        if (*name_a >= '0' && *name_a <= '9' && *name_b >= '0' && *name_b <= '9') {
            long num_a = 0, num_b = 0;

            // Parse number from string a
            while (*name_a >= '0' && *name_a <= '9') {
                num_a = num_a * 10 + (*name_a - '0');
                name_a++;
            }

            // Parse number from string b
            while (*name_b >= '0' && *name_b <= '9') {
                num_b = num_b * 10 + (*name_b - '0');
                name_b++;
            }

            // Compare numbers
            if (num_a != num_b) {
                return (num_a < num_b) ? -1 : 1;
            }
        }
        // Otherwise compare characters
        else {
            if (*name_a != *name_b) {
                return (*name_a < *name_b) ? -1 : 1;
            }
            name_a++;
            name_b++;
        }
    }

    // Handle end of string
    if (*name_a == '\0' && *name_b == '\0') return 0;
    return (*name_a == '\0') ? -1 : 1;
}

void ConvertLevToBip_callback(const char* path, void* userdata)
{
    // Collect all .lev files
    if (strstr(path, ".lev") != NULL) {
        if (_file_count >= _capacity) {
            _capacity *= 2;
            _filenames = pd->system->realloc(_filenames, _capacity * sizeof(char*));
        }
        pd->system->formatString(&_filenames[_file_count], "%s/%s", (char*)userdata, path);
        _file_count++;
    }
}

void ConvertLevToBip(char* dir_path, char* output_file, char* collectionName, char* author) {
    _capacity = 100;
    _filenames = pd->system->realloc(NULL, _capacity * sizeof(char*));
    _file_count = 0;
    _level_num = 0;
    pd->file->listfiles(dir_path, ConvertLevToBip_callback, dir_path, false);
    if (_file_count == 0)
    {
        pd->system->realloc(_filenames, 0);
        return;
    }
    
    // Sort filenames
    qsort(_filenames, _file_count, sizeof(char*), compare_filenames);

    // Open output file
    _outputfile = pd->file->open(output_file, kFileWrite);
    if (!_outputfile) {
        for (int i = 0; i < _file_count; i++)
            pd->system->realloc(_filenames[i], 0);
        pd->system->realloc(_filenames, 0);
        _file_count = 0;
        return;
    }

    pd->system->formatString(&_author, "%s", author);
    
    unsigned int utc_epoch = pd->system->getSecondsSinceEpoch(NULL);
    int32_t offset = pd->system->getTimezoneOffset();
    unsigned int local_epoch = utc_epoch + offset;

    struct PDDateTime time_info;
    pd->system->convertEpochToDateTime(local_epoch, &time_info);

    // Write header
    pdfprintf(_outputfile, "Date of Last Change: %d.%02d.%02d - %02d:%02d:%02d\n", time_info.year, time_info.month, time_info.day,
        time_info.hour,  time_info.minute, time_info.second);
    pdfprintf(_outputfile, "\n");
    pdfprintf(_outputfile, "Set:       %s\n", collectionName);
    pdfprintf(_outputfile, "Author:    %s\n", author);
    pdfprintf(_outputfile, "Email:     \n");
    pdfprintf(_outputfile, "Homepage:  \n");
    pdfprintf(_outputfile, "\n");
    pdfprintf(_outputfile, ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    pdfprintf(_outputfile, "\n");
    pdfprintf(_outputfile, "Generated from multiple *.lev format to .bip format for description see blips readme.md\n");
    pdfprintf(_outputfile, "\n");
    pdfprintf(_outputfile, ":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    pdfprintf(_outputfile, "\n");
}

int ConvertLevToBipContinue()
{
    if (_file_count == 0)
        return -1;

    // Convert each level
         
    if (load_lev_file(_filenames[_level_num], &level)) {        
        write_level_ascii(_outputfile, &level, _level_num+1, _author, _filenames[_level_num]);
    }
    pd->system->realloc(_filenames[_level_num], 0);
    _level_num++;
    
    
    if (_level_num >= _file_count)
    {
        pd->file->close(_outputfile);
        pd->system->realloc(_filenames, 0);
        pd->system->realloc(_author, 0);
        _file_count = 0;
        return 0;
    }
    return _file_count - _level_num;
}