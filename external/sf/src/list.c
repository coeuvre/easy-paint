#include <inttypes.h>
#include <string.h>

#include "sf/log.h"
#include "sf/list.h"
#include "sf/utils.h"


sf_result_t sf_list_init(sf_list_t *l, const sf_list_def_t *def) {
    if (l == NULL || def == NULL || def->size == 0) {
        return SF_INVAL;
    }

    l->def = *def;
    /* BUG: if l is alloc on the stack, and caller copy it to another space,
     * l->head.next and l->head.prev will not equal to &l->head.
     */
    l->head.prev = l->head.next = &l->head;
    l->nelts = 0;

    return sf_pool_init(&l->pool, 0);
}

void sf_list_clear(sf_list_t *l) {
    sf_list_iter_t iter;

    if (l->def.free) {
        if (sf_list_begin(l, &iter)) do {
            l->def.free(sf_list_iter_elt(&iter));
        } while (sf_list_iter_next(&iter));
    }

    l->head.prev = l->head.next = &l->head;
    l->nelts = 0;

    return sf_pool_clear(&l->pool);
}

void sf_list_destroy(sf_list_t *l) {
    sf_list_iter_t iter;

    if (l->def.free) {
        if (sf_list_begin(l, &iter)) do {
            l->def.free(sf_list_iter_elt(&iter));
        } while (sf_list_iter_next(&iter));
    }

    sf_pool_destroy(&l->pool);

    sf_memzero(l, sizeof(*l));
}

void *sf_list_push(sf_list_t *l, const void *elt) {
    sf_list_iter_t iter;

    sf_list_end(l, &iter);

    return sf_list_insert(l, &iter, elt);
}

void *sf_list_push_front(sf_list_t *l, const void *elt) {
    sf_list_iter_t iter;

    sf_list_begin(l, &iter);

    return sf_list_insert(l, &iter, elt);
}

void sf_list_pop(sf_list_t *l) {
    sf_list_iter_t iter;

    sf_list_end(l, &iter);
    iter.cur = iter.cur->prev;

    return sf_list_remove(l, &iter);
}

void sf_list_pop_front(sf_list_t *l) {
    sf_list_iter_t iter;

    sf_list_begin(l, &iter);

    return sf_list_remove(l, &iter);
}

void *sf_list_nth(sf_list_t *l, uint32_t nth) {
    uint32_t        cnt;
    int             n;
    sf_list_iter_t  iter;

    if ((cnt = sf_list_cnt(l)) > 0) {
        if (nth < cnt / 2) {
            sf_list_begin(l, &iter);
        } else {
            sf_list_rbegin(l, &iter);
            nth = cnt - 1 - nth;
        }

        n = 0;
        do {
            if (n == nth) {
                return sf_list_iter_elt(&iter);
            }
        } while (++n, sf_list_iter_next(&iter));
    }

    sf_log(SF_LOG_ERR, "sf_list_nth: %" PRIu32 " out of range.", nth);
    return NULL;
}

sf_bool_t sf_list_begin(sf_list_t *l, sf_list_iter_t *iter) {
    iter->l     = l;
    iter->order = 1;
    iter->cur   = l->head.next;
    /*
     * If iter->cur is point to l->head,
     * then `sf_list_iter_elt(iter)` will be zero because the next entry
     * in the /sf_list_t/ is /nelts/, and only if /nelts/ is equal to zero
     * that iter->cur can point to l->head.
     */
    return iter->cur != &l->head;
}

sf_bool_t sf_list_rbegin(sf_list_t *l, sf_list_iter_t *iter) {
    iter->l     = l;
    iter->order = -1;
    iter->cur   = l->head.prev;
    return iter->cur != &l->head;
}

void sf_list_end(sf_list_t *l, sf_list_iter_t *iter) {
    iter->l     = l;
    iter->order = 1;
    iter->cur   = &l->head;
}

void *sf_list_insert(sf_list_t *l, sf_list_iter_t *iter, const void *elt) {
    sf_list_node_t *prev = iter->cur->prev;
    sf_list_node_t *next = iter->cur;
    sf_list_node_t *node;
    void           *dst;

    if (iter->l != l) {
        sf_log(SF_LOG_ERR, "sf_list_insert: Invalid iterator.");
        return NULL;
    }

    node = sf_pool_alloc(&l->pool, sizeof(*node) + l->def.size);

    prev->next = node;
    node->prev = prev;

    node->next = next;
    next->prev = node;

    dst = node + 1;
    if (l->def.cpy) {
        l->def.cpy(dst, elt);
    } else {
        memcpy(dst, elt, l->def.size);
    }

    ++l->nelts;

    return dst;
}

void sf_list_remove(sf_list_t *l, sf_list_iter_t *iter) {
    sf_list_node_t *prev = iter->cur->prev;
    sf_list_node_t *next = iter->cur->next;
    sf_pool_node_t *node;

    if (iter->l != l) {
        sf_log(SF_LOG_ERR, "sf_list_insert: Invalid iterator.");
        return;
    }

    if (sf_list_cnt(l) == 0) {
        sf_log(SF_LOG_ERR, "sf_list_remove: empty list.");
        return;
    }

    if (l->def.free) {
        l->def.free(sf_list_iter_elt(iter));
    }

    for (node = l->pool.first; node; node = node->next) {
        uint8_t *last;

        last = (uint8_t *) sf_list_iter_elt(iter) + l->def.size;
        last = SF_ALIGN_PTR(last, SF_ALIGNMENT);
        if (node->last == last) {
            node->last -= (last - (uint8_t *) iter->cur);
            sf_log(SF_LOG_DEBUG, "sf_list_remove: clean pool for %zu bytes.",
                   last - (uint8_t *) iter->cur);
            break;
        }
    }

    prev->next = next;
    next->prev = prev;

    --l->nelts;

    if (iter->order > 0) {
        iter->cur = prev;
    } else {
        iter->cur = next;
    }
}

sf_bool_t sf_list_iter_next(sf_list_iter_t *iter) {
    if (iter->order > 0) {
        iter->cur = iter->cur->next;
    } else {
        iter->cur = iter->cur->prev;
    }
    return iter->cur != &iter->l->head;
}
