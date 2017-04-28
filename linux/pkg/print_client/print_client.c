#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "dataport.h"
#include "consumes_event.h"
#include "emits_event.h"

int main(int argc, char *argv[]) {

    int data_fd = open("/dev/camkes_data", O_RDWR);
    assert(data_fd >= 0);

    int do_print_fd = open("/dev/camkes_do_print", O_RDWR);
    assert(do_print_fd >= 0);

    int done_printing_fd = open("/dev/camkes_done_printing", O_RDWR);
    assert(done_printing_fd >= 0);

    char *data = (char*)dataport_mmap(data_fd);
    assert(data != MAP_FAILED);

    ssize_t dataport_size = dataport_get_size(data_fd);
    assert(dataport_size > 0);

    for (int i = 1; i < argc; i++) {
        strncpy(data, argv[i], dataport_size);
        emits_event_emit(do_print_fd);
        consumes_event_wait(done_printing_fd);
    }

    close(data_fd);
    close(do_print_fd);
    close(done_printing_fd);

    return 0;
}
