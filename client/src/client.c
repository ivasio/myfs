#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include <web.h>
#include <client.h>


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("\n Usage: %s METHOD_NUMBER \n", argv[0]);
        return 1;
    }

    int method_id = atoi(argv[1]);
    web_request_t* request = construct_request(method_id);
    send_request(request);

    return 0;
}

int send_request(web_request_t* request) {
    socket_t client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8001);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    int connectRes = connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(connectRes < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    char* send_buffer = serialize_request(request);
    write(client_socket, send_buffer, strlen(send_buffer));

    web_response_t* response = read_response(client_socket);
    printf("Request method id was %s", response->response_buff);

    // read the rest
    char recvBuff[64] = {0};
    while (1)
    {
        int readRes = read(client_socket, recvBuff, sizeof(recvBuff) - 1);
        if (readRes <= 0) {
            break;
        }

        recvBuff[readRes] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    }

    return 0;
}

web_request_t* construct_request(int method_id) {
    if (method_id <= 0 || method_id >= N_FS_OPERATIONS) {
        return NULL;
    }
    web_request_t* request = (web_request_t*) calloc(1, sizeof(web_request_t));
    request->operation_code = method_id;
    request->n_args = 0;
    return request;
}

char* serialize_request(struct web_request_t* request) {
    /*
    int sum_args_len = 0;
    for (int i = 0; i < request->n_args; i++) {
        //
    }
    */
    char* request_buf = (char*) calloc(2, sizeof(char));
    request_buf[0] = request->operation_code;
    request_buf[1] = request->n_args;

    return request_buf;
}

web_response_t* read_response(socket_t client_socket) {
    char receive_buffer[2] = {0};
    read(client_socket, receive_buffer, 2);
    web_response_t* response = (web_response_t*) calloc(1, sizeof(web_response_t));
    response->status = receive_buffer[0];
    if (response->status == RESPONSE_OK) {
        response->response_len = receive_buffer[1];
        response->response_buff = (char *) calloc(response->response_len, sizeof(char));
    }
    return response;
}