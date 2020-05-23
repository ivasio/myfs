#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <myfs.h>
#include <filesystem.h>


int main() {
    disc_t disc;
    fs_t fs;
    fs.disc = &disc;

    int setup_res = setup_fs(&fs);
    if (setup_res < 0) {
        perror("Falied to initialize file system");
        return 1;
    }

    char* pipe_path = "/tmp/ivasio-fs-fifo";
    run_fs(&fs, pipe_path);
    finalize_fs(&fs);
}


void run_fs(fs_t *fs, char *pipe_path) {
    int serve_res;
    while(1) {
        serve_res = serve(fs, pipe_path);
        if (serve_res != 0)
            return;
    }
}


int serve(fs_t *fs, char *pipe_path) {
    web_request_t request;
    read_request(&request, pipe_path);

    web_response_t response;
    process_request(fs, &response, &request);

    send_response(&response, pipe_path);

    return 0;
}


int process_request(fs_t *fs, web_response_t *response, web_request_t *request) {
    printf("Got request for method %s with %d args\n", operations_names[request->operation_code],
        request->n_args);
    for (int i = 0; i < request->n_args; i++) {
        printf("%d : %s, ", i, request->args[i]);
    }
    printf("\n");

    fs->operations[request->operation_code](fs, request, response);
    printf("\n");
    return 0;
}


int read_request(web_request_t *request, char *pipe_path) {
    int pipe = open(pipe_path, O_RDONLY);

    char code_len[2];
    _read(pipe, code_len, 2);
    char* buff = (char*) calloc(code_len[1], sizeof(char));
    _read(pipe, buff, code_len[1]);

    parse_request(request, code_len[0], code_len[1], buff);

    return 0;
}


int parse_request(web_request_t *request, char op_code, char buf_len, char *receive_buffer) {
    // check if parameters are valid for this type of request
    if (op_code >= N_FS_OPERATIONS) {
        return -1;
    }
    request->operation_code = op_code;

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


int send_response(web_response_t* response, char* pipe_path) {
    int pipe = open(pipe_path, O_WRONLY);

    char resp_code = (char)(response->status);
    write(pipe, &resp_code, 1);

    char resp_len = (char)(response->len);
    write(pipe, &resp_len, 1);
    write(pipe, response->buff, response->len);

    close(pipe);
    free(response->buff);
    return 0;
}
