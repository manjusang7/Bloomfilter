#ifndef _BLOOM_H
#define _BLOOM_H
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>     /* calloc, exit, free */


typedef unsigned int (*hash_function)(const void *data);
typedef struct bloom_filter * bloom_t;

struct bloom_hash {
    hash_function func;
    struct bloom_hash *next;
};
struct bloom_filter {
    struct bloom_hash *func;
    void *bits;
    size_t size;
};


/* Creates a new bloom filter with no hash functions and size * 8 bits. */
bloom_t bloom_create(size_t size) {
    bloom_t res = (bloom_t)calloc(1, sizeof(struct bloom_filter));
    res->size = size;
    res->bits = malloc(size);
    return res;
}

/* Frees a bloom filter. */
void bloom_free(bloom_t filter) {
    if (filter) {
        while (filter->func) {
            struct bloom_hash *h;
            filter->func = h->next;
            free(h);
        }
        free(filter->bits);
        free(filter);
    }
}

/* Adds a hashing function to the bloom filter. You should add all of the
* functions you intend to use before you add any items. */
void bloom_add_hash(bloom_t filter, hash_function func) {
    struct bloom_hash *h = (struct bloom_hash *)calloc(1, sizeof(struct bloom_hash));
    h->func = func;
    struct bloom_hash *last = filter->func;
    while (last && last->next) {
        last = last->next;
    }
    if (last) {
        last->next = h;
    } else {
        filter->func = h;
    }
}

/* Adds an item to the bloom filter. */
void bloom_add(bloom_t filter, const void *item) {
    struct bloom_hash *h = filter->func;
    uint8_t *bits = (uint8_t *)filter->bits;
    while (h) {
        unsigned int hash = h->func(item);
        hash %= filter->size * 8;
        bits[hash / 8] |= 1 << hash % 8;
        h = h->next;
    }
}


/* Tests if an item is in the bloom filter.
*
* Returns false if the item has definitely not been added before. Returns true
* if the item was probably added before. */
bool bloom_test(bloom_t filter, const void *item) {
    struct bloom_hash *h = filter->func;
    uint8_t *bits = (uint8_t *)filter->bits;
    while (h) {
        unsigned int hash = h->func(item);
        hash %= filter->size * 8;
        if (!(bits[hash / 8] & 1 << hash % 8)) {
            return false;
        }
        h = h->next;
    }
    return true;
}

#endif
