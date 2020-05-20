#pragma once

#include <stdlib.h>

#define N_FS_OPERATIONS 9

// List of all supported operations
typedef enum FS_OPERATION {
    OP_CD,
    OP_LS,
    OP_CP,
    OP_CP_TO_REMOTE,
    OP_CP_FROM_REMOTE,
    OP_RM,
    OP_MV,
    OP_MKDIR,
    OP_CAT
} FS_OPERATION;


// Their actual string representations
extern const char* operations_names[N_FS_OPERATIONS];

// Number of arguments that should be passed to any of these operations on client side
extern const char n_operations_args[N_FS_OPERATIONS];

// Number of arguments that should be sent via the network. It is the same as number of
// operation arguments except for `cp` variations
extern const char n_request_args[N_FS_OPERATIONS];


/**
 * Valid file and directory paths in the remote file system are must follow these criteria:
 * - max length of 64 symbols
 * - given token := [0-9a-zA-Z\(\)_- ]+, directory paths must conform to regex $/(token/)*^,
 *      and file paths to regex $/token(/token)*^
 */
int validate_fs_file_path(const char* path);
int validate_fs_directory_path(const char* path);


typedef int socket_t;


typedef struct web_request_t {
    FS_OPERATION operation_code;
    char n_args;
    char** args;
} web_request_t;


typedef enum response_status_t {
    RESPONSE_OK,
    RESPONSE_INVALID_REQUEST,
    RESPONSE_SERVER_ERROR
} response_status_t;


typedef struct web_response_t {
    response_status_t status;
    unsigned len;
    char* buff;
} web_response_t;

int _read(int fd, char* buf, size_t len);

void request_destroy(web_request_t* request);
void response_destroy(web_response_t* response);
