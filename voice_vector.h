#include <stdlib.h>
typedef struct VoiceDetect
{
    double start_time;
    double power;

} VoiceDetect;

typedef struct VoiceDetectVector
{
    int allocated;
    int size;
    VoiceDetect* data;
} VoiceDetectVector;

VoiceDetectVector* create_vector();
void add_VoiceDetect(VoiceDetectVector* vector, VoiceDetect* v);
void free_vector(VoiceDetectVector *vector);


