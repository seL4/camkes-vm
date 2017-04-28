#include <camkes.h>
#include <stdio.h>

int run(void) {

    while (1) {
        do_print_wait();

        printf("%s\n", (char*)data);

        done_printing_emit();
    }

    return 0;
}
