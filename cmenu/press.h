#include <stdbool.h>

#ifndef PRESS_H
#define PRESS_H

int press_compress_data(
    const unsigned char* src,
    int src_size, 
    char* dst,
    int dst_capacity
);

int press_decompress_data(
    const unsigned char* src, 
    int src_size, 
    char* dst,
    int dst_capacity
);


enum press_error_code {
    PRESS_ERROR_NONE = 0,
    PRESS_ERROR_CAPACITY_INSUFFICIENT = 1,
    PRESS_ERROR_INPUT_FORMAT_INVALID = 2,
    PRESS_ERROR_ARGUMENT_INVALID = 3,
    PRESS_ERROR_INTERNAL = 4,
};

bool press_is_error(int error_code);

const char* press_get_error_name(int error_code);

#endif
