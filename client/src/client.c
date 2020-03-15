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
    if(argc != 3)
    {
        printf("\n Usage: %s METHOD_NUMBER ARGS_STRING\n", argv[0]);
        return 1;
    }

    int method_id = atoi(argv[1]);
    web_request_t request;

    int request_init_res = initialize_request(&request, method_id, argv[2]);
    if (request_init_res != 0) {
        printf("Request initialization failed with code %d\n", request_init_res);
        return 1;
    }

    socket_t client_socket;
    int request_send_res = send_request(&request, &client_socket);
    if (request_send_res != 0) {
        printf("Request sending failed with code %d\n", request_send_res);
        return 1;
    }

    web_response_t response;
    int response_read_res = read_response(client_socket, &response);
    if (response_read_res != 0) {
        printf("Response reading failed with code %d\n", response_read_res);
        return 1;
    }

    printf("Server responded with message :\n%s", response.buff);

    return 0;
}

int send_request(web_request_t* request, socket_t* client_socket) {
    *client_socket = socket(AF_INET, SOCK_STREAM, 0);
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

    int connectRes = connect(*client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(connectRes < 0) {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    unsigned int buf_len = 0;
    char* send_buffer = serialize_request(request, &buf_len);
    write(*client_socket, send_buffer, buf_len);
    return 0;
}

int initialize_request(web_request_t* request, int method_id, char *data) {
    if (method_id <= 0 || method_id >= N_FS_OPERATIONS) {
        return -1;
    }

    request->operation_code = method_id;
    request->n_args = 3;
    request->args = (char**) calloc(request->n_args, sizeof(char*));
    for (int i = 0; i < request->n_args; i++) {
        request->args[i] = data;
    }

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

int read_response(socket_t client_socket, web_response_t* response) {
    char receive_buffer[2] = {0};

    // reading response status and length
    read(client_socket, receive_buffer, 2);
    response->status = receive_buffer[0];
    if (response->status != RESPONSE_OK) {
        return response->status;
    }

    response->len = receive_buffer[1];
    response->buff = (char *) calloc(response->len + 1, sizeof(char));

    // read the message
    read(client_socket, response->buff, response->len);

    // read the rest, todo improve
    while (1)
    {
        int readRes = read(client_socket, receive_buffer, 1);
        if (readRes <= 0) {
            break;
        }

        receive_buffer[readRes] = 0;
        if(fputs(receive_buffer, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    }

    return 0;
}