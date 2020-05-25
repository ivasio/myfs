#include <string.h>

#include <filesystem.h>
#include <data_structures.h>


int parse_path(char *full_path, str_array *tokens_arr, char *is_absolute) {
    const char* delimiter = "/";

    if (full_path[0] == '/') {
        *is_absolute = 1;
    } else {
        *is_absolute = 0;
    }

    if (full_path[0] == '.') {
        full_path += 1;
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


int setup_path_traversal(fs_t* fs, char* full_path, str_array* tokens, inode_t** file) {
    char is_absolute = 0;
    int parse_path_result = parse_path(full_path, tokens, &is_absolute);
    if (parse_path_result < 0) {
        return parse_path_result;
    }
    if (is_absolute) {
        fs_get_root_dir(fs, file);
    } else {
        *file = fs->current_dir;
    }
    return 0;
}


void map_disc(fs_t *fs, void *disc) {
    /// Setting the pointers to the starts of all the sections & initializing `/` folder
    fs->disc->head_block = (head_block_t*)disc;
    disc += sizeof(head_block_t);

    fs->disc->inodes_map = (char*)disc;
    disc += fs->disc->head_block->n_inodes_total * sizeof(char);

    fs->disc->blocks_map = (char*)disc;
    disc += fs->disc->head_block->n_blocks_total * sizeof(char);

    fs->disc->inodes = (inode_t*)disc;
    disc += fs->disc->head_block->n_inodes_total * fs->disc->head_block->inode_size;

    fs->disc->blocks = (void**)disc;

    fs->disc->inodes_map[0] = INODE_OCCUPIED;
    fs->disc->inodes_map[1] = INODE_OCCUPIED;

//    inode_t* root_dir = (inode_t*)(fs->disc->inodes + fs->disc->head_block->inode_size);
//    root_dir->file_type = FILE_DIR;

    inode_t* root_dir = fs->disc->inodes + fs->disc->head_block->inode_size;
    root_dir->file_type = FILE_DIR;
    root_dir->file_name = (char*)(root_dir + sizeof(file_type_t) + sizeof(char*) + sizeof(unsigned*));
    root_dir->contents = (void*)(root_dir->file_name + fs->disc->head_block->file_name_len);
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
    head_block->n_inodes_total = 128;
    head_block->n_blocks_total = 512;
    head_block->block_size = 1024;
    head_block->n_blocks_in_file = 32;
    head_block->inode_size = get_inode_size(head_block);
}