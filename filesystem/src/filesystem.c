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
    int get_child_result;
    fs_get_root_dir(fs, &file);
    for (int i = 0; i < tokens.len - 1; i++) {
        get_child_result = fs_directory_get_child(fs, file, tokens.strings[i], &file);
        if (get_child_result < 0 || file->file_type != FILE_DIR) {
            return -1;
        }
    }
    if (tokens.len != 0) {
        get_child_result = fs_directory_get_child(fs, file, tokens.strings[tokens.len - 1], &file);
        if (get_child_result < 0) {
            return -1;
        }
    }
    if (file->file_type != file_type) {
        return -1;
    }

    *result = file;
    str_array_destroy(&tokens);
    return 0;
}


int fs_create_file(fs_t *fs, char *full_path, enum file_type_t file_type, inode_t** result) {
    str_array tokens;
    parse_path(full_path, &tokens);
    if (tokens.len == 0) {
        return -1;
    }

    inode_t* directory;
    int get_child_result;
    fs_get_root_dir(fs, &directory);
    for (int i = 0; i < tokens.len - 1; i++) {
        get_child_result = fs_directory_get_child(fs, directory, tokens.strings[i], &directory);
        if (get_child_result < 0) {
            return get_child_result;
        }
    }

    get_child_result = fs_directory_get_child(fs, directory,
        tokens.strings[tokens.len - 1], &directory);
    if (get_child_result == 0) {
        // file exists
        return -1;
    }

    unsigned inode_idx = fs_find_free_inode(fs);
    fs->disc->inodes_map[inode_idx] = INODE_OCCUPIED;
    fs_get_inode(fs, result, inode_idx);
    (*result)->file_type = file_type;
    strcpy((*result)->file_name, tokens.strings[tokens.len - 1]);

    for (int i = 0; i < fs->disc->head_block->n_blocks_in_file; i++) {
        if (directory->contents[i] == 0) {
            directory->contents[i] = inode_idx;
            break;
        }
    }
    return 0;
}

unsigned int fs_find_free_inode(fs_t *fs) {
    for (int i = 0; i < fs->disc->head_block->n_inodes_total; i++) {
        if (fs->disc->inodes_map[i] == INODE_FREE) return i;
    }
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
    return fs_get_inode(fs, dir, 1);
}


int fs_get_inode(fs_t* fs, inode_t **inode, unsigned inode_idx) {
    if (inode_idx >= fs->disc->head_block->n_inodes_total) {
        return -1; // improve error handling
    }
    *inode = fs->disc->inodes + fs->disc->head_block->inode_size * inode_idx;
    (*inode)->file_name = (char*)((*inode) + sizeof(file_type_t) + sizeof(char*) + sizeof(unsigned*));
    (*inode)->contents = (void*)((*inode)->file_name + fs->disc->head_block->file_name_len);
    return 0;
}


int setup_fs(fs_t *fs) {
    int disc_fd = open("/tmp/ivasio-fs-disc", O_RDWR);
    if (disc_fd == -1) {
        if (errno != ENOENT) {
            perror("Failed to open disc file");
            return disc_fd;
        }
        disc_fd = open("/tmp/ivasio-fs-disc", O_RDWR | O_CREAT);

        head_block_t default_head_block;
        fill_default_head_block(&default_head_block);

        unsigned disc_size = get_disc_size(&default_head_block);
        char zeros_buf[32] = {0};
        for (unsigned i = 0; i < disc_size / 32 + 1; i++) {
            lseek(disc_fd, i * 32, SEEK_SET);
            write(disc_fd, zeros_buf, 32);
        }
        void *disc = mmap(NULL, disc_size, PROT_READ | PROT_WRITE, MAP_SHARED, disc_fd, 0);
        if(disc == MAP_FAILED) {
            return -1;
        }
        close(disc_fd);

        // set initial values for fs
        memcpy(disc, &default_head_block, sizeof(head_block_t));
        map_disc(fs, disc);

        fs_get_root_dir(fs, &(fs->current_dir));
        fs_setup_operations(fs);

        return 0;
    } else {
        void *disc = mmap(NULL, sizeof(head_block_t), PROT_READ | PROT_WRITE, MAP_SHARED, disc_fd, 0);
        head_block_t *head_block = (head_block_t *) disc;
        unsigned disc_size = get_disc_size(head_block);

        mmap(disc, disc_size, PROT_READ | PROT_WRITE, MAP_SHARED, disc_fd, 0);
        close(disc_fd);

        map_disc(fs, disc);

        fs_get_root_dir(fs, &(fs->current_dir));
        fs_setup_operations(fs);
        return 0;
    }
}


void finalize_fs(fs_t *fs) {
    munmap(fs->disc, get_disc_size(fs->disc->head_block));
}
