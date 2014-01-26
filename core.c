#include <stdio.h>
#include "voice_vector.h"
#include "subtitle_analyze.h"
#include "fft.h"

#define INF 1000000000
typedef struct Voice_Chunks
{
    double start_time;
    double end_time;
} Voice_Chunks;

typedef struct Subtitle
{
    int sub_start_time;
    char* subtitle;
    int subtitle_count;

} Subtitle;

void find_match(Voice_Chunks* subt, Voice_Chunks* voice, int sub_size, int voc_size);
double calculate_score(Voice_Chunks * subtitle, Voice_Chunks* chunks, 
        double offset, double extend,
        int subt_chunk_size, int voic_chunk_size);

void fit_subtitle(Subtitle* subtitle, VoiceDetectVector* vector)
{
    int voice_num = vector->size; 
    Voice_Chunks* voice_list = malloc(sizeof(Voice_Chunks) * voice_num);
    Voice_Chunks* subt_list = malloc(sizeof(Voice_Chunks) * subtitle->subtitle_count);

    // The discrete voices within 2 second difference are considered as a
    // consecutive conversation. 

    voice_list[0].start_time = vector->data[0].start_time;
    voice_list[0].end_time = vector->data[0].start_time + 1;

    int chunk_size = 1;

    for(int i = 1; i < vector->size; i ++) {
        if(vector->data[i].start_time - vector->data[i - 1].start_time <= 2) {
            // Then considered as a consecutive conversation
            voice_list[chunk_size - 1].end_time = vector->data[i].start_time + 1;        
        }
        else {
            voice_list[chunk_size].start_time = vector->data[i].start_time;
            voice_list[chunk_size].end_time = vector->data[i].start_time + 1;
            chunk_size ++;
        }
    }

    // Do the same thing to the subtitle

    subt_list[0].start_time = subtitle[0].sub_start_time;
    subt_list[0].end_time = subtitle[0].sub_start_time + 1;

    int subt_chunk_size = 1;
    for(int i = 1; i < subtitle->subtitle_count; i ++) {
        if(subtitle[i].sub_start_time - subtitle[i - 1].sub_start_time <= 2) {
            subt_list[subt_chunk_size - 1].end_time = subtitle[i].sub_start_time + 1;
        } else {
            subt_list[subt_chunk_size].start_time = subtitle[i].sub_start_time;
            subt_list[subt_chunk_size].end_time = subtitle[i].sub_start_time + 1;
            subt_chunk_size ++;
        }
    }

    find_match(subt_list, voice_list, subt_chunk_size, chunk_size);
}
void find_match(Voice_Chunks* subt, Voice_Chunks* voice, int sub_size, int voc_size)
{
    // Find the closest and similar chunks
    double *offset = (double *) malloc(sizeof(double) * sub_size);
    double avg_offset = 0;
    for(int i = 0; i < sub_size; i ++) {
        double sub_chunk_size = subt[i].end_time - subt[i].start_time;
        int best_match;
        double smallest = INF;
        for(int j = 0; j < voc_size; j ++) {
            double match = abs(voice[j].start_time - subt[i].start_time) * 
                abs(voice[j].end_time - subt[i].end_time) *
                abs(voice[j].end_time - voice[j].start_time - sub_chunk_size + 10);
            if(match < smallest) {
                smallest = match;
                best_match = j;
            }
        }
        offset[i] = voice[best_match].start_time - subt[i].start_time;        
    } 

    for(int i = 0; i < sub_size; i ++) { 
        avg_offset += offset[i];
        printf("off : %f \n", offset[i]);
    }
    avg_offset /= sub_size;

    // Check the minimum nearby the solution
    double min = INF;
    int best_i, best_j;
    for(int i = -2; i <= 2; i ++) {
        for(int j = -2; j <= 2; j ++) {
            double temp;
            if((temp = calculate_score(subt, voice, avg_offset + i, 1 + 0.1 * j, sub_size, voc_size)) < min) {
                min = temp;
                best_i = i; best_j = j;
            }
        }
    }

    printf("OFFSET : %f | EXTEND RATE : %f \n", best_i + avg_offset, 1 + 0.1 * best_j);
}
double changed_loc(double loc, double offset, double extend)
{
    return (loc + offset) * extend;
}
double calculate_score(Voice_Chunks * subtitle, Voice_Chunks* chunks, 
        double offset, double extend,
        int subt_chunk_size, int voic_chunk_size)
{
    // Fix the position of the voice chunks, move the subtitles
    int matched_voice_chunk = 0;
    int checked_sub_chunk = 0;

    double total_point = 0;

    while (checked_sub_chunk < subt_chunk_size) {

        double min_dist = INF;
        int min_chunk = 0;

        double changed_subt_start = changed_loc(subtitle[checked_sub_chunk].start_time, offset, extend);
        double changed_subt_end = changed_loc(subtitle[checked_sub_chunk].end_time, offset, extend);

        for(int i = matched_voice_chunk; i < voic_chunk_size; i ++) {
            if(abs(chunks[i].start_time - changed_subt_start ) < min_dist) {
                min_chunk = i;
                min_dist = (abs(chunks[i].start_time - changed_subt_start));
            }
        }

        if(min_chunk != matched_voice_chunk) {
            // Penalty
            for(int i = matched_voice_chunk; i < min_chunk; i ++) {
                total_point += (subtitle[i].end_time - subtitle[i].start_time);
            }
        }
        matched_voice_chunk = min_chunk;

        // There is a lot of penalty for missed subtitle for a voice
        // But not much penalty for missed voice for a subtitle

        total_point += 2 * (min_dist + abs(chunks[min_chunk].end_time - changed_subt_end));

        checked_sub_chunk ++;
    }

    return total_point;
}

int main()
{
    struct Handle handle;
    VoiceDetectVector* vector = create_vector(); 
    setup_audio_file(&handle, "/home/probablee/cpp/caption-sync/b.wav");
    run_fft(&handle, vector);

    for(int i = 0; i < vector->size; i ++) {
        printf("At %f : \n", vector->data[i].start_time);
    }

    char** subtitles;
    int* start_times;
    int total_subtitle = 0;
    analyze("a.smi", &subtitles, &start_times, &total_subtitle);
    printf("total sub : %d \n", total_subtitle);
    Subtitle* subtitle_list = (Subtitle *)malloc(sizeof(Subtitle) * total_subtitle);
    for(int i = 0; i < total_subtitle; i ++) {
        subtitle_list[i].subtitle = subtitles[i];
        subtitle_list[i].sub_start_time = start_times[i];
        printf("start at : %d \n", start_times[i]);
        subtitle_list[i].subtitle_count = total_subtitle; 
    }

    fit_subtitle(subtitle_list, vector);
    free(subtitle_list);
    free_vector(vector);

    return 0;

}
