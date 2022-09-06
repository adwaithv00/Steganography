#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

Status_d open_files_d(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open stego file %s\n", decInfo->stego_image_fname);
    	return d_failure;
    }
    return d_success;
}

Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //get .bmp stego file
    if(argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo -> stego_image_fname = argv[2];
    }
    else
    {
        return d_failure;
    }

    if(argv[3] != NULL )
    {
        if(strcmp(strstr(argv[3], "."), ".txt") == 0  || strcmp(strstr(argv[3], "."), ".doc") == 0 ||strcmp(strstr(argv[3], "."), ".pdf") == 0  )
        {
            decInfo -> decode_text_fname = argv[3];
        }
        else
        {
            return d_failure;
        }
    }
    else
    {
        decInfo -> decode_text_fname = "decode_text.txt";
    }

    return d_success;
}

Status_d decode_lsb_to_byte(char* data, char *dec_data_buffer)
{
    *data=0;
    for(int i = 0; i < 8; i++)
    {
        *data = ( *data | ((dec_data_buffer[i] & 1) << (7-i)));
    }
    return d_success;
}

Status_d decode_image_to_data( int size, FILE *fptr_stego_image, char* dec_data_buffer, DecodeInfo *decInfo)
{
    char ch;
    for(int i=0; i<size; i++)
    {
        //read 8 bytes of beautiful.bmp
        fread(decInfo->image_data, 8,sizeof(char), fptr_stego_image);
        //decode the data
        decode_lsb_to_byte(&ch, decInfo->image_data);
        dec_data_buffer[i] = ch;
    }
    return d_success;
}

Status_d decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    //every encoding will call encode_data_to_image
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    decode_image_to_data(strlen(magic_string), decInfo->fptr_stego_image, decInfo->decode_magic, decInfo);
    if(strcmp(decInfo->decode_magic, magic_string) != 0)
    {
        return d_failure;
    }
    return d_success;
}

Status_d decode_extn_size(int size, FILE* fptr_stego_image, DecodeInfo* decInfo)
{
    decInfo -> secret_file_extn_size = 0;
    decode_image_to_data(size, decInfo -> fptr_stego_image, decInfo -> decode_magic, decInfo);
    for(int i=0; i<size; i++)
    {
        decInfo -> secret_file_extn_size = ( decInfo -> secret_file_extn_size | ((int)decInfo -> decode_magic[i] << (8*i) ) );
    }
    if( decInfo -> secret_file_extn_size != MAX_FILE_SUFFIX )
    {
        return d_failure;
    }
    return d_success;
}

Status_d decode_secret_file_extn(int size,  DecodeInfo *decInfo)
{
    decode_image_to_data( size,decInfo -> fptr_stego_image, decInfo -> decode_magic, decInfo);
    //storing secret file extension ie .txt in dec_magic only
    if( strcmp( decInfo -> decode_magic, ".txt") != 0 )
    {
        return d_failure;
    }
    return d_success;
}

Status_d decode_secret_file_size(long size, FILE* fptr_stego_image, DecodeInfo* decInfo)
{
    decInfo -> secret_file_size = 0;
    decode_image_to_data(size, decInfo -> fptr_stego_image, decInfo -> decode_magic, decInfo);
    for(int i=0; i<size; i++)
    {
        decInfo -> secret_file_size = ( decInfo -> secret_file_size | ((int)decInfo -> decode_magic[i] << (8*i) ) );
    }
    return d_success;
}

Status_d open_secret_file(DecodeInfo* decInfo)
{
    //decode_text.txt file
    decInfo -> fptr_decode_text = fopen(decInfo -> decode_text_fname, "w");
    //do error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");    //builtin function to print error message
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_text_fname);
    	return d_failure;
    }
    return d_success;
}

Status_d decode_secret_file_data(long size, DecodeInfo* decInfo)
{
    char ch;
    for(int i=0; i<size; i++)
    {
        //read 8 bytes from stego.bmp and store it in dec_image_data
        fread(decInfo -> image_data, 8, sizeof(char), decInfo -> fptr_stego_image);
        //decode the data
        decode_lsb_to_byte(&ch, decInfo -> image_data);
        fwrite(&ch, 1, 1, decInfo -> fptr_decode_text);
    }
    return d_success;
}

Status_d do_decoding(DecodeInfo *decInfo)
{
    if(open_files_d(decInfo) == d_success)
    {
        printf("Opened stego.bmp file\n");
        if(decode_magic_string(MAGIC_STRING, decInfo) == d_success)
        {
            printf("Decoding magic string Signature\n");

            if(decode_extn_size(strlen(".txt"),decInfo -> fptr_stego_image, decInfo) == d_success)
            {
                printf("Decode secret file extension size is successful\n");

                if(decode_secret_file_extn(decInfo->secret_file_extn_size, decInfo) == d_success)
                {
                    printf("Successfully decoded secret file extension\n");

                    if(decode_secret_file_size( MAX_FILE_SUFFIX,decInfo -> fptr_stego_image, decInfo) == d_success)
                    {
                        printf("Successfully decoded secret file size\n");

                        if(open_secret_file(decInfo) == d_success)
                        {
                            printf("Opened secret file successfully\n");

                            if(decode_secret_file_data( decInfo -> secret_file_size,decInfo) == d_success)
                            {
                                return d_success;
                            }
                            else
                            {
                                return d_failure;
                            }
                        }
                        else
                        {
                            printf("Failed to open secret file\n");
                            return d_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to decode secret file size\n");
                        return d_failure;
                    }
                }
                else
                {
                    printf("Failed to decode secret file extension\n");
                    return d_failure;
                }
            }
            else
            {
                printf("Failed to decode secret file extension size\n");
                return d_failure;
            }
        }
        else
        {
            printf("Failed to Encode magic string\n");
            return d_failure;
        }
    }
    else
    {
        printf("Failed opening the files\n");
        return d_failure;
    }

    return d_success;

}