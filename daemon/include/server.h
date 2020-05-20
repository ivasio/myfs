#pragma once

void setup_pipe(char* pipe_path);
int setup_server(int server_port);
int run_server(socket_t server_socket, char *pipe_path);
int serve(socket_t server_socket, char *pipe_path);

void resend_request(char *pipe_path, char op_code, char len, char *buf);
void read_request(web_request_t *request, socket_t client_socket, char *pipe_path);

void construct_response(web_response_t *response, char *pipe_path);
int send_response(web_response_t *response, socket_t client_socket);
