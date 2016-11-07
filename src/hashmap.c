#include <string.h>

#include "common.h"
#include "hashmap.h"

#define BUCKET_SIZE 4


typedef struct {
    unsigned long hash;
    void * value;
} bckt_el;

typedef struct {
    int count;
    bckt_el els[BUCKET_SIZE];
} bckt;

typedef struct {
    int size;
} dict_header;

XXdict dict_alloc(int size) {
    dict_header * dict = (dict_header*)malloc(sizeof(dict_header) + sizeof(bckt) * size);
    dict->size = size;
    memset(dict + sizeof(dict_header), 0, sizeof(bckt) * size);
    return dict;
}

void dict_free(XXdict dict) {
    free(dict);
}

bckt * getbucket(XXdict dict, unsigned long hash) {
    dict_header * d = (dict_header*)dict;
    int p = hash%d->size;
    bckt * buckets = dict + sizeof(dict_header);
    return &buckets[p];
}

void add_to_bucket(bckt * bucket, unsigned long valuehash, void* value) {
    bckt_el el = {
        .hash = valuehash, .value = value
    };
    if (bucket->count >= BUCKET_SIZE) {
        XX_TODO("inflate hashmap...");
    }
    bucket->els[bucket->count++] = el;
}

int bucket_value_i(bckt * bucket, unsigned long hash) {
    for(int i = 0; i < bucket->count; i++) {
        if (bucket->els[i].hash == hash) {
            return i;
        }
    }
    return -1;
}

void dict_set(XXdict dict, unsigned long hash, void * value) {
    bckt * bucket = getbucket(dict, hash);
    int i = bucket_value_i(bucket, hash);
    if (i >= 0) {
        bckt_el el = {
            .hash = hash, .value = value
        };
        bucket->els[i] = el;
    } else {
        add_to_bucket(bucket, hash, value);
    }
}

void * dict_get(XXdict dict, unsigned long hash) {
    bckt * bucket = getbucket(dict, hash);
    int i = bucket_value_i(bucket, hash);
    if (i >= 0) {
        return bucket->els[i].value;
    } else {
        // no value
        __builtin_trap();
    }
}


