#include "preprocess.h"
#include <stdio.h>

int process_user_input(int argc, char *argv[])
{
    if(argc != 3) {
        printf("INVALID COMMAND INPUT");
        return -1;
    }
    
    char buffer[1000];
    sprintf(buffer, "avconv -i %s", argv[1]);

    char avconv_output[1000];
    FILE* fp = popen(buffer, avconv_output);

    printf("%s", avconv_output);

    sprintf(buffer, "avconv -i %s %s.wav", argv[1], argv[1]);
    
    // Convert any media file to .wav file format
    system(buffer);

    sprintf(buffer, "normalize-audio -b %s.wav", argv[1]);

    // Normalize an audio into a standard volume so that
    // we can easily screen the human voices
    system(buffer);

    return 0;
}

