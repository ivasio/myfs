#include <web.h>

int send_request(web_request_t* request);
web_request_t* construct_request(int method_id);
char* serialize_request(struct web_request_t* request);
web_response_t* read_response(socket_t client_socket);
