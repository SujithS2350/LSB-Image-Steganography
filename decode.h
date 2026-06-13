#ifndef DECODE_H
#define DECODE_H

#include "types.h"


typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    
    char magic_data[10];
    /* Destination File Info */
    char *dest_fname;
    FILE *fptr_dest;

    uint secret_file_extn_size;
    uint secret_file_size;

    char secret_file_extn[10];
} DecodeInfo;


Status read_and_validate_decode_args(char *argv[], DecodeInfo *deInfo);

Status do_decoding(DecodeInfo *deInfo);

Status open_de_files(DecodeInfo *deInfo);

Status check_magic(DecodeInfo *deInfo);

Status decode_extension_size(DecodeInfo *deInfo);

Status decode_magic(int, DecodeInfo *deInfo);

Status decode_secret_data_size(DecodeInfo *deInfo);

Status decode_extension(DecodeInfo *deInfo);

Status decode_secret_data(DecodeInfo *deInfo);

Status store_byte_in_file(char *, DecodeInfo *deInfo);

uint decode_integer(DecodeInfo *deInfo);

#endif
debugging at the rate of int 