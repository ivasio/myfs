#pragma once


typedef struct str_array {
    char** strings;
    int len;
} str_array;


void str_array_destroy(str_array* array);
