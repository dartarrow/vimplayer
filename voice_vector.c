#include "voice_vector.h"

VoiceDetectVector* create_vector()
{
    VoiceDetectVector *v = (VoiceDetectVector *)malloc(sizeof(VoiceDetectVector));
    v->allocated = 50;
    v->size = 0;
    v->data = (VoiceDetect *) malloc(sizeof(VoiceDetect) * 50);

    return v;
}
void add_VoiceDetect(VoiceDetectVector* vector, VoiceDetect* v)
{
    if(vector->allocated > vector->size) {
        vector->data[vector->size].power = v->power;
        vector->data[vector->size].start_time = v->start_time; 
    }
    else {
        VoiceDetect* new_data = (VoiceDetect *)malloc(sizeof(VoiceDetect) * 2 * vector->allocated);
        for(int i = 0; i < vector->size; i ++) {
            new_data[i].power = vector->data[i].power;
            new_data[i].start_time = vector->data[i].start_time;
        }
        new_data[vector->size].power = v->power;
        new_data[vector->size].start_time = v->start_time;

        free(vector->data);
        vector->data = new_data;
        vector->allocated *= 2;
    }
    vector->size ++;
}
void free_vector(VoiceDetectVector *vector) 
{
    free(vector->data);
    free(vector);
}

