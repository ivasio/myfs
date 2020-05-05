#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <myfs.h>

#include <filesystem.h>


int process_request(web_request_t *request) {
    printf("Got request for method id %d with %d args\n", request->operation_code, request->n_args);
    for (int i = 0; i < request->n_args; i++) {
        printf("%d : %s, ", i, request->args[i]);
    }
    printf("\n");

    return 0;
}

int parse_request(web_request_t* request, char* receive_buffer, char buf_len) {
    // check if parameters are valid for this type of request
    if (request->operation_code >= N_FS_OPERATIONS)
        return -1;

    int n_args_total = n_request_args[request->operation_code];
    char* buff_pointer = receive_buffer;
    request->args = (char**) calloc(n_args_total, sizeof(char*));

    char n_args = 0;
    for(; buff_pointer < receive_buffer + buf_len && n_args < n_args_total; n_args++) {
        request->args[n_args] = buff_pointer;
        buff_pointer += strlen(buff_pointer) + 1;  // moving to next 0-separated string
    }
    request->n_args = n_args;

    return 0;
}