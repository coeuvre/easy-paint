#ifndef SF_LOG_H
#define SF_LOG_H


#include <stdarg.h>

#include "sf/defs.h"


#ifdef __cplusplus
extern "C" {
#endif


enum sf_log_level {
    SF_LOG_PANIC = 0,
    SF_LOG_ERR,
    SF_LOG_WARN,
    SF_LOG_INFO,
    SF_LOG_DEBUG,
    SF_LOG_NLEVELS,
};
typedef enum sf_log_level sf_log_level_t;

/** Callback function called when there are something logged.
 *
 * Return value specify whether the log will output to stderr.
 */
typedef sf_bool_t (sf_log_func)(sf_log_level_t level, const char *str);


sf_result_t sf_log_set_hook(sf_log_func *f);

sf_result_t sf_log_set_level(sf_log_level_t level);

sf_log_level_t sf_log_get_level(void);

sf_result_t sf_log(sf_log_level_t level, const char *fmt, ...);

sf_result_t sf_logv(sf_log_level_t level, const char *fmt, va_list ap);


#ifdef __cplusplus
}
#endif

#endif /* SF_LOG_H */
