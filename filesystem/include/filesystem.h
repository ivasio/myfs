#pragma once

#include <myfs.h>


int parse_request(web_request_t* request, char* receive_buffer, char buf_len);
int process_request(web_request_t *request);


typedef struct fs_t {

} fs_t;


fs_t* setup_fs();
void run_fs(fs_t *fs, char *pipe_path);
void finalize_fs(fs_t *fs);
