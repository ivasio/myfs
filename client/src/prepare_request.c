#include <client.h>
#include <web.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int initialize_request(int n_args, char *operation, char **args, web_request_t *request) {
    int operation_code = -1;
    for (int i = 0; i < N_FS_OPERATIONS; i++) {
        if (strcmp(operation, operations_names[i]) == 0) {
            operation_code = i;
            break;
        }
    }
    if (operation_code == -1) {
        // Provided operation_name doesn't match any of the supported ones
        return -1;
    }

    if (n_args != n_operations_args[operation_code]) {
        // Wrong number of arguments was provided
        return -2;
    }

    request->operation_code = operation_code;
    request->n_args = (char)n_args;
    request->args = args;

    return 0;
}

char *serialize_request(struct web_request_t *request, unsigned int *buffer_len) {
    unsigned long sum_args_len = 0;
    for (int i = 0; i < request->n_args; i++) {
        sum_args_len += strlen(request->args[i]) + 1;
    }

    *buffer_len = sum_args_len + 2;
    char* request_buf = (char*) calloc(*buffer_len, sizeof(char));

    request_buf[0] = request->operation_code;
    request_buf[1] = sum_args_len;

    char* buf_pos = &request_buf[2];
    for (int i = 0; i < request->n_args; i++) {
        strcpy(buf_pos, request->args[i]);
        buf_pos += strlen(buf_pos) + 1;
    }

    return request_buf;
}