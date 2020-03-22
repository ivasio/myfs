#pragma once
int setup_server();
int run_server(int server_socket);


int serve(socket_t server_socket);

void read_request(web_request_t *request, socket_t client_socket);
void construct_response(web_request_t* request, web_response_t* response);

int send_response(web_response_t *response, socket_t client_socket);
int parse_request(web_request_t* request, char* receive_buffer, char buf_len);

