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
double haan_window(int total, int i)
{
    return 0.5 * (1 - cos(2 * 3.1415 * i / (total - 1)));
};
void setup_audio_file(Handle* handle, char *path)
{
    handle->audio_file = sf_open(path, SFM_READ, &handle->file_info);
}

double square_sum(double *arr, int from, int to)
{
    double sum = 0;
    for(int i = from; i < to; i ++)
        sum += arr[i] * arr[i];
    return sum;
}
int double_comp(double* x, double* y)
{
    if(*x > *y) return 1;
    else if(*x == *y) return 0;
    return -1;
}
int find_biggest(double* arr, int size)
{
    double big = arr[0];
    int big_num = 0;
    for(int i = 1; i < size; i ++) {
        if(big < arr[i]) {
            big = arr[i];
            big_num = i;
        }
    }
    return big_num;
}
void run_fft(Handle* handle, VoiceDetectVector* vector) //, double * voice_starting_points)
{
    if(handle->audio_file == NULL) return ;
    double sample[SAMPLE_SIZE * handle->file_info.channels];
    double output[SAMPLE_SIZE];

    double result[SAMPLE_SIZE];

    float buffer[SAMPLE_SIZE];

    int read_count = 0;
    int read_num;

    fftw_plan plan;
    plan = fftw_plan_r2r_1d(SAMPLE_SIZE, sample, output, 0, FFTW_ESTIMATE);

    FILE* fp = fopen("check_result", "w");

    while ((read_num = sf_readf_float(handle->audio_file, buffer, SAMPLE_SIZE)) > 0) {
        for(int i = 0; i < read_num; i ++) {
            sample[i] = 0;
            for(int j = 0; j < handle->file_info.channels; j ++) {
                sample[i] += buffer[handle->file_info.channels * i + j];
            }
            sample[i] *= haan_window(SAMPLE_SIZE, i);
        }
        fftw_execute(plan);

        read_count += read_num;

        for(int i = 0; i < SAMPLE_SIZE / 2; i ++) {
            result[i] = sqrt(output[i] * output[i] + output[SAMPLE_SIZE - i - 1]*output[SAMPLE_SIZE - i - 1]); 
        }
        double total_energy = 0;
        for(int i = 0; i < SAMPLE_SIZE / 2; i ++) {
            total_energy += result[i] * result[i];
        }

        if(square_sum(result, DIVIDE * 2, DIVIDE * 4) / total_energy > 0.30 && total_energy > 200000) {
            fprintf(fp, "Time : %d m %ds " , read_count / 48000 / 60, (read_count) / 48000 - (read_count / 48000 / 60) * 60);
            fprintf(fp, ": %d ~ %d Hz :  %f", 48000 / SAMPLE_SIZE * DIVIDE * 2, 48000 / SAMPLE_SIZE * 
                    4 * DIVIDE, square_sum(result, DIVIDE * 2, 4*DIVIDE) / total_energy);
            fprintf(fp, " : %f \n", total_energy);

            double result_check[12];
            for(int i = 0; i < 12; i ++) {
                // frequency : 50 * i + 300
                result_check[i] = square_sum(result, (50 * i + 300) / (48000 / SAMPLE_SIZE), (50 * i + 350) / (48000 / SAMPLE_SIZE));
                //    fprintf(fp, "%d ~ %d Hz, %f \n", 50 * i + 300, 50 * i + 350, result_check[i]);
            }
            // Find biggest
            int biggest = find_biggest(result_check, 12);
            result_check[biggest] = 0;

            // Second biggest
            int second = find_biggest(result_check, 12);
            double second_big = result_check[second];
            result_check[second] = 0;

            // thrid biggest
            int third = find_biggest(result_check, 12);
            double third_big = result_check[third];
            result_check[third] = 0; 

            // If the average of remining 9 is larger than
            // half of the second and thrid biggest ones, than
            // it is not likely a human voice
            double sum = 0;
            for(int i = 0; i < 12; i ++) {
                sum += result_check[i];
            }

            sum /= 9;
            fprintf(fp, "AVG : %f , TOP 2,3 AVG : %f \n", sum, (second_big + third_big) / 2 );
            if(sum > (second_big + third_big) / 4) {
                fprintf(fp, ">>>>>>>>>>>>>>>>>>>> NOT HUMAN VOICE <<<<<<<<<<<<<<<<<<\n");

            } else {
                VoiceDetect v = {(double)read_count/48000, total_energy};
                add_VoiceDetect(vector, &v);
            }
        }
    }
    fftw_destroy_plan(plan);
}
int main()
{
    struct Handle handle;
    VoiceDetectVector* vector = create_vector(); 
    setup_audio_file(&handle, "/home/probablee/cpp/caption-sync/bbad.wav");
    run_fft(&handle, vector);

    for(int i = 0; i < vector->size; i ++) {
        printf("At %f : \n", vector->data[i].start_time);
    }
    free_vector(vector);

    return 0;
    
}
