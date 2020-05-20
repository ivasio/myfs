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

#include <myfs.h>
#include <client.h>

int main(int argc, char *argv[])
{
    if(argc <= 2)
    {
        printf("\n Usage: %s OPERATION OPERATION_ARGUMENT_1 [, OPERATION_ARGUMENT_2, ...]\n", argv[0]);
        return 1;
    }
    printf("Requesting %s\n", argv[1]);

    web_request_t request;
    int request_init_res = initialize_request(argc - 2, argv[1], argv + 2, &request);
    if (request_init_res != 0) {
        printf("Request initialization failed with code %d\n", request_init_res);
        return 1;
    }

    socket_t client_socket;
    int request_send_res = send_request(&request, &client_socket);
    if (request_send_res != 0) {
        perror("Request sending failed");
        return 1;
    }

    web_response_t response;
    int response_read_res = read_response(client_socket, &response);
    if (response_read_res != 0) {
        perror("Response reading failed");
        return 1;
    }

    printf("Server responded with message :\t");
    for (int i = 0; i < response.len; i++) {
        printf("%c", response.buff[i]);
    }
    printf("\n\n");

    return 0;
}

int send_request(web_request_t* request, socket_t* client_socket) {
    *client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket < 0)
    {
        perror("Error : Could not create socket");
        return 1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8006);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("inet_pton error occured");
        return 1;
    }

    int connectRes = connect(*client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(connectRes < 0) {
        perror("Connect failed");
        return 1;
    }

    unsigned int buf_len = 0;
    char* send_buffer = serialize_request(request, &buf_len);
    write(*client_socket, send_buffer, buf_len);
    return 0;
}

int read_response(socket_t client_socket, web_response_t* response) {
    char receive_buffer[2] = {0};

    // reading response status and length
    _read(client_socket, receive_buffer, 2);
    response->status = receive_buffer[0];
    if (response->status != RESPONSE_OK) {
        return response->status;
    }

    response->len = receive_buffer[1];
    response->buff = (char *) calloc(response->len + 1, sizeof(char));

    // read the message
    _read(client_socket, response->buff, response->len);

    // read the rest, todo improve
//    while (1)
//    {
//        int readRes = _read(client_socket, receive_buffer, 1);
//        if (readRes <= 0) {
//            perror("Failed to read the residing symbol");
//            break;
//        }
//
//        receive_buffer[readRes] = 0;
//        if(fputs(receive_buffer, stdout) == EOF)
//        {
//            printf("\n Error : Fputs error\n");
//        }
//    }

    return 0;
}