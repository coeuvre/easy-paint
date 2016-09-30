#ifndef SF_ARRAY_H
#define SF_ARRAY_H


#include "defs.h"


#ifdef __cplusplus
extern "C" {
#endif


#define SF_ARRAY_DEF_NALLOC(a) (SF_PAGESIZ / a->def.size)

typedef struct sf_array_def sf_array_def_t;
struct sf_array_def {
    /*
     * Hint for the initial element count to allocated,
     *
     * If zero, `SF_ARRAY_DEF_NALLOC` is used.
     */
    uint32_t    nalloc;

    /* size of an element in byte */
    size_t      size;

    /*
     * Function to compare to elements.
     *
     * a and b are the address the each element.
     *
     * If NULL, memcmp is used.
     */
    int (*cmp)(const void *a, const void *b);

    /*
     * Function to copy an element.
     *
     * This is used to populate the element.
     *
     * If NULL, memcpy is used.
     */
    void (*cpy)(void *dst, const void *src);

    /*
     * Function to free an element.
     *
     * Called when deleting an element.
     *
     * If NULL, nothing happened.
     */
    void (*free)(void *elt);
};


typedef struct sf_array sf_array_t;
struct sf_array {
    void           *elts;
    uint32_t        nelts;
    sf_array_def_t  def;
};

sf_result_t sf_array_init(sf_array_t *a, sf_array_def_t *def);

void sf_array_clear(sf_array_t *a);

void sf_array_destroy(sf_array_t *a);

/**
 * Allocate n elements at the end of array, initialized to zero.
 */
void sf_array_alloc(sf_array_t *a, uint32_t n);

void sf_array_grow(sf_array_t *a, uint32_t nalloc);

void *sf_array_push(sf_array_t *a, const void *elt);

void *sf_array_push_front(sf_array_t *a, const void *elt);

void sf_array_pop(sf_array_t *a);

void sf_array_pop_front(sf_array_t *a);

void *sf_array_nth(sf_array_t *l, uint32_t nth);

#define sf_array_head(l) ((l)->elts)

#define sf_array_tail(l) sf_array_nth(l, sf_array_cnt(l) - 1)

#define sf_array_cnt(l) ((l)->nelts)

#define sf_array_nalloc(l) ((l)->def.nalloc)


typedef struct sf_array_iter sf_array_iter_t;
struct sf_array_iter {
    sf_array_t *a;
    uint32_t    idx;
    int32_t     order;
};


/**
 * Get the first iterator of array.
 *
 * Return SF_FALSE if there is no element in the array.
 *
 * The order of iteration is their physical order.
 */
sf_bool_t sf_array_begin(sf_array_t *a, sf_array_iter_t *iter);

/**
 * Get the last iterator of array and iterate backwards.
 */
sf_bool_t sf_array_rbegin(sf_array_t *a, sf_array_iter_t *iter);

void sf_array_end(sf_array_t *a, sf_array_iter_t *iter);

/**
 * Insert the specity element to an array.
 *
 * The new element will be inserted _behind_ the iter.
 */
void *sf_array_insert(sf_array_t *a, sf_array_iter_t *iter, const void *elt);

/**
 * Remove the specity element from an array.
 *
 * The iter will then point to the _previous_ element of removed element.
 */
void sf_array_remove(sf_array_t *a, sf_array_iter_t *iter);

/**
 * Walk to the 'next' element in an array.
 *
 * Return SF_FALSE if there are no more elements to iterate.
 */
sf_bool_t sf_array_iter_next(sf_array_iter_t *iter);

#define sf_array_iter_elt(iter) sf_array_nth((iter)->a, (iter)->idx)


#ifdef __cplusplus
}
#endif

#endif /* SF_ARRAY_H */
