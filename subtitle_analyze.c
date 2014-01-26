Using username "hjkwon93".
Using keyboard-interactive authentication.
Password:
corn07.stanford.edu - Ubuntu 13.04, amd64
8-core Opteron 2384 (SB X6240, 0916FM400J); 31.42GB RAM, 10GB swap
Puppet environment: rec_master; kernel 3.8.0-31-generic (x86_64)
 --*-*- Stanford University Research Computing -*-*--

  _____                    ____  _
 |  ___|_ _ _ __ _ __ ___ / ___|| |__   __ _ _ __ ___
 | |_ / _` | '__| '_ ` _ \\___ \| '_ \ / _` | '__/ _ \
 |  _| (_| | |  | | | | | |___) | | | | (_| | | |  __/
 |_|  \__,_|_|  |_| |_| |_|____/|_| |_|\__,_|_|  \___|


    http://farmshare.stanford.edu

###
##
# new to Ubuntu 13.04 Farmshare?
# follow this link to get started:
# https://www.stanford.edu/group/farmshare/cgi-bin/wiki/index.php/Ubuntu13Transi                                                                                                                     tionGuide
##
###
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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