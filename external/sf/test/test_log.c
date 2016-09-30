#include <stdio.h>

#include <sf/log.h>


sf_bool_t on_logging_true(sf_log_level_t level, const char *str) {
    fprintf(stdout, "on_logging_true: %s", str);
    return SF_TRUE;
}

sf_bool_t on_logging_false(sf_log_level_t level, const char *str) {
    fprintf(stdout, "on_logging_false: %s", str);
    return SF_FALSE;
}

int main(void) {
    sf_log(SF_LOG_DEBUG, "log from main.");
    sf_log(SF_LOG_ERR, "log from main.");

    sf_log_set_level(SF_LOG_ERR);
    sf_log_set_hook(on_logging_true);

    sf_log(SF_LOG_DEBUG, "log from test_log.");
    sf_log(SF_LOG_ERR, "log from test_log.");

    sf_log_set_hook(on_logging_false);
    sf_log(SF_LOG_DEBUG, "log from test_log.");
    sf_log(SF_LOG_ERR, "log from test_log.");

    return 0;
}
