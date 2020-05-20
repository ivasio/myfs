#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#include <myfs.h>

const char* operations_names[N_FS_OPERATIONS] = {
    "cd",
    "ls",
    "cp",
    "cp-upload",
    "cp-download",
    "rm",
    "mv",
    "mkdir",
    "cat"
};

const char n_operations_args[N_FS_OPERATIONS] = {
    1, // cd takes the directory path to navigate to
    1, // ls takes the path to display info about
    2, // all cp variations take the destination and source paths
    2,
    2,
    1, // rm takes a path to be deleted
    2, // mv takes the destination and source paths
    1, // mkdir takes the path of directory to be created
    1, // cat takes the path of file to be shown
};

const char n_request_args[N_FS_OPERATIONS] = {
    1,
    1,
    2, // remote cp sends the destination and source paths on the server
    2, // cp to remote sends destination path on the server and the content of file to be copied
    1, // cp from remote sends source path
    1,
    2,
    1,
    1,
};

int validate_path_part(const char* path_part, char length) {
    const char* allowed_special_chars = "()_- ";
    for (char i = 0; i < length; i++) {
        if (!isalnum(path_part[i]) || strchr(allowed_special_chars, path_part[i]) == NULL) {
            return -1;
        }
    }
    return 0;
}

int validate_fs_file_path(const char* path) {
    return 0;
}

int validate_fs_directory_path(const char* path) {
    return 0;
}


int _read(int fd, char* buf, size_t len) {
    size_t len_left = len;
    while (len_left != 0) {
        int read_res = read(fd, buf, len_left);
        if (read_res < 0) {
            return read_res;
        } else {
            len_left -= read_res;
            buf += read_res;
        }
    }
    return len;
}


void response_destroy(web_response_t* response) {
    free(response->buff);
    free(response);
}


void request_destroy(web_request_t* request) {
    int n_components = n_request_args[request->operation_code];
    for (int i = 0; i < n_components; i++)
        free(request->args[i]);

    free(request);
}