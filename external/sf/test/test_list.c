#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sf/list.h>
#include <sf/log.h>
#include <sf/utils.h>


int main(void) {
    uint32_t n;
    char ch;
    sf_list_iter_t  iter;
    sf_list_t       l;
    sf_list_def_t   def;

    sf_memzero(&def, sizeof(def));
    def.size = sizeof(char);

    (void) sf_list_init(&l, &def);

    if (sf_list_begin(&l, &iter) == SF_FALSE) {
        assert((*(int *) sf_list_iter_elt(&iter)) == 0);
    }

    for (ch = 'a'; ch <= 'z'; ++ch) {
        sf_list_push(&l, &ch);
    }

    while (sf_list_cnt(&l) > 10) {
        fprintf(stdout, "pop: %c\n", * (char *) sf_list_tail(&l));
        sf_list_pop(&l);
    }

    ch = '!';
    sf_list_push_front(&l, &ch);

    while (sf_list_cnt(&l) != 0) {
        fprintf(stdout, "pop: %c\n", * (char *) sf_list_tail(&l));
        sf_list_pop(&l);
    }

    for (ch = 'a'; ch <= 'z'; ++ch) {
        sf_list_push_front(&l, &ch);
    }

    if (n = 0, sf_list_begin(&l, &iter) == SF_TRUE) do {
        char *pch = sf_list_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
        if (n == 10) {
            ch = '!';
            sf_list_insert(&l, &iter, &ch);
        }
    } while (++n, sf_list_iter_next(&iter) == SF_TRUE);

    fprintf(stdout, "\n");

    for (n = 0; n < sf_list_cnt(&l); ++n) {
        fprintf(stdout, "%uth: %c\n", n, *(char *) sf_list_nth(&l, n));
    }

    sf_list_clear(&l);

    for (ch = 'a'; ch <= 'z'; ++ch) {
        sf_list_push_front(&l, &ch);
    }

    if (n = 0, sf_list_rbegin(&l, &iter) == SF_TRUE) do {
        char *pch = sf_list_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
        if (n == 10) {
            break;
        }
    } while (++n, sf_list_iter_next(&iter) == SF_TRUE);

    fprintf(stdout, "\n");
    fprintf(stdout, "cur[10]: %c\n", *(char *) sf_list_iter_elt(&iter));
    fprintf(stdout, "head: %c\n", *(char *) sf_list_head(&l));
    fprintf(stdout, "tail: %c\n", *(char *) sf_list_tail(&l));

    sf_list_remove(&l, &iter);
    fprintf(stdout, "after remove, cur: %c\n",
            *(char *) sf_list_iter_elt(&iter));
    ch = '!';
    sf_list_insert(&l, &iter, &ch);

    do {
        char *pch = sf_list_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
    } while (sf_list_iter_next(&iter) == SF_TRUE);
    fprintf(stdout, "\n");

    sf_list_destroy(&l);

    sf_memcheck();

    return 0;
}
