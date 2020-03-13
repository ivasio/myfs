#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// Usual socket headers
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <web.h>
#include <server.h>


int main(void)
{
    int serverSocket = setup_server();
    return run_server(serverSocket);
}

int setup_server() {
    socket_t serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8001);
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    int listening = listen(serverSocket, 10);
    if (listening < 0) {
        printf("Error: The server is not listening.\n");
        return 1;
    }

    printf("Started server on port %d", 8001);

    return serverSocket;
}

int run_server(socket_t server_socket) {
    int serve_res;
    while(1) {
        serve_res = serve(server_socket);
        if (serve_res != 0)
            return serve_res;
    }
}


#define BUFF_SIZE 128

int serve(socket_t server_socket) {
    socket_t client_socket = accept(server_socket, NULL, NULL);
    char recv_buff[BUFF_SIZE];

    int read_res = read(client_socket, recv_buff, sizeof(recv_buff) - 1);
    if (read_res != 0) {
        // todo
    }

    web_request_t* request = parse_request(recv_buff);
    web_response_t* response;

    if (request != NULL) {
        printf("Got request for method id %d\n", request->operation_code);
        response = get_response(request);
    } else {
        response = (web_response_t*)calloc(1, sizeof(web_response_t));
        response->status = RESPONSE_INVALID_REQUEST;
    }

    char resp_code = (char)(response->status);
    write(client_socket, &resp_code, 1);

    if (response->status == RESPONSE_OK) {
        char resp_len = (char)(response->len);
        write(client_socket, &resp_len, 1);
        write(client_socket, response->buff, response->len);
    }

    // closing connection
    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);

    // freeing used memory
    // response_destroy(response);
    request_destroy(request);

    return 0;
}

web_request_t* parse_request(const char* request_buff) {
    char operation_code = request_buff[0];
    if (operation_code >= N_FS_OPERATIONS)
        return NULL;

    web_request_t* request = (web_request_t*)calloc(1, sizeof(web_request_t));
    request->operation_code = operation_code;
    return request;
}

web_response_t* get_response(const web_request_t* request) {
    web_response_t* response = (web_response_t *) calloc(1, sizeof(web_response_t));
    if (request == NULL) {
        response->status = RESPONSE_INVALID_REQUEST;
    } else {
        response->status = RESPONSE_OK;
        response->len = 1;
        response->buff = (char*) calloc(2, sizeof(char));
        sprintf(response->buff, "%d", request->operation_code);
    }
    return response;
}



