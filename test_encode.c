/*
Name - Sujith.S
Start Date - 22/04/2026
End Date - 27/04/2026
Discription - This project implements **LSB (Least Significant Bit) steganography** — a technique for concealing secret data inside a 
BMP image file without producing any visible change to the image. Every pixel in a 24-bit BMP is stored as three bytes representing its 
red, green, and blue channel values. By replacing the least significant bit of each byte with one bit of the secret message, the pixel 
color shifts by at most one unit out of 255 — a difference completely invisible to the human eye. Because no pixel is added or removed 
and the file dimensions stay identical, the stego image is indistinguishable from the original when viewed normally, making it an effective 
tool for covert communication.
Beyond the core bit-manipulation technique, the program embeds structured metadata alongside the secret payload so that decoding is self-
contained and reliable. Before any secret data is written, a magic string (`#*`) is encoded into the image as a fingerprint, allowing the
decoder to immediately verify whether a given BMP file actually contains hidden data. Following the magic string, the extension of the 
secret file, its byte length, and finally the secret content itself are each encoded in sequence. This layered encoding scheme means that a 
decoder reading the stego image can reconstruct the original file completely — correct file type, correct size, and correct content — without 
needing any external metadata file or prior knowledge of what was hidden.
*/

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])  //command line argument.
{

    if(check_operation_type(argv) == e_encode) //check wheather -e entered.
    {
        printf("You have choosed encoding\n");

        EncodeInfo encInfo;   // declare structure as encInfo
        
        if(read_and_validate_encode_args(argv, &encInfo) == e_success) //check in encode.c
        {
            printf("Read and validate is successful\n");
        }
        else
        {
            printf("Read and validate is unsuccessful\n");
            return e_failure;
        }
        if(do_encoding(&encInfo) == e_success)  //check in encode.c
        {
            printf("Encoding successfull\n");
        }
        else
        {
            printf("Encoding unsuccessfully\n");
            return e_failure;
        }
        return e_success;
    }
    else if(check_operation_type(argv) == e_decode)
    {
        printf("You have choosed decoding\n");

        DecodeInfo deInfo;
        
        if(read_and_validate_decode_args(argv, &deInfo) == e_success)
        {
            printf("Read and validate is successful\n");
        }
        else
        {
            printf("Read and validate is unsuccessful\n");
            return e_failure;
        }
        if(do_decoding(&deInfo) == e_success)
        {
            printf("Decoding successfull\n");
        }
        else
        {
            printf("Decoding unsuccessfully\n");
            return e_failure;
        }

        return e_success;
    }
    else
    {
        printf("Invalid expression\n");
        printf("for encoding - > ./a.out -e beutiful.bmp secret.txt [stego.bmp]\n");
        printf("for decoding - > ./a.out -d stego.bmp [output.txt]\n");
        return e_failure;
    }
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if(strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}