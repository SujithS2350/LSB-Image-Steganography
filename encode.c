#include <stdio.h>
#include <string.h>
#include "types.h"
#include "encode.h"

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo) //check the files
{
    if(strstr(argv[2], ".bmp") != NULL)    //does the [2] index argument extention is .bmp.
    {
        printf(".bmp found\n");
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf(".bmp not found\n");
        return e_failure;
    }
    if(strstr(argv[3], ".txt") != NULL)   //does the [3] index argument extention is .txt.
    {
        printf(".txt found\n");
        encInfo->secret_fname = argv[3];
    }
    else
    {
        printf(".txt not found\n");
        return e_failure;
    }
    if(strstr(argv[4], ".bmp") != NULL)   //does the [4] index argument extention is .bmp.
    {
        printf("stego.bmp found\n");
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp"; 
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)   //encoding process
{
    printf("<---------Encoding started---------->\n");
    
    if(open_files(encInfo) == e_success)   //to open all the files.
    {
        printf("All files opened sunccessfully\n");
    }
    else
    {
        printf("Files not opened successfully\n");
        return e_failure;
    }

    printf("Enter magic string:\n");     //magic string #* = 16B.
    scanf(" %[^\n]", encInfo->magic);    //structure pointer
    
    if(check_capacity(encInfo) == e_success)  //check size capacity of all the files.
    {
        printf("Check capacity is successful\n");
    }
    else
    {
        printf("Check capacity is unsuccessful\n");
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("Header bytes copied successfully\n");
    }
    else
    {
        printf("Header bytes not copied\n");
        return e_failure;
    }

    if(encode_magic_string(encInfo->magic,encInfo) == e_success)
    {
        printf("Magic string is encoded successfully\n");
    }
    else
    {
        printf("Magic did not happen\n");
        return e_failure;
    }


    if(encode_secret_file_extn_size(strlen(strchr(encInfo->secret_fname, '.')), encInfo) == e_success)
    {
        printf("Secret file extention size encoded successfully\n");
    }
    else
    {
        printf("Secret file extention size not encoded successfully\n");
        return e_failure;
    }

    if(encode_secret_file_extn(strchr(encInfo->secret_fname,'.'), encInfo) == e_success)
    {
        printf("Secret file extensiom endoded successfully\n");
    }
    else
    {
        printf("Secret file extension not endoded successfully\n");
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("Secret file size encoded successfully\n");
    }
    else
    {
        printf("Secret file size not encoded successfully\n");
        return e_failure;
    }
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret file data encoded successfully\n");
    }
    else
    {
        printf("Secret file data not encoded successfully\n");
        return e_failure;
    }
    
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo) == e_success)
    {
        printf("Remaining data copied successfully\n");
    }
    else
    {
        printf("Remaining data not copied successfully\n");
        return e_failure;
    }

    return e_success;
}


Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");

    if(encInfo->fptr_src_image == NULL)
    {
        printf("Source file is not present\n");
        return e_failure;
    }


    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if(encInfo->fptr_secret ==  NULL)
    {
        printf("Source file is not present\n");
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");



    return e_success;
}


Status check_capacity(EncodeInfo *encInfo) 
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); //get size of beautuful.bmp file.
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);  //get size of secret.txt file

    if(encInfo->image_capacity > ((strlen(encInfo->magic) * 8) + 32 + 32 + 32 + encInfo->size_secret_file * 8))
        return e_success; //magic string + size of .bmp + size of .txt + size of .bmp + secret file.
    else
        return e_failure;
}

uint get_image_size_for_bmp(FILE *fptr_image)  //to get .bmp size
{
    int width, length;

    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, 4, 1, fptr_image);
    fread(&length, 4, 1, fptr_image);

    return length * width * 3;
}

uint get_file_size(FILE *fptr)  //to get secret file size.
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image) // all remaining contents from .bmp to stego.
{
    rewind(fptr_src_image);

    char buffer[54];

    fread(buffer, 54, 1, fptr_src_image);

    fwrite(buffer, 54, 1, fptr_dest_image);

    return e_success;

}


Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    return encode_data_to_image(magic_string, strlen(magic_string), encInfo);
}


Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i = 0; i < size; i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(data[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
   
}


Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i = 7; i >= 0; i--)
    {
        image_buffer[7 - i] = ((data & (1 << i)) >> i | (image_buffer[7 - i] & 0xFE));
    }

    return e_success;
}


Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    return encode_int_to_image(size, encInfo);
}

Status encode_int_to_image(int size, EncodeInfo *encInfo)
{
    char buffer[32];

    fread(buffer, 32, 1, encInfo->fptr_src_image);
    
    for(int i = 31; i >= 0; i--)
    {
        buffer[31 - i] = (((size & (1 << i)) >> i) | buffer[31 - i] & 0xFE);
    }
    
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}


Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    return encode_data_to_image(file_extn, strlen(file_extn), encInfo);
}


Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    return encode_int_to_image(file_size, encInfo);
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    for(int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(encInfo->secret_data, 1, 1, encInfo->fptr_secret);
        encode_data_to_image(encInfo->secret_data, 1, encInfo);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest, EncodeInfo *encInfo)
{
    while(ftell(fptr_src) != encInfo->image_capacity)
    {
        fread(encInfo->image_data, 1, 1, fptr_src);
        fwrite(encInfo->image_data, 1, 1, fptr_dest);
    }
    return e_success;
}