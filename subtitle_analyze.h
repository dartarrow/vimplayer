#ifndef SUBTITLE_ANALYZE
#define SUBTITLE_ANALYZE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void read_smi(char** subtitles[], int* start_times[], int* num, char* filename);
void read_srt(char** subtitles[], int*start_times[], int* num, char* filename);
void analyze(char* filename, char **subtitles[], int *times [], int *num);
#endif
