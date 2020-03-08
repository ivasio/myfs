#include <stdlib.h>

#include <myfs.h>
#include <web.h>


void response_destroy(web_response_t* response) {
    free(response->response_buff);
    free(response);
}

void request_destroy(web_request_t* request) {
    int n_components = get_n_operation_args(request->operation_code);
    for (int i = 0; i < n_components; i++)
        free(request->args[i]);

    free(request);
}
