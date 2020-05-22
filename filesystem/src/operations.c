#pragma once

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

#include <filesystem.h>


int cd(fs_t* fs, web_request_t* request, web_response_t* response) {
    inode_t* directory;
    fs_find_inode(fs, request->args[0], FILE_DIR, &directory);
    fs_set_current_directory(fs, directory);
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

int fs_setup_operations(fs_t *fs) {
    fs->operations[OP_CD] = cd;
    fs->operations[OP_LS] = ls;
    fs->operations[OP_CP] = cp;
    fs->operations[OP_CP_TO_REMOTE] = cp_to_remote;
    fs->operations[OP_CP_FROM_REMOTE] = cp_from_remote;
    fs->operations[OP_RM] = rm;
    fs->operations[OP_MV] = mv;
    fs->operations[OP_MKDIR] = mkdir;
    fs->operations[OP_CAT] = cat;

    return 0;
}
