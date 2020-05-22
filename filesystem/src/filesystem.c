#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include <data_structures.h>
#include <filesystem.h>


int fs_find_inode(fs_t* fs, char* full_path, file_type_t file_type, inode_t **result) {
    str_array tokens;
    int parse_path_result = parse_path(full_path, &tokens);
    if (parse_path_result < 0) {
        return parse_path_result;
    }

    inode_t* file = NULL;
    fs_get_root_dir(fs, &file);
    for (int i = 0; i < tokens.len - 1; i++) {
        fs_directory_get_child(fs, file, tokens.strings[i], &file);
        if (file->file_type != FILE_DIR) {
            return -1;
        }
    }
    if (tokens.len != 0) {
        fs_directory_get_child(fs, file, tokens.strings[tokens.len - 1], &file);
    }
    if (file->file_type != file_type) {
        return -1;
    }

    *result = file;
    str_array_destroy(&tokens);
    return 0;
}


int fs_set_current_directory(fs_t* fs, inode_t* directory) {
    fs->current_dir = directory;
    return 0;
}


int fs_directory_get_child(fs_t *fs, inode_t *parent, char *child_name, inode_t **result) {
    inode_t* child = NULL;
    for (unsigned i = 0; i < fs->disc->head_block->n_blocks_in_file; i++) {
        fs_get_inode(fs, &child, parent->contents[i]);
        if (strcmp(child->file_name, child_name) == 0) {
            *result = child;
            return 0;
        }
    }
    return -1;
}


int fs_get_root_dir(fs_t *fs, inode_t** dir) {
    return fs_get_inode(fs, dir, 0);
}


int fs_get_inode(fs_t* fs, inode_t **inode, unsigned inode_idx) {
    if (inode_idx >= fs->disc->head_block->n_inodes_total) {
        return -1; // improve error handling
    }
    *inode = fs->disc->inodes + fs->disc->head_block->inode_size * inode_idx;
    (*inode)->contents = (void*)((*inode)->file_name + fs->disc->head_block->file_name_len);
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

    void* disc = mmap(NULL, sizeof(head_block_t), PROT_READ | PROT_WRITE, MAP_SHARED, disc_fd, 0);

    head_block_t* head_block = (head_block_t*)disc;
    unsigned disc_size = get_disc_size(head_block);
    mmap(disc, disc_size, PROT_READ | PROT_WRITE, MAP_SHARED, disc_fd, 0);
    close(disc_fd);

    map_disc(fs, disc);

    fs_get_root_dir(fs, &(fs->current_dir));
    fs_setup_operations(fs);
    return 1;
}


void finalize_fs(fs_t *fs) {
    munmap(fs->disc, get_disc_size(fs->disc->head_block));
}
