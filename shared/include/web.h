#ifndef MYFS_H
#define MYFS_H

#include <myfs.h>

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


void request_destroy(web_request_t* request);
void response_destroy(web_response_t* response);


#endif //MYFS_H
