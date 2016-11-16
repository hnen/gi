
#define MAX_RUNTIME_SAMPLES 100000000
#define MAX_SAMPLERS 1024
#define MAX_DEPTH 1024

#include <time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "common.h"
#include "timer.h"

typedef struct {
    char * desc;
    int parent;
    unsigned long hash;
    double avg_time;
    double min_time;
    double max_time;
    double total_time;
    int time_samples;
    int samples;
} timersampler;

typedef struct {
    char * desc;
    long tv_msec;
} runtimesample;

int runtimesample_p = 0;
runtimesample samples[MAX_RUNTIME_SAMPLES];

int samplers_count;
timersampler samplers[MAX_SAMPLERS];

double reset_time;
int frames_count=0;

static unsigned long hash(char *str);
static int find_sampler(char * desc, int parent);
static int create_sampler(char * desc, int parent);

struct timespec tp;
int times_sampled_hashmap[65536]; 

int sampled_stack_top = -1;
char sampled_stack[1024];

#define HASHMAP_LOC(hm, val) (((unsigned int)(val)) & 0xffff)

void timer_push(char * desc) {
    int * times_sampled = &times_sampled_hashmap[HASHMAP_LOC(times_sampled_hashmap, desc)];
    (*times_sampled)++;
    if (rand() <= RAND_MAX / (1 + ((*times_sampled) >> 4))) {
        runtimesample * s = &samples[runtimesample_p++];
        s->desc = desc;
        //clock_gettime(CLOCK_REALTIME, &tp);
        s->tv_msec = tp.tv_nsec / 1000 + tp.tv_sec * 1000000;
        sampled_stack[++sampled_stack_top] = 1;
    } else {
        sampled_stack[++sampled_stack_top] = 0;
    }
}

void timer_pop() {
    if (sampled_stack[sampled_stack_top--]) {
        runtimesample * s = &samples[runtimesample_p++];
        s->desc = 0;
        //clock_gettime(CLOCK_REALTIME, &tp);
        s->tv_msec = tp.tv_nsec / 1000 + tp.tv_sec * 1000000;
    } 
}


void timer_frame() {
    frames_count++;
}

void timer_purge() {
    int stack_top = -1;
    int stack_sampler[1024];
    long stack_time[1024];

    for (int i = 0; i < runtimesample_p; i++) {
        runtimesample * s = &samples[i];
        if (s->desc) {
            int curr = -1;
            if (stack_top >= 0) {
                curr = stack_sampler[stack_top];
            }
            int i = find_sampler(s->desc, curr);
            if (i<0) {
                i=create_sampler(s->desc, curr);
            }
            stack_top++;
            stack_sampler[stack_top] = i;
            stack_time[stack_top] = s->tv_msec;
        } else {
            int i = stack_sampler[stack_top];
            timersampler * slr = &samplers[i];
            if (s->tv_msec >= 0) {
                double dur = (double)(s->tv_msec - stack_time[stack_top]) / 1000.0;
                slr->avg_time = (slr->avg_time * slr->samples + dur) / (slr->samples + 1);
                slr->total_time += dur;
                slr->time_samples++;
            }
            stack_top--;
        }
    }

    for(int i = 0; i < samplers_count; i++) {
        samplers[i].samples += times_sampled_hashmap[HASHMAP_LOC(times_sampled_hashmap, samplers[i].desc)];
    }

    runtimesample_p = 0;
    memset(times_sampled_hashmap, 0, sizeof(times_sampled_hashmap));
}

static void timer_dump_0(int parent, int depth);

void timer_dump() {
    timer_purge();
    printf("%-25s%-10s%-10s%-10s%-10s\n", "desc", "avg", "total", "calls", "timesamples");
    timer_dump_0(-1, 0);
    //printf("%d frames in %.2fms\n", frames_count, (glfwGetTime() - reset_time) * 1000.0f);
    printf("--\n");
}

void timer_dump_0(int parent, int depth) {
    char desc[1024];
    for(int i = 0; i < samplers_count; i++) {
        timersampler * sampler = &samplers[i];
        if (sampler->parent == parent) {
            for (int j = 0; j < depth; j++) {
                desc[j] = '>';
            }
            strcpy(&desc[depth], sampler->desc);
            printf("%-25s%-10.5f%-10.2f%-10.2f%10.2f\n", desc, sampler->avg_time, (sampler->samples * sampler->total_time / sampler->time_samples)/frames_count, (float)sampler->samples/frames_count, (float)sampler->time_samples/frames_count);
            timer_dump_0(i, depth+1);
        }
    }
}

void timer_reset() {
    //reset_time = glfwGetTime();
    frames_count = 0;
    samplers_count = 0;
}

static unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ( (c = *str++) )
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

static int find_sampler(char * desc, int parent) {
    unsigned long hash_desc = hash(desc);
    for(int i = 0; i < samplers_count; i++) {
        if (samplers[i].hash == hash_desc) {
            return i;
        }
    }
    return -1;
}

static int create_sampler(char * desc, int parent) {
    if (samplers_count >= MAX_SAMPLERS) {
        XX_TODO("hit sampler limit %d", MAX_SAMPLERS);
        return -1;
    }
    timersampler nsampler = {
        .desc = desc,
        .parent = -1,
        .hash = hash(desc),
        .avg_time = 0, .min_time = INFINITY, .max_time = -INFINITY,
        .samples = 0
    };
    samplers[samplers_count] = nsampler;
    return samplers_count++;
}
