#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sf/array.h>
#include <sf/log.h>
#include <sf/utils.h>


int main(void) {
    uint32_t n;
    char ch;
    sf_array_iter_t  iter;
    sf_array_t       l;
    sf_array_def_t   def;

    sf_memzero(&def, sizeof(def));
    def.size = sizeof(char);

    (void) sf_array_init(&l, &def);

    for (ch = 'a'; ch <= 'z'; ++ch) {
        sf_array_push(&l, &ch);
    }

    while (sf_array_cnt(&l) > 10) {
        fprintf(stdout, "pop: %c\n", * (char *) sf_array_tail(&l));
        sf_array_pop(&l);
    }

    ch = '!';
    sf_array_push_front(&l, &ch);

    while (sf_array_cnt(&l) != 0) {
        fprintf(stdout, "pop: %c\n", * (char *) sf_array_tail(&l));
        sf_array_pop(&l);
    }

    for (ch = 'a'; ch <= 'z'; ++ch) {
        sf_array_push_front(&l, &ch);
    }

    if (n = 0, sf_array_begin(&l, &iter) == SF_TRUE) do {
        char *pch = sf_array_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
        if (n == 10) {
            ch = '!';
            sf_array_insert(&l, &iter, &ch);
        }
    } while (++n, sf_array_iter_next(&iter) == SF_TRUE);

    fprintf(stdout, "\n");

    for (n = 0; n < sf_array_cnt(&l); ++n) {
        fprintf(stdout, "%uth: %c\n", n, *(char *) sf_array_nth(&l, n));
    }

    sf_array_clear(&l);

    for (ch = 'a'; ch <= 'z'; ++ch) {
        sf_array_push_front(&l, &ch);
    }

    if (n = 0, sf_array_rbegin(&l, &iter) == SF_TRUE) do {
        char *pch = sf_array_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
        if (n == 10) {
            break;
        }
    } while (++n, sf_array_iter_next(&iter) == SF_TRUE);

    fprintf(stdout, "\n");
    fprintf(stdout, "cur[10]: %c\n", *(char *) sf_array_iter_elt(&iter));
    fprintf(stdout, "head: %c\n", *(char *) sf_array_head(&l));
    fprintf(stdout, "tail: %c\n", *(char *) sf_array_tail(&l));

    sf_array_remove(&l, &iter);
    fprintf(stdout, "after remove, cur: %c\n",
            *(char *) sf_array_iter_elt(&iter));
    ch = '!';
    sf_array_insert(&l, &iter, &ch);

    do {
        char *pch = sf_array_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
    } while (sf_array_iter_next(&iter) == SF_TRUE);
    fprintf(stdout, "\n");


    if (n = 0, sf_array_begin(&l, &iter) == SF_TRUE) do {
        char *pch = sf_array_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
        if (n == 10) {
            break;
        }
    } while (++n, sf_array_iter_next(&iter) == SF_TRUE);

    fprintf(stdout, "\n");
    fprintf(stdout, "cur[10]: %c\n", *(char *) sf_array_iter_elt(&iter));
    fprintf(stdout, "head: %c\n", *(char *) sf_array_head(&l));
    fprintf(stdout, "tail: %c\n", *(char *) sf_array_tail(&l));

    sf_array_remove(&l, &iter);
    fprintf(stdout, "after remove, cur: %c\n",
            *(char *) sf_array_iter_elt(&iter));
    ch = '!';
    sf_array_insert(&l, &iter, &ch);

    do {
        char *pch = sf_array_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
    } while (sf_array_iter_next(&iter) == SF_TRUE);
    fprintf(stdout, "\n");

    if (sf_array_begin(&l, &iter)) do {
        char *pch = sf_array_iter_elt(&iter);
        fprintf(stdout, "%c", *pch);
    } while (sf_array_iter_next(&iter));
    fprintf(stdout, "\n");

    sf_array_destroy(&l);

    return 0;
}
