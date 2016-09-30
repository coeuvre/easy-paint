#include <stdio.h>

#include <sf/utils.h>

int main(void) {
    int i;

    for (i = 0; i < 3000; ++i) {
        fprintf(stdout, "%d --> %d\n", i, sf_power_2(i));
        i = sf_power_2(i);
    }
    return 0;
}
