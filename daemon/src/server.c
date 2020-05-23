#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <myfs.h>
#include <server.h>


int main(void) {
    int server_port = 8006;
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

    int read_res = _read(client_socket, recv_buff, 2);
    if (read_res == -1) {
        perror("read_res != 0");
    }

    char op_code = recv_buff[0];
    char msg_len = recv_buff[1];

    char* msg_buf = (char*) calloc(msg_len + 1, sizeof(char));
    _read(client_socket, msg_buf, msg_len);
    resend_request(pipe_path, op_code, msg_len, msg_buf);
    free(msg_buf);
}


void resend_request(char *pipe_path, char op_code, char len, char *buf) {
    int pipe = open(pipe_path, O_WRONLY);

    write(pipe, &op_code, 1);
    write(pipe, &len, 1);
    write(pipe, buf, len);

    close(pipe);
}


void construct_response(web_response_t *response, char *pipe_path) {
    int pipe = open(pipe_path, O_RDONLY);

    char status_len[2];
    _read(pipe, status_len, 2);
    response->status = status_len[0];
    response->len = status_len[1];
    response->buff = (char*) calloc(status_len[1], sizeof(char));
    _read(pipe, response->buff, response->len);

    close(pipe);
}


int send_response(web_response_t *response, socket_t client_socket) {
    char resp_code = (char)(response->status);
    write(client_socket, &resp_code, 1);

    char resp_len;
    resp_len = (char)(response->len);
    write(client_socket, &resp_len, 1);
    write(client_socket, response->buff, response->len);

    return 0;
}
