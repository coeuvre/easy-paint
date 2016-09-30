#include <stdlib.h>
#include <string.h>

#include "sf/log.h"
#include "sf/utils.h"


uint32_t sf_power_2(uint32_t n) {
    --n;
    n |= (n >> 16);
    n |= (n >> 8);
    n |= (n >> 4);
    n |= (n >> 2);
    n |= (n >> 1);
    return n + 1;
}

static size_t nalloc = 0;
static size_t nfree  = 0;
static size_t nbytes = 0;

void *sf_alloc(size_t size) {
    void *p;

    p = malloc(sizeof(size_t) + size);
    if (p == NULL) {
        sf_log(SF_LOG_PANIC, "sf_alloc: failed to allocate %zu bytes.", size);
        abort();
    }

    ++nalloc;
    nbytes += size;
    *(size_t *) p = size;

    sf_log(SF_LOG_INFO, "sf_alloc: allocate %zu bytes.", size);

    return (size_t *) p + 1;
}

void *sf_calloc(size_t size) {
    void *p;

    p = sf_alloc(size);
    sf_memzero(p, size);

    return p;
}

void *sf_realloc(void *p, size_t size) {
    size_t *real = (size_t *) p - 1;
    void   *newp;

    nbytes -= *real;
    newp = realloc(real, size);
    if (newp == NULL) {
        sf_log(SF_LOG_PANIC, "sf_realloc: failed to allocate %zu bytes.", size);
        abort();
    }

    nbytes += size;
    *(size_t *) newp = size;

    sf_log(SF_LOG_INFO, "sf_realloc: allocate %zu bytes.", size);

    return (size_t *) newp + 1;
}

void sf_free(void *p) {
    size_t *real = (size_t *) p - 1;

    sf_log(SF_LOG_INFO, "sf_free: free %zu bytes.", *real);

    ++nfree;
    nbytes -= *real;

    free(real);
}

sf_bool_t sf_memcheck(void) {
    if (nbytes > 0) {
        sf_log(SF_LOG_WARN, "sf_memcheck: %zu bytes in use."
               " (%zu allocs and %zu frees)", nbytes, nalloc, nfree);
        return SF_FALSE;
    }

    return SF_TRUE;
}
