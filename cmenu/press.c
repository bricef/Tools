#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "lz4frame.h"


#include "press.h"

bool press_is_error(int error_code){
    return error_code != PRESS_ERROR_NONE;
}

const char* press_get_error_name(int error_code){
    switch (error_code){
        case PRESS_ERROR_NONE:
            return "No error";
        case PRESS_ERROR_CAPACITY_INSUFFICIENT:
            return "Capacity insufficient";
        case PRESS_ERROR_INPUT_FORMAT_INVALID:
            return "Input format invalid";
        case PRESS_ERROR_ARGUMENT_INVALID:
            return "Argument invalid";
        case PRESS_ERROR_INTERNAL:
            return "Internal error";
        default:
            return "Unknown error";
    }
}

LZ4F_preferences_t preferences = LZ4F_INIT_PREFERENCES;

int press_compress_data(
    const unsigned char* src,
    int src_size, 
    char* dst,
    int dst_capacity
){
    if (src == NULL || dst == NULL){
        return PRESS_ERROR_ARGUMENT_INVALID;
    }

    if (src_size <= 0 || dst_capacity <= 0){
        return PRESS_ERROR_ARGUMENT_INVALID;
    }

    if (src_size > dst_capacity){
        return PRESS_ERROR_CAPACITY_INSUFFICIENT;
    }

    int compressed_size = LZ4F_compressFrame(
        dst, 
        dst_capacity,
        src, 
        src_size,
        &preferences
    );

    if (compressed_size <= 0){
        return PRESS_ERROR_INPUT_FORMAT_INVALID;
    }

    return PRESS_ERROR_NONE;
}

int press_decompress_data(
    const unsigned char* src,
    int src_size, 
    char* dst,
    int dst_capacity
){
    if (src == NULL || dst == NULL){
        return PRESS_ERROR_ARGUMENT_INVALID;
    }

    if (src_size <= 0 || dst_capacity <= 0){
        return PRESS_ERROR_ARGUMENT_INVALID;
    }

    if (src_size > dst_capacity){
        return PRESS_ERROR_CAPACITY_INSUFFICIENT;
    }

    LZ4F_dctx* dctx;
    size_t const dctxStatus = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
    if (LZ4F_isError(dctxStatus)) {
        printf("LZ4F_dctx creation error: %s\n", LZ4F_getErrorName(dctxStatus));
        exit(1);
    }
    
    LZ4F_decompressOptions_t options = {
        .stableDst = 0,
        .skipChecksums = 0,
        .reserved1 = 0,
        .reserved0 = 0,
    };

    int decompressed_size = LZ4F_decompress(
        dctx,
        dst,
        (size_t*) &dst_capacity,
        src,
        (size_t*) &src_size,
        &options
    );

    if (LZ4F_isError(decompressed_size)){
        printf("LZ4F_decompress error: %s\n", LZ4F_getErrorName(decompressed_size));
        return PRESS_ERROR_INPUT_FORMAT_INVALID;
    }

    int code = LZ4F_freeDecompressionContext(dctx);
    if(LZ4F_isError(code)){
        return PRESS_ERROR_INTERNAL;
    }

    return PRESS_ERROR_NONE;
}

