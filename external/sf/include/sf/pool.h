#ifndef SF_POOL_H
#define SF_POOL_H


#include <stddef.h>
#include <stdint.h>

#include "defs.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct sf_pool_large sf_pool_large_t;
struct sf_pool_large {
    sf_pool_large_t *next;
    void            *alloc;
};

typedef struct sf_pool_node sf_pool_node_t;
struct sf_pool_node {
    sf_pool_node_t *next;
    uint8_t        *last;
    uint8_t        *end;

    /*
     * flexible array for allocated memory
     */
};

typedef struct sf_pool sf_pool_t;
struct sf_pool {
    size_t              max;
    sf_pool_node_t     *first;
    sf_pool_large_t    *large;
};


sf_result_t sf_pool_init(sf_pool_t *pool, size_t max);

void sf_pool_destroy(sf_pool_t *pool);

void *sf_pool_alloc(sf_pool_t *pool, size_t size);

void *sf_pool_calloc(sf_pool_t *pool, size_t size);

void sf_pool_free(sf_pool_t *pool, void *p);

void sf_pool_clear(sf_pool_t *pool);


#ifdef __cplusplus
}
#endif

#endif /* SF_POOL_H */
