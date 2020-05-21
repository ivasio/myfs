#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include <filesystem.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>


void map_disc(fs_t *fs, void *disc);

void fill_default_head_block(head_block_t *head_block);

int cd(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "OK";
    response->len = 2;
    return 0;
}


int ls(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "List of files goes here";
    response->len = strlen(response->buff);
    return 0;
}


int cp(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "OK";
    response->len = 2;
    return 0;
}


int cp_to_remote(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "OK";
    response->len = 2;
    return 0;
}


int cp_from_remote(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "OK";
    response->len = 2;
    return 0;
}


int rm(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "OK";
    response->len = 2;
    return 0;
}


int mv(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "OK";
    response->len = 2;
    return 0;
}


int mkdir(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "OK";
    response->len = 2;
    return 0;
}


int cat(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    response->buff = "File content goes here";
    response->len = strlen(response->buff);
    return 0;
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

void map_disc(fs_t *fs, void *disc) {
    /// Only setting the pointers to the starts of all the sections
    fs->disc->head_block = (head_block_t*)disc;
    disc += sizeof(head_block_t);

    fs->disc->inodes_map = (char*)disc;
    disc += fs->disc->head_block->n_inodes_total * sizeof(char);

    fs->disc->blocks_map = (char*)disc;
    disc += fs->disc->head_block->n_blocks_total * sizeof(char);

    fs->disc->inodes = (inode_t*)disc;
    disc += fs->disc->head_block->n_inodes_total * get_inode_size(fs->disc->head_block);

    fs->disc->blocks = (void**)disc;
}


unsigned get_disc_size(head_block_t* head_block) {
    return sizeof(head_block_t) +
        head_block->n_inodes_total * (sizeof(char) + get_inode_size(head_block)) +
        head_block->n_blocks_total * (sizeof(char) + head_block->block_size);
}


unsigned get_inode_size(head_block_t* head_block) {
    return sizeof(inode_t) - 2 * sizeof(void*) + head_block->file_name_len * sizeof(char) +
        head_block->n_blocks_in_file * sizeof(unsigned);
}

void fill_default_head_block(head_block_t *head_block) {
    head_block->file_name_len = 32;
    head_block->n_inodes_total = 1024;
    head_block->n_blocks_total = 2048;
    head_block->block_size = 1024;
    head_block->n_blocks_in_file = 128;
}

