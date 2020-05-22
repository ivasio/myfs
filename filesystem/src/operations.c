#pragma once

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <filesystem.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>


int cd(fs_t* fs, web_request_t* request, web_response_t* response) {
    inode_t directory;
    fs_find_inode(fs, request->args[0], FILE_DIR, &directory);
    fs_set_current_directory(fs, &directory);
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