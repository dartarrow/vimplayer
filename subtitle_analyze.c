
#include "subtitle_analyze.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
void read_srt(char** subtitles[], int*start_times[], int* num, char* filename){   
    char *line = malloc(1000+1);
    FILE *fp = fopen(filename, "r");
    int count = 0;
    while(fgets(line,1000,fp) != NULL){
        if(strcmp(line,"\n")==0){
            continue;
        }
        if(atoi(line) == count+1){
            count++;
        }else{
            continue;
        }
    }

    *subtitles = malloc(count*sizeof(char*));
    *start_times = malloc(count*sizeof(int));
    rewind(fp);
    while(fgets(line,1000,fp) != NULL){
        fgets(line,1000,fp);
        int start_time = 0;
        int end_time = 0;
        for(int i=0;i<12;i++){
            if(i==2 || i==5|| i==8)continue;
            start_time = 10*start_time + line[i] - '0';
            end_time = 10*end_time + line[i+18] - '0';
        }
        start_time = start_time/10000000*3600 + start_time%10000000*100000*60 + start_time%100000*1000;
        end_time = end_time/10000000*3600 + end_time%10000000*100000*60 + end_time%100000*1000;
        fgets(line,1000,fp);
        while(strcmp(line,"\n")!=0){
            char addon[1000];
            printf("%s/n", line);
            fgets(addon,1000,fp);
            if(strcmp(addon,"\n")==0)break;
            strcat(line,addon);
        }
        (*start_times)[*num] = start_time;
        (*subtitles)[*num] = malloc(sizeof(char)*(strlen(line)+1));

        strcpy((*subtitles)[*num],line);
        (*num)++;
    }
}
void read_smi(char** subtitles[], int* start_times[], int* num, char* filename)
{
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) return;

    char buffer[1000];
    *num = 0;

    //Count the number of subtitles
    while (fgets(buffer, 1000, fp) != NULL) {
        if(strstr(buffer, "<SYNC Start") != NULL) (*num) ++;
    }

    (*start_times) = malloc(sizeof(int) * (*num));

    rewind(fp);

    int count = 0;
    while (fgets(buffer, 1000, fp) != NULL) {
        char *ptr = NULL;
        if((ptr = strstr(buffer, "<SYNC Start=")) != NULL) {
            ptr = ptr + strlen("<SYNC Start=");

            int num = 0;
            while(isdigit(*ptr)) {
                num *= 10;
                num += (*ptr - '0');
                ptr ++;
            }
            (*start_times)[count] = num;
            (*start_times)[count] /= 1000;
            count ++;
        }
    }
}

void analyze(char* filename, char **subtitles [], int *times [], int *num){
    if(!strcmp(filename+strlen(filename)-3,"smi")){
        read_smi(subtitles,times,num,filename);

    }
    else if(!strcmp(filename+strlen(filename)-3, "srt")){
        read_srt(subtitles, times,num, filename);

    }
    else{
        printf("Wrong file format/n");
    }
}
