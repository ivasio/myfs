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

    printf("Started server on port %d\n\n", 8001);

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

int serve(socket_t server_socket) {
    socket_t client_socket = accept(server_socket, NULL, NULL);

    web_request_t request;
    read_request(&request, client_socket);

    printf("Got request for method id %d with %d args\n", request.operation_code, request.n_args);
    for (int i = 0; i < request.n_args; i++) {
        printf("%d : %s, ", i, request.args[i]);
    }
    printf("\n");

    web_response_t response;
    /* in case of invalid request
    {
    response->status = RESPONSE_INVALID_REQUEST;
    }
    */
    construct_response(&request, &response);
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

void read_request(web_request_t* request, socket_t client_socket) {
    char recv_buff[2];

    int read_res = read(client_socket, recv_buff, 2);
    if (read_res != 0) {
        // todo
    }

    request->operation_code = recv_buff[0];
    char msg_len = recv_buff[1];

    char* msg_buf = (char*) calloc(msg_len + 1, sizeof(char));
    read(client_socket, msg_buf, msg_len);
    parse_request(request, msg_buf, msg_len);
}


#define MAX_COMPONENTS_IN_REQUEST 8  // todo get it dynamically by method id
int parse_request(web_request_t* request, char* receive_buffer, char buf_len) {
    // check if parameters are valid for this type of request
    if (request->operation_code >= N_FS_OPERATIONS)
        return 1;

    char* buff_pointer = receive_buffer;
    request->args = (char**) calloc(MAX_COMPONENTS_IN_REQUEST, sizeof(char*));

    char n_args = 0;
    for(; (buff_pointer < receive_buffer + buf_len) && n_args < MAX_COMPONENTS_IN_REQUEST; n_args++) {
        request->args[n_args] = buff_pointer;
        buff_pointer += strlen(buff_pointer) + 1;  // moving to next 0-separated string
    }
    request->n_args = n_args;

    return 0;
}


void construct_response(web_request_t* request, web_response_t* response) {
    if (request == NULL) {
        response->status = RESPONSE_INVALID_REQUEST;
    } else {
        response->status = RESPONSE_OK;
        response->len = 1;
        response->buff = (char*) calloc(2, sizeof(char));
        sprintf(response->buff, "%d", request->operation_code);
    }
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
