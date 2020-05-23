#include <string.h>

#include <filesystem.h>


int cd(fs_t* fs, web_request_t* request, web_response_t* response) {
    inode_t* directory;
    if (fs_find_inode(fs, request->args[0], FILE_DIR, &directory) < 0) {
        response->status = RESPONSE_INVALID_REQUEST;
        response->buff = "No such directory";
        response->len = strlen(response->buff);
        return 0;
    }
    if (fs_set_current_directory(fs, directory) < 0) {
        response->status = RESPONSE_SERVER_ERROR;
        response->buff = "Failed to set directory as current";
        response->len = strlen(response->buff);
        return 0;
    }
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
    inode_t* inode;
    if (fs_create_file(fs, request->args[0], FILE_DIR, &inode) < 0) {
        response->status = RESPONSE_SERVER_ERROR;
        response->buff = "Failed to create directory";
        response->len = strlen(response->buff);
    } else {
        response->status = RESPONSE_OK;
        response->buff = "OK";
        response->len = 2;
    }
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
