#pragma once

#include <myfs.h>


typedef struct fs_t {

} fs_t;
fs_t* setup_fs();


void run_fs(fs_t *fs, char *pipe_path);


void finalize_fs(fs_t *fs);
int serve(fs_t *fs, char *pipe_path);

int read_request(web_request_t *request, char *pipe_path);
int parse_request(web_request_t *request, char op_code, char buf_len, char *receive_buffer);
int process_request(fs_t *fs, web_response_t *response, web_request_t *request);
int send_response(web_response_t* response, char *pipe_path);
