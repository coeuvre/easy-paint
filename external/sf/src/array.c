#include <inttypes.h>

#include "sf/array.h"
#include "sf/log.h"
#include "sf/utils.h"


#define SF_ARRAY_IDX_END -1


static void sf_array_move_elts(sf_array_t *a, void *dst, void *src) {
    if (a->def.cpy) {
        a->def.cpy(dst, src);
    } else {
        memcpy(dst, src, a->def.size);
    }

    if (a->def.free) {
        a->def.free(src);
    }
}


sf_result_t sf_array_init(sf_array_t *a, sf_array_def_t *def) {
    if (a == NULL || def == NULL || def->size == 0) {
        return SF_INVAL;
    }

    a->def = *def;
    if (a->def.nalloc == 0) {
        a->def.nalloc = SF_ARRAY_DEF_NALLOC(a);
    } else {
        a->def.nalloc = sf_power_2(a->def.nalloc);
    }
    a->nelts = 0;
    a->elts = sf_alloc(a->def.nalloc * a->def.size);

    return SF_OK;
}

void sf_array_clear(sf_array_t *a) {
    if (a->def.free) {
        sf_array_iter_t iter;

        if (sf_array_begin(a, &iter)) do {
            a->def.free(sf_array_iter_elt(&iter));
        } while (sf_array_iter_next(&iter));
    }

    a->nelts = 0;
}

void sf_array_destroy(sf_array_t *a) {
    sf_array_clear(a);
    sf_free(a->elts);
    sf_memzero(a, sizeof(*a));
}

void sf_array_alloc(sf_array_t *a, uint32_t n) {
    void    *p;
    uint32_t cnt = sf_array_cnt(a);

    cnt += n;

    if (sf_array_nalloc(a) < cnt) {
        sf_array_grow(a, cnt);
    }

    p = sf_array_nth(a, sf_array_cnt(a));
    sf_memzero(p, a->def.size * n);

    a->nelts += n;
}

void sf_array_grow(sf_array_t *a, uint32_t nalloc) {
    nalloc = sf_power_2(nalloc);

    if (nalloc <= a->def.nalloc) {
        sf_log(SF_LOG_WARN, "sf_array_grow: %" PRIu32 " <= %" PRIu32 ".",
               nalloc, a->def.nalloc);
        return;
    }

    a->elts = sf_realloc(a->elts, nalloc * a->def.size);

    sf_log(SF_LOG_INFO, "sf_array_grow: %" PRIu32 " --> %" PRIu32 ".",
           a->def.nalloc, nalloc);

    a->def.nalloc = nalloc;
}

void *sf_array_push(sf_array_t *a, const void *elt) {
    sf_array_iter_t iter;

    sf_array_end(a, &iter);

    return sf_array_insert(a, &iter, elt);
}

void *sf_array_push_front(sf_array_t *a, const void *elt) {
    sf_array_iter_t iter;

    sf_array_begin(a, &iter);
    iter.idx = 0;

    return sf_array_insert(a, &iter, elt);
}

void sf_array_pop(sf_array_t *a) {
    sf_array_iter_t iter;

    sf_array_end(a, &iter);
    --iter.idx;

    return sf_array_remove(a, &iter);
}

void sf_array_pop_front(sf_array_t *a) {
    sf_array_iter_t iter;

    sf_array_begin(a, &iter);
    iter.idx = 0;

    return sf_array_remove(a, &iter);
}

void *sf_array_nth(sf_array_t *a, uint32_t nth) {
    /*
     * could get the end element for convience, but the action
     * is undefined.
     */
    if (nth > a->nelts) {
        sf_log(SF_LOG_ERR, "sf_array_nth: %" PRIu32 " out of range.", nth);
        return NULL;
    }

    return ((uint8_t *) a->elts) + nth * a->def.size;
}


sf_bool_t sf_array_begin(sf_array_t *a, sf_array_iter_t *iter) {
    iter->a     = a;
    iter->order = 1;
    if (sf_array_cnt(a) == 0) {
        iter->idx = SF_ARRAY_IDX_END;
        return SF_FALSE;
    }
    iter->idx = 0;
    return SF_TRUE;
}

sf_bool_t sf_array_rbegin(sf_array_t *a, sf_array_iter_t *iter) {
    iter->a     = a;
    iter->order = -1;

    if (sf_array_cnt(a) == 0) {
        iter->idx = SF_ARRAY_IDX_END;
        return SF_FALSE;
    }

    iter->idx = sf_array_cnt(a) - 1;
    return SF_TRUE;
}

void sf_array_end(sf_array_t *a, sf_array_iter_t *iter) {
    iter->a     = a;
    iter->order = 1;
    iter->idx   = a->nelts;
}

void *sf_array_insert(sf_array_t *a, sf_array_iter_t *iter, const void *elt) {
    uint32_t idx;
    void *dst;

    if (iter->a != a) {
        sf_log(SF_LOG_ERR, "sf_array_next: Invalid iterator.");
        return NULL;
    }

    if (sf_array_cnt(a) + 1 >= sf_array_nalloc(a)) {
        sf_array_grow(a, a->def.nalloc << 1);
    }

    if (sf_array_cnt(a) > 0) {
        for (idx = a->nelts; idx > iter->idx; --idx) {
            uint8_t *dst, *src;

            dst = sf_array_nth(a, idx);
            src = dst - a->def.size;
            sf_array_move_elts(a, dst, src);
        }
    }

    ++a->nelts;
    dst = sf_array_nth(a, iter->idx);
    if (a->def.cpy) {
        a->def.cpy(dst, elt);
    } else {
        memcpy(dst, elt, a->def.size);
    }

    ++iter->idx;
    return dst;
}

void sf_array_remove(sf_array_t *a, sf_array_iter_t *iter) {
    uint32_t idx;

    if (iter->a != a) {
        sf_log(SF_LOG_ERR, "sf_array_next: Invalid iterator.");
    }

    if (sf_array_cnt(a) == 0) {
        sf_log(SF_LOG_ERR, "sf_array_remove: empty array.");
        return;
    }

    if (a->def.free) {
        a->def.free(sf_array_iter_elt(iter));
    }

    for (idx = iter->idx + 1; idx < a->nelts; ++idx) {
        uint8_t *src, *dst;

        src = sf_array_nth(a, idx);
        dst = src - a->def.size;
        sf_array_move_elts(a, dst, src);
    }

    --a->nelts;

    if (iter->order > 0 && iter->idx > 0) {
        --iter->idx;
    }
}

sf_bool_t sf_array_iter_next(sf_array_iter_t *iter) {
    if (iter->idx == SF_ARRAY_IDX_END) {
        return SF_FALSE;
    }

    if (iter->order > 0) {
        if (++iter->idx >= sf_array_cnt(iter->a)) {
            iter->idx = SF_ARRAY_IDX_END;
            return SF_FALSE;
        }
    } else {
        if (iter->idx-- == 0 || iter->idx > sf_array_cnt(iter->a)) {
            iter->idx = SF_ARRAY_IDX_END;
            return SF_FALSE;
        }
    }

    return SF_TRUE;
}
