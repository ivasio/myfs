#include <stdlib.h>

#include <data_structures.h>


void str_array_destroy(str_array* array) {
    free(array->strings);
}