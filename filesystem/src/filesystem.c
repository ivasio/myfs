#include <stdlib.h>
#include <stdio.h>

#include <filesystem.h>


int main() {
    fs_t* fs = setup_fs();
    if (fs == NULL) {
        perror("Falied to initialize file system");
        return 1;
    }

    char* pipe_path = "/tmp/ivasio-fs-fifo";
    run_fs(fs, pipe_path);
    finalize_fs(fs);
}

void finalize_fs(fs_t *fs) {

}

void run_fs(fs_t *fs, char *pipe_path) {

}

fs_t *setup_fs() {
    return NULL;
}
