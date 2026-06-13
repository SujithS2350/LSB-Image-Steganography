#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "decode.h"


Status read_and_validate_decode_args(char *argv[], DecodeInfo *deInfo)
{
     if(strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp found\n");
        deInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf(".bmp not found\n");
        return e_failure;
    }
    if(strstr(argv[3], ".txt") != NULL)
    {
        printf(".txt found\n");
        deInfo->dest_fname = argv[3];
    }
    else
    {
        deInfo->dest_fname = "output.txt";
    }
   

    return e_success;
}

Status do_decoding(DecodeInfo *deInfo)
{
    printf("<------Decoding started------>\n");

    if(open_de_files(deInfo) == e_success)
    {
        printf("All files opened sunccessfully\n");
    }
    else
    {
        printf("Files not opened successfully\n");
        return e_failure;
    }

    if(check_magic(deInfo) == e_success)
    {
        printf("Magic string found\n");
    }
    else
    {
        printf("Magic string not found\n");
        return e_failure;
    }
    if(decode_extension_size(deInfo) == e_success)
    {
        printf("Secrect file extension size decoded successfully\n");
    }
    else
    {
        printf("Secrect file extension size not decoded successfully\n");
        return e_failure;
    }

    if(decode_extension(deInfo) == e_success)
    {
        printf("Secret file extension decoded successfully\n");
    }
    else
    {
        printf("Secret file extension not decoded successfully\n");
        return e_failure;
    }
    if(decode_secret_data_size(deInfo) == e_success)
    {
        printf("Secret data size decoded successfully\n");
      
    }
    else
    {
        printf("Secret data size not decoded successfully\n");
        return e_failure;
    }
    if(decode_secret_data(deInfo) == e_success)
    {
        printf("Secret data decoded successfully\n");
    }
    else
    {
        printf("Secret data not decoded successfully\n");
        return e_failure;
    }
    return e_success;
}

Status open_de_files(DecodeInfo *deInfo)
{
    deInfo->fptr_stego_image = fopen(deInfo->stego_image_fname, "r");

    if(deInfo->stego_image_fname == NULL)
    {
        printf("Stego file is not present\n");
        return e_failure;
    }

    deInfo->fptr_dest = fopen(deInfo->dest_fname, "w");
   
    return e_success;
}

Status check_magic(DecodeInfo *deInfo)
{
    fseek(deInfo->fptr_stego_image, 54, SEEK_SET);
    decode_magic(2, deInfo);
    if(strstr(deInfo->magic_data, "#*") != NULL)
    {
        return e_success;
    }
    return e_failure;
}


Status decode_magic(int size, DecodeInfo *deInfo)
{
    char buffer[size * 8];
    fread(buffer, size * 8, 1, deInfo->fptr_stego_image);
    int j = 0;
    int shift = 7;
    for(int i = 0; i < size * 8; i++)
    {
        if(i % 8 == 0)
        {
            j++; 
            shift = 7;
        }
        deInfo->magic_data[j] = deInfo->magic_data[j] | (buffer[i] & 1) << shift;   
        shift--;       
    }
    return e_success;
}


Status decode_extension_size(DecodeInfo *deInfo)
{
    deInfo->secret_file_extn_size = decode_integer(deInfo);
    return e_success;
}


Status decode_extension(DecodeInfo *deInfo)
{
    char buffer[deInfo->secret_file_extn_size * 8];
    fread(buffer, deInfo->secret_file_extn_size * 8, 1, deInfo->fptr_stego_image);

    int j = 0;
    int shift = 7;
    for(int i = 0; i < deInfo->secret_file_extn_size * 8; i++)
    {
        if(i != 0 && i % 8 == 0)
        {
            j++; 
            shift = 7;
        }
        deInfo->secret_file_extn[j] = deInfo->secret_file_extn[j] | (buffer[i] & 1) << shift;   
        shift--;      
    }
    
    return e_success;
}

Status decode_secret_data_size(DecodeInfo *deInfo)
{
    
    deInfo->secret_file_size = decode_integer(deInfo);
    return e_success;
}




uint decode_integer(DecodeInfo *deInfo)
{
    char buffer[32];
    uint num = 0;
    fread(buffer, 32, 1, deInfo->fptr_stego_image);
    for(int i = 0; i < 32; i++)
    {
        num = num | (buffer[i] & 1) << (31 - i);
    }
    return num;
}

Status decode_secret_data(DecodeInfo *deInfo)
{
    char buffer[8];
    for(int i = 0; i < deInfo->secret_file_size; i++)
    {
        fread(buffer, 8, 1, deInfo->fptr_stego_image);
        store_byte_in_file(buffer, deInfo);
    }

    return e_success;
}

Status store_byte_in_file(char *buffer, DecodeInfo *deInfo)
{
    char ch = 0;
    for(int i = 0; i < 8; i++)
    {
        ch = ch | (buffer[i] & 1) << (7 - i);    
    }
    putc(ch, deInfo->fptr_dest);

    return e_success;
}