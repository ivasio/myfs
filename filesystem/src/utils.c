#include <string.h>

#include <filesystem.h>
#include <data_structures.h>


int parse_path(char *full_path, str_array* tokens_arr) {
    const char* delimiter = "/";
    if (full_path[0] != delimiter[0]) {
        return -1;
    }

    unsigned long path_len = strlen(full_path);
    int n_tokens = 0;
    for (int i = 0; i < path_len - 1; i++) {
        if (full_path[i] == delimiter[0]) n_tokens += 1;
    }

    char** tokens = (char**) calloc(n_tokens, sizeof(char*));
    int token_idx = 0;
    tokens[token_idx] = strtok(full_path, delimiter);
    while(tokens[token_idx] != NULL) {
        token_idx += 1;
        tokens[token_idx] = strtok(NULL, delimiter);
    }

    tokens_arr->len = n_tokens;
    tokens_arr->strings = tokens;
    return 0;
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
    disc += fs->disc->head_block->n_inodes_total * fs->disc->head_block->inode_size;

    fs->disc->blocks = (void**)disc;
}

unsigned get_disc_size(head_block_t* head_block) {
    return sizeof(head_block_t) +
        head_block->n_inodes_total * (sizeof(char) + head_block->inode_size) +
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
    head_block->inode_size = get_inode_size(head_block);
}