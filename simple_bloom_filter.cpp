#include "simple_bloom.h"
#include <stdio.h>

unsigned int djb2(const void *_str) {
    const char *str = (const char *)_str;
    unsigned int hash = 5381;
    char c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

unsigned int jenkins(const void *_str) {
    const char *key = (const char *)_str;
    unsigned int hash, i;
    while (*key) {
        hash += *key;
        hash += (hash << 10);
        hash ^= (hash >> 6);
        key++;
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

int main() {
    bloom_t bloom = bloom_create(8);
    bloom_add_hash(bloom, djb2);
    bloom_add_hash(bloom, jenkins);
    printf("Should be 0: %d\n", bloom_test(bloom, "hello world"));
    bloom_add(bloom, "hello world");
    printf("Should be 1: %d\n", bloom_test(bloom, "hello world"));
    printf("Should (probably) be 0: %d\n", bloom_test(bloom, "world hello"));
    return 0;
}


