#pragma once

int parse_request(web_request_t* request, char* receive_buffer, char buf_len);
int process_request(web_request_t *request);
