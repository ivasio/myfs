#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include <filesystem.h>
#include "operations.c"


int fs_find_directory(fs_t* fs, char* full_path, inode_t* directory) {

}


int setup_fs(fs_t *fs) {
    int disc_fd = open("/tmp/ivasio-fs-disc", O_RDWR);
    if (disc_fd == -1) {
        if (errno == ENOENT) { // file does not exist
            disc_fd = open("/tmp/ivasio-fs-disc", O_RDWR | O_CREAT);
            head_block_t default_head_block;
            fill_default_head_block(&default_head_block);
            write(disc_fd, &default_head_block, sizeof(head_block_t));
        } else {
            perror("Failed to open disc file");
            return disc_fd;
        }
    }

    // map memory
    void* disc = NULL;
    mmap(disc, sizeof(head_block_t), PROT_READ | PROT_WRITE, MAP_SHARED, disc_fd, NULL);

    head_block_t* head_block = (head_block_t*)disc;
    unsigned disc_size = get_disc_size(head_block);
    mmap(disc, disc_size, PROT_READ | PROT_WRITE, MAP_SHARED, disc_fd, NULL);
    close(disc_fd);

    map_disc(fs, disc);

    fs->current_dir = 0;  // directory /

    fs->operations[OP_CD] = cd;
    fs->operations[OP_LS] = ls;
    fs->operations[OP_CP] = cp;
    fs->operations[OP_CP_TO_REMOTE] = cp_to_remote;
    fs->operations[OP_CP_FROM_REMOTE] = cp_from_remote;
    fs->operations[OP_RM] = rm;
    fs->operations[OP_MV] = mv;
    fs->operations[OP_MKDIR] = mkdir;
    fs->operations[OP_CAT] = cat;

    return 1;
}


void finalize_fs(fs_t *fs) {
    munmap(fs->disc, get_disc_size(fs->disc->head_block));
}
