#ifndef CLIENT_SERVER_H
#define CLIENT_SERVER_H


int setup_server();
int run_server(int server_socket);


int serve(socket_t server_socket);
web_request_t* parse_request(const char* request);
web_response_t* get_response(const web_request_t* request);


#endif //CLIENT_SERVER_H
