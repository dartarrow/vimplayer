#ifndef FFT_H
#define FFT_H
#include <stdio.h>
#include <fftw3.h>
#include <sndfile.h>
#include <math.h>
#include <complex.h>
#include "voice_vector.h"

#define SAMPLE_SIZE 24000
#define DIVIDE 100

typedef struct Handle
{
    SNDFILE *audio_file;
    SF_INFO file_info;

} Handle;

void run_fft(Handle* handle, VoiceDetectVector* vector);
void setup_audio_file(Handle* handle, char *path);

#endif
