#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    //check operation type
    if(check_operation_type(argv) == e_encode)
    {
        EncodeInfo encInfo;
        printf("Selectedd Encoding\n");
        if(read_and_validate_encode_args(argv,&encInfo) == e_success)
        {
            printf("Read and Validate arguments is successful\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("Encoding is done successful\n");
            }
            else
            {
                printf("Failed Encoding the data ! \n");
            }
        }
        else
        {
            printf("Read and Validate arguments is unsuccessful\n");
        }
    }
    else if(check_operation_type(argv) == e_decode)
    {
        DecodeInfo decInfo;
        printf("Selected Decoding\n");
        if(read_and_validate_decode_args(argv,&decInfo) == d_success)
        {
            printf("Read and Validate arguments is successful\n");
            if(do_decoding(&decInfo) == d_success)
            {
                printf("Decoding is done successful\n");
            }
            else
            {
                printf("Failed Decoding the data ! \n");
            }
        }
        else
        {
            printf("Read and Validate arguments is unsuccessful\n");
        }

    }
    else
    {
        printf("Invalid Option\n");
        printf("Usage:\nEncoding: ./a.out -e beautiful.bmp secret.txt stego.bmp(optional)");
        printf("Decoding: ./a.out -d stego.bmp\n");
    }

    // EncodeInfo encInfo;
    // uint img_size;

    // // Fill with sample filenames
    // encInfo.src_image_fname = "beautiful.bmp";
    // encInfo.secret_fname = "secret.txt";
    // encInfo.stego_image_fname = "stego_img.bmp";

    // // Test open_files
    // if (open_files(&encInfo) == e_failure)
    // {
    // 	printf("ERROR: %s function failed\n", "open_files" );
    // 	return 1;
    // }
    // else
    // {
    // 	printf("SUCCESS: %s function completed\n", "open_files" );
    // }

    // // Test get_image_size_for_bmp
    // img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    // printf("INFO: Image size = %u\n", img_size);

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    //./a.out -e
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }

}