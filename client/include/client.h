#include <web.h>

int send_request(web_request_t* request, socket_t* client_socket);
int initialize_request(web_request_t* request, int method_id, char *data);
char* serialize_request(struct web_request_t* request);
int read_response(socket_t client_socket, web_response_t* response);
