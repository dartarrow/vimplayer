#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void read_srt(char** subtitles[], int*start_times[], int* num){
    char *filename = "subtitle.srt";
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
            fgets(addon,1000,fp);
            line = realloc(line,sizeof(line)+1000);
            strcat(line,addon);
        }

        (*start_times)[*num] = start_time;
        (*subtitles)[*num] = malloc(sizeof(char)*(strlen(line)+1));
        strcpy((*subtitles)[*num],line);
        (*num)++;
    }
}

void read_smi(char** subtitles[], int* start_times[], int* num){
    char *filename = "subtitles.smi";
    char *line = malloc(1000+1);
    FILE *fp = fopen(filename, "r");
    int count=0;
    while(fgets(line,1000,fp) != NULL){
        if(strstr(line, "<SYNC Start=")==line){
            count++;
        }
    }
    *subtitles = malloc(count*sizeof(char*));
    *start_times = malloc(count*sizeof(int));
    rewind(fp);
    while(fgets(line,1000,fp) != NULL){
        if(strstr(line,"<SYNC Start=")!=line){
            continue;
        }
        int time=0;
        for(int i=strlen("<SYNC Start=");i<strlen(line);i++){
            if(line[i] == '>'){
                break;
            }
            time = 10*time + line[i] - '0';
        }
        time = time/10000 * 60 * 60 + (time%10000)/100 * 60 + time %100;
        if(strcmp(line+strlen(line)-7,"&nbsp;\n")==0){
            char *empty = "";
            (*start_times)[*num] = time;
            (*subtitles)[*num] = malloc(sizeof(char)*(strlen(empty)+1));
            strcpy((*subtitles)[*num],empty);
            (*num)++;
        }else{
            fgets(line,1000,fp);
            while(strlen(line)<5 || strcmp(line+strlen(line)-5,"<br>\n")==0){
                char addon[1000];
                fgets(addon,1000,fp);
                line = realloc(line,sizeof(line)+1000);
                strcat(line,addon);
            }
            (*subtitles)[*num] = malloc(sizeof(char)*(strlen(line)+1));
            strcpy((*subtitles)[*num],line);
            (*start_times)[*num] = time;
            (*num)++;
        }
    }
    fclose(fp);
    free(line);
}
