#pragma once
#include <web.h>


/**
 * Initializes web request for the operation_name using given arguments
 * @param request : pointer to the request object
 * @param operation_name : name of the operation to be performed, e.g. ls, cat
 * @param n_args : number of actual operation arguments
 * @param args : operation arguments
 * @return
 */
int initialize_request(int n_args, char *operation_name, char **args, web_request_t *request);

char *serialize_request(struct web_request_t *request, unsigned int *buffer_len);

/**
 * Sends the request for remote FS to perform the operation
 * @param request : request to be performed
 * @param client_socket : client socket connected to the remote server
 * @return
 */
int send_request(web_request_t* request, socket_t* client_socket);

int read_response(socket_t client_socket, web_response_t* response);
