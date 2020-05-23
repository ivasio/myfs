#pragma once

#include <data_structures.h>
#include <myfs.h>


typedef struct head_block_t {
    unsigned file_name_len;
    unsigned n_blocks_in_file;
    unsigned n_inodes_total;
    unsigned n_blocks_total;
    unsigned block_size;
    unsigned inode_size;
} head_block_t;


typedef enum file_type_t {
    FILE_FILE,
    FILE_DIR
} file_type_t;


typedef struct inode_t {
    file_type_t file_type;
    char* file_name; // only current part of the name
    unsigned* contents; // pointers to blocks for file or to other inodes for directory
    char* file_name_data;
    unsigned* contents_data;
} inode_t;


enum {
    INODE_FREE,
    INODE_OCCUPIED
};


typedef struct disc_t {
    head_block_t* head_block;
    char* inodes_map;
    char* blocks_map;
    inode_t* inodes;
    void** blocks;
} disc_t;


typedef struct fs_t {
    disc_t* disc;
    inode_t* current_dir;  // inode # of the current directory
    int (*operations[N_FS_OPERATIONS])(struct fs_t*, web_request_t*, web_response_t*);
} fs_t;


// fs lifecycle functions
int setup_fs(fs_t *fs);
void finalize_fs(fs_t *fs);


// fs file manipulation functions
int fs_find_inode(fs_t* fs, char* full_path, file_type_t file_type, inode_t **file);
int fs_get_current_directory(fs_t* fs, inode_t* directory);
int fs_set_current_directory(fs_t* fs, inode_t* directory);
int fs_get_root_dir(fs_t *fs, inode_t **dir);
int fs_directory_get_child(fs_t *fs, inode_t *parent, char *child_name, inode_t **child);
int fs_get_inode(fs_t* fs, inode_t **inode, unsigned inode_idx);
int fs_create_file(fs_t *fs, char *full_path, enum file_type_t file_type, inode_t** inode);
unsigned int fs_find_free_inode(fs_t *fs);


// fs ipc functions
void run_fs(fs_t *fs, char *pipe_path);
int read_request(web_request_t *request, char *pipe_path);
int serve(fs_t *fs, char *pipe_path);
int parse_request(web_request_t *request, char op_code, char buf_len, char *receive_buffer);
int process_request(fs_t *fs, web_response_t *response, web_request_t *request);
int send_response(web_response_t* response, char *pipe_path);


// utility functions
unsigned get_disc_size(head_block_t* head_block);
void map_disc(fs_t *fs, void *disc);
unsigned get_inode_size(head_block_t* head_block);
void fill_default_head_block(head_block_t *head_block);
int parse_path(char *full_path, str_array *tokens);
int fs_setup_operations(fs_t *fs);
