#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <web.h>
#include <server.h>


void resend_request(char *pipe_path, char *buf, char len);

int main(void)
{
    int server_port = 8005;
    int serverSocket = setup_server(server_port);

    char* fifo_path = "/tmp/ivasio-fs-fifo";
    setup_pipe(fifo_path);

    return run_server(serverSocket, fifo_path);
}

void setup_pipe(char* path) {
    // todo check if was created earlier
    mkfifo(path, 0666);
}

int setup_server(int server_port) {
    socket_t serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(server_port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    int listening = listen(serverSocket, 10);
    if (listening < 0) {
        printf("Error: The server is not listening.\n");
        return 1;
    }

    printf("Started server on port %d\n\n", server_port);

    return serverSocket;
}

int run_server(socket_t server_socket, char *pipe_path) {
    int serve_res;
    while(1) {
        serve_res = serve(server_socket, pipe_path);
        if (serve_res != 0)
            return serve_res;
    }
}


int serve(socket_t server_socket, char *pipe_path) {
    socket_t client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        printf("Error in accept!\n");
        return 1;
    }

    printf("New connection from client\n");

    web_request_t request;
    read_request(&request, client_socket, pipe_path);

    web_response_t response;
    construct_response(&response, pipe_path);
    send_response(&response, client_socket);

    // closing connection
    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);

    printf("Finished processing the request\n\n");
    // freeing used memory
    // response_destroy(response);
    // request_destroy(request);

    return 0;
}

void read_request(web_request_t *request, socket_t client_socket, char *pipe_path) {
    printf("Started reading request\n");
    char recv_buff[2];

    int read_res = read(client_socket, recv_buff, 2);
    if (read_res != 0) {
        // todo
    }

    request->operation_code = recv_buff[0];
    char msg_len = recv_buff[1];

    char* msg_buf = (char*) calloc(msg_len + 1, sizeof(char));
    read(client_socket, msg_buf, msg_len);
    resend_request(pipe_path, msg_buf, msg_len);
    free(msg_buf);
}

void resend_request(char *pipe_path, char *buf, char len) {
    int pipe = open(pipe_path, O_WRONLY);

    write(pipe, &len, 1);
    write(pipe, buf, len);

    close(pipe);
}


int process_request(web_request_t *request, char *pipe_path) {
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


void construct_response(web_response_t *response, char *pipe_path) {
    int pipe = open(pipe_path, O_RDONLY);

    char status_len[2];
    read(pipe, status_len, 2);
    response->status = status_len[0];
    response->len = status_len[1];

    response->buff = (char*) calloc(status_len[1], sizeof(char));
    read(pipe, response->buff, response->len);

    close(pipe);
}


int send_response(web_response_t *response, socket_t client_socket) {
    char resp_code = (char)(response->status);
    write(client_socket, &resp_code, 1);

    char resp_len;
    if (response->status == RESPONSE_OK) {
        resp_len = (char)(response->len);
        write(client_socket, &resp_len, 1);
        write(client_socket, response->buff, response->len);
    } else {
        resp_len = 0;
        write(client_socket, &resp_len, 1);
    }

    return 0;
}
