#ifndef SF_UTILS_H
#define SF_UTILS_H


#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "defs.h"


#ifdef __cplusplus
extern "C" {
#endif


#define SF_ALIGN_PTR(p, a)                                  \
    (uint8_t *) (((uintptr_t) (p) + ((uintptr_t) a - 1))    \
                 & ~((uintptr_t) a - 1))


/** sf_swap(T, x, y) */
#define sf_swap(T, x, y) do {       \
    T __sf_swap_tmp__ = x;          \
    x = y;                          \
    y = __sf_swap_tmp__;            \
} while(0)

/** sf_max(x, y) */
#ifdef __GNUC__
#  define sf_max(x, y) ({               \
       typeof(x) __sf_max_x__ = (x);    \
       typeof(y) __sf_max_y__ = (y);    \
       __sf_max_x__ > __sf_max_y__      \
           ? __sf_max_x__               \
           : __sf_max_y__;              \
   })
#else
#  define sf_max(x, y) ((x) > (y) ? (x) : (y))
#endif /* __GNUC__ */

/** sf_min(x, y) */
#ifdef __GNUC__
#  define sf_min(x, y) ({ \
       typeof(x) __sf_min_x__ = (x);    \
       typeof(y) __sf_min_y__ = (y);    \
       __sf_min_x__ < __sf_min_y__      \
           ? __sf_min_x__               \
           : __sf_min_y__;              \
   })
#else
#  define sf_min(x, y) ((x) < (y) ? (x) : (y))
#endif /* __GNUC__ */

/** sf_offsetof(type, filed) */
#ifdef __GNUC__
#  define sf_offsetof(type, field) __builtin_offsetof(type, field)
#else
#  define sf_offsetof(type, field) ((size_t) (&((type *) 0)->field))
#endif /* __GNUC__ */

/** sf_container_of(ptr, type, filed) */
#ifdef __GNUC__
#  define sf_container_of(ptr, type, filed) ({                              \
       const typeof(((type *) 0)->filed) *__sf_container_of_ptr__ = (ptr);  \
       (type *) ((char *) __sf_container_of_ptr__                           \
                 - sf_offsetof(type, filed));                               \
   })
#else
#  define sf_container_of(ptr, type, member) \
       ((type *) ((char *) ptr - sf_offsetof(type, member)))
#endif /* __GNUC__ */

/** Round up to next power of 2 */
uint32_t sf_power_2(uint32_t n);

#define sf_memzero(p, size) (void) memset(p, 0, size)

void *sf_alloc(size_t size);

void *sf_calloc(size_t size);

void *sf_realloc(void *p, size_t size);

void sf_free(void *p);

sf_bool_t sf_memcheck(void);


#ifdef __cplusplus
}
#endif

#endif /* SF_UTILS_H */
