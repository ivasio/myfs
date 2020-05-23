#include <string.h>

#include <filesystem.h>


int cd(fs_t* fs, web_request_t* request, web_response_t* response) {
    inode_t* directory;
    if (fs_find_inode(fs, request->args[0], FILE_DIR, &directory) < 0) {
        response->status = RESPONSE_INVALID_REQUEST;
        char* msg = "No such directory";
        response->buff = (char*) calloc(strlen(msg), sizeof(char));
        strcpy(response->buff, msg);
        response->len = strlen(response->buff);
        return 0;
    }
    if (fs_set_current_directory(fs, directory) < 0) {
        response->status = RESPONSE_SERVER_ERROR;
        char* msg = "Failed to set directory as current";
        response->buff = (char*) calloc(strlen(msg), sizeof(char));
        strcpy(response->buff, msg);
        response->len = strlen(response->buff);
        return 0;
    }
    response->status = RESPONSE_OK;
    char* msg = "OK";
    response->buff = (char*) calloc(strlen(msg), sizeof(char));
    strcpy(response->buff, msg);
    response->len = 2;
    return 0;
}


int ls_form_message(fs_t* fs, inode_t* file, char* message) {
    strcpy(message, file->file_name);
    strcat(message, ": ");
    if (file->file_type == FILE_FILE) {
        strcat(message, "file\n");
    } else {
        strcat(message, "directory. Contains:\n");
        inode_t* child;
        for (int i = 0; i < fs->disc->head_block->n_blocks_in_file && file->contents[i] != 0; i++) {
            if (fs_get_inode(fs, &child, file->contents[i]) < 0) {
                return -1;
            } else {
                strcat(message, child->file_name);
                strcat(message, ": ");
                if (child->file_type == FILE_FILE) {
                    strcat(message, "file\n");
                } else {
                    strcat(message, "directory\n");
                }
            }
        }
    }
    return 0;
}

int ls(fs_t* fs, web_request_t* request, web_response_t* response) {
    inode_t* file;
    if (fs_find_inode(fs, request->args[0], FILE_DIR, &file) < 0) {
        if (fs_find_inode(fs, request->args[0], FILE_FILE, &file) < 0) {
            response->status = RESPONSE_INVALID_REQUEST;
            char* msg = "No such directory";
            response->buff = (char*) calloc(strlen(msg), sizeof(char));
            strcpy(response->buff, msg);
            response->len = strlen(response->buff);
            return 0;
        }
    }

    char* message = (char*) calloc(256, sizeof(char));
    if (ls_form_message(fs, file, message) < 0) {
        response->status = RESPONSE_SERVER_ERROR;
        char* msg = "Failed to get contents";
        response->buff = (char*) calloc(strlen(msg), sizeof(char));
        strcpy(response->buff, msg);
        response->len = strlen(response->buff);
    } else {
        response->status = RESPONSE_OK;
        response->buff = message;
        response->len = strlen(response->buff);
    }
    return 0;
}



int cp(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    char* msg = "OK";
    response->buff = (char*) calloc(strlen(msg), sizeof(char));
    strcpy(response->buff, msg);
    response->len = 2;
    return 0;
}

int cp_to_remote(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    char* msg = "OK";
    response->buff = (char*) calloc(strlen(msg), sizeof(char));
    strcpy(response->buff, msg);
    response->len = 2;
    return 0;
}

int cp_from_remote(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    char* msg = "OK";
    response->buff = (char*) calloc(strlen(msg), sizeof(char));
    strcpy(response->buff, msg);
    response->len = 2;
    return 0;
}

int rm(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    char* msg = "OK";
    response->buff = (char*) calloc(strlen(msg), sizeof(char));
    strcpy(response->buff, msg);
    response->len = 2;
    return 0;
}

int mv(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    char* msg = "OK";
    response->buff = (char*) calloc(strlen(msg), sizeof(char));
    strcpy(response->buff, msg);
    response->len = 2;
    return 0;
}

int mkdir(fs_t* fs, web_request_t* request, web_response_t* response) {
    inode_t* inode;
    if (fs_create_file(fs, request->args[0], FILE_DIR, &inode) < 0) {
        response->status = RESPONSE_SERVER_ERROR;
        char* msg = "Failed to create directory";
        response->buff = (char*) calloc(strlen(msg), sizeof(char));
        strcpy(response->buff, msg);
        response->len = strlen(response->buff);
    } else {
        response->status = RESPONSE_OK;
        char* msg = "OK";
        response->buff = (char*) calloc(strlen(msg), sizeof(char));
        strcpy(response->buff, msg);
        response->len = 2;
    }
    return 0;
}

int cat(fs_t* fs, web_request_t* request, web_response_t* response) {
    response->status = RESPONSE_OK;
    char* msg = "File content goes here";
    response->buff = (char*) calloc(strlen(msg), sizeof(char));
    strcpy(response->buff, msg);
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
