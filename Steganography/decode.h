#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file from source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *decode_text_fname;
    FILE *fptr_decode_text;
    int secret_file_extn_size;
    char secret_file_extn[MAX_IMAGE_BUF_SIZE];
    char decode_magic[MAX_IMAGE_BUF_SIZE];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long secret_file_size;

} DecodeInfo;

/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status_d do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status_d open_files_d(DecodeInfo *decInfo);

/* Get image size */
uint get_image_size_for_bmp_d(FILE *fptr_image);

/* Get file size */
uint get_file_size_d(FILE *fptr);

/* Decodes Magic String */
Status_d decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/*Decodes image to data */
Status_d decode_image_to_data( int size, FILE *fptr_stego_image, char* dec_data_buffer, DecodeInfo *decInfo);

Status_d decode_lsb_to_byte(char* data, char *dec_data_buffer);

Status_d decode_extn_size(int size, FILE* fptr_stego_image, DecodeInfo* decInfo);

Status_d decode_secret_file_extn(int size,  DecodeInfo *decInfo);

Status_d decode_secret_file_size(long size, FILE* fptr_stego_image, DecodeInfo* decInfo);

Status_d open_secret_file(DecodeInfo* decInfo);

Status_d decode_secret_file_data(long size, DecodeInfo* decInfo);

#endif
