#ifndef SF_LIST_H
#define SF_LIST_H


#include "defs.h"
#include "pool.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * # Double-linked list
 *
 * ## Define an integer list
 *
 * ```
 * sf_list_t        l;
 * sf_list_def_t    def;
 *
 * sf_memzero(&def, sizeof(def));
 * def.size = sizeof(int);
 *
 * sf_list_init(&l, &def);
 * ```
 *
 * ## Iterating the list
 *
 * ```
 * sf_list_iter_t iter;
 *
 * if (sf_list_begin(&l, &iter)) do {
 *     int elt = *(int *) sf_list_iter_elt(&iter);
 *
 *     ...
 *
 * } while (sf_list_iter_next(&iter));
 * ```
 */

typedef struct sf_list_def sf_list_def_t;
struct sf_list_def {
    /* size of an element in byte */
    size_t  size;

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

typedef struct sf_list_node sf_list_node_t;
struct sf_list_node {
    sf_list_node_t *prev;
    sf_list_node_t *next;
    /*
     * flexible array for allocated memory
     */
};

typedef struct sf_list sf_list_t;
struct sf_list {
    sf_list_node_t  head;
    uint32_t        nelts;
    sf_list_def_t   def;
    sf_pool_t       pool;
};

sf_result_t sf_list_init(sf_list_t *l, const sf_list_def_t *def);

void sf_list_clear(sf_list_t *l);

void sf_list_destroy(sf_list_t *l);

void *sf_list_push(sf_list_t *l, const void *elt);

void *sf_list_push_front(sf_list_t *l, const void *elt);

void sf_list_pop(sf_list_t *l);

void sf_list_pop_front(sf_list_t *l);

void *sf_list_nth(sf_list_t *l, uint32_t nth);

#define sf_list_head(l) ((void *) ((l)->head.next + 1))

#define sf_list_tail(l) ((void *) ((l)->head.prev + 1))

#define sf_list_cnt(l) ((l)->nelts)


typedef struct sf_list_iter sf_list_iter_t;
struct sf_list_iter {
    sf_list_t      *l;
    sf_list_node_t *cur;
    int32_t         order;
};


/**
 * Get the first iterator of list.
 *
 * Return SF_FALSE if there is no element in the list.
 *
 * The order of iteration is their physical order.
 */
sf_bool_t sf_list_begin(sf_list_t *l, sf_list_iter_t *iter);

/**
 * Get the last iterator of list and iterate backwards.
 */
sf_bool_t sf_list_rbegin(sf_list_t *l, sf_list_iter_t *iter);

void sf_list_end(sf_list_t *l, sf_list_iter_t *iter);

/**
 * Insert the specity element to a list.
 *
 * The new element will be inserted _behind_ the iter.
 */
void *sf_list_insert(sf_list_t *l, sf_list_iter_t *iter, const void *elt);

/**
 * Remove the specity element from a list.
 *
 * The iter will then point to the _previous_ element of removed element.
 */
void sf_list_remove(sf_list_t *l, sf_list_iter_t *iter);

/**
 * Walk to the 'next' element in a list.
 *
 * Return SF_FALSE if there are no more elements to iterate.
 */
sf_bool_t sf_list_iter_next(sf_list_iter_t *iter);

/**
 * Get a pointer which point to the iterator's element.
 */
#define sf_list_iter_elt(iter) ((void *) ((iter)->cur + 1))


#ifdef __cplusplus
}
#endif

#endif /* SF_LIST_H */
