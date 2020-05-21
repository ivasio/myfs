#pragma once

#include <myfs.h>


typedef struct head_block_t {
    unsigned file_name_len;
    unsigned n_blocks_in_file;
    unsigned n_inodes_total;
    unsigned n_blocks_total;
    unsigned block_size;
} head_block_t;


typedef enum file_type_t {
    FILE,
    DIR
} file_type_t;


typedef struct inode_t {
    file_type_t file_type;
    unsigned link_count;
    char* file_name; // only current part of the name
    unsigned* contents; // pointers to blocks for file or to other inodes for directory
} inode_t;


typedef struct disc_t {
    head_block_t* head_block;
    char* inodes_map;
    char* blocks_map;
    inode_t* inodes;
    void** blocks;
} disc_t;


typedef struct fs_t {
    disc_t* disc;
    int (*operations[N_FS_OPERATIONS])(struct fs_t*, web_request_t*, web_response_t*);
} fs_t;


int setup_fs(fs_t *fs);
void finalize_fs(fs_t *fs);

unsigned get_disc_size(head_block_t* head_block);
unsigned get_inode_size(head_block_t* head_block);

void run_fs(fs_t *fs, char *pipe_path);
int serve(fs_t *fs, char *pipe_path);

int read_request(web_request_t *request, char *pipe_path);
int parse_request(web_request_t *request, char op_code, char buf_len, char *receive_buffer);
int process_request(fs_t *fs, web_response_t *response, web_request_t *request);
int send_response(web_response_t* response, char *pipe_path);
