#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
    //if CMYK --> 4
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");    //builtin function to print error message
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char* argv[], EncodeInfo* encInfo)
{
    //get .bmp file - ./a.out -e beautiful.bmp
    if(argv[2] != NULL && strcmp( (strstr(argv[2], ".")),".bmp") == 0 )
    {
        encInfo -> src_image_fname = argv[2];
    } 
    else
    {
        return e_failure;
    }
    //get .txt file - ./a.out -e beautiful.bmp secret.txt
    if(argv[3] != NULL && strcmp( (strstr(argv[3], ".")),".txt") == 0 )
    {
        encInfo -> secret_fname = argv[3];
    } 
    else
    {
        return e_failure;
    }
    //get stago.bmp file - ./a.out -e beautiful.bmp secret.txt stago.bmp(optional)
    if( argv[4] != NULL)
    {
        if(strcmp( (strstr(argv[4], ".")),".bmp") == 0 )
        {
            encInfo -> stego_image_fname = argv[4];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        encInfo -> stego_image_fname = "stego.bmp";
    }
    return e_success;
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}
Status check_capacity(EncodeInfo* encInfo)
{
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    if(encInfo -> image_capacity > (54 + (2+4+4+4+encInfo -> size_secret_file)*8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status copy_bmp_header(FILE* fptr_src_image, FILE* fptr_stego_image)
{
    char str[54];
    rewind(fptr_src_image); //move fptr_src_image to 1st pos
    fread(str, sizeof(char), 54, fptr_src_image);
    fwrite(str, sizeof(char), 54, fptr_stego_image);
    return e_success;

}
Status encode_byte_to_lsb(char data, char* image_buffer)
{
    uint mask =  1 << 7;
    for(int i=0; i<8; i++)
    {
        //image_buffer[i] - fetching each byte from source .bmp image 
        //0XFE used to clear LSB bit of image_buffer[i]
        image_buffer[i] = ((image_buffer[i] & 0xFE)  | ((data & mask) >> (7-i)));
        mask = mask >> 1;
    }
    return e_success;
}
Status encode_data_to_image(const char* data, int size, FILE* fptr_src_image, FILE* fptr_stego_image, EncodeInfo* encInfo)
{
    for(int i=0; i<size; i++)
    {
        //read 8 bytes from beautiful.bmp and store it in image_data
        fread(encInfo -> image_data, 8, sizeof(char), fptr_src_image);
        //encode the data
        encode_byte_to_lsb(data[i], encInfo -> image_data);
        fwrite(encInfo -> image_data, 8, sizeof(char), fptr_stego_image);
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo* encInfo)
{
    //every encoding will call encode_data_to_image
    encode_data_to_image(magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}
Status encode_size_to_lsb(char* buffer , int size)
{
    uint mask = 1 << 7;
    for(int i=0; i<32; i++)
    {
        buffer[i] = ((buffer[i] & 0xFE) | ((size & mask) >> (7-i)));
        mask = mask >> 1;
    }
    return e_success;
}
Status encode_size(int size, FILE* fptr_src_image, FILE* fptr_stego_image)
{
    char str[32];// to encode 4 bytes
    fread(str, 32, sizeof(char), fptr_src_image);
    encode_size_to_lsb(str, size);
    fwrite(str, 32, sizeof(char), fptr_stego_image);
    return e_success;
}
Status encode_secret_file_extn(const char* file_ext, EncodeInfo* encInfo)
{
    file_ext = ".txt";
    encode_data_to_image(file_ext, strlen(file_ext), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}
Status encode_secret_file_size(int size, EncodeInfo* encInfo)
{
    char str[32];// to encode 4 bytes
    fread(str, 32, sizeof(char), encInfo -> fptr_src_image);
    encode_size_to_lsb(str, size);
    fwrite(str, 32, sizeof(char), encInfo -> fptr_stego_image);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo* encInfo)
{
    char ch;
    rewind(encInfo -> fptr_secret); //move cursor to starting in secret file
    for(int i=0; i<encInfo -> size_secret_file; i++)
    {
        fread(encInfo -> image_data, 8, sizeof(char), encInfo -> fptr_src_image);
        fread(&ch, 1, sizeof(char), encInfo -> fptr_secret);
        encode_byte_to_lsb(ch, encInfo -> image_data);
        fwrite(encInfo -> image_data, 8, sizeof(char), encInfo -> fptr_stego_image);
    }
    return e_success;
}
Status copy_remaining_img_data(FILE* fptr_src_image, FILE* fptr_stego_image)
{
    char ch;
    while( fread(&ch, 1, 1, fptr_src_image) > 0)
    {
        fwrite(&ch, 1, 1, fptr_stego_image);
    }
    return e_success;
}
Status do_encoding(EncodeInfo* encInfo)
{
    //rest of the encoding function
    //open files
    if(open_files(encInfo) == e_success)
    {
        printf("Opened all the required Files\n");
        //check capacity
        if(check_capacity(encInfo) == e_success)
        {
            printf("Check capacity is success..Can proceed to Encode the data\n");
            //copy header info from src to dst
            if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success )
            {
                printf("Copied Header successfully\n");
                //Encode magic string
                if( encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Encoded magic string successfully\n");
                    //encode secret file extension size
                    if(encode_size(strlen(".txt"), encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                    {
                        printf("Encode secret file extension size successful\n");
                        //Encode secret file extension
                        if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                        {
                            printf("Encode secret file extension successful\n");
                            //encode secret file size
                            if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
                            {
                                printf("Successfully encoded secret file size\n");
                                //encode secret file data
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Successfully encoded secret data.... :)\n");
                                    //Copy remaining image bytes from src to stego image after encoding
                                    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                                    {
                                        printf("Copied remaining bytes successfully.. :)\n");
                                    }
                                    else
                                    {
                                        printf("Failed to copy remaining bytes.\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Failed encoding secret data.. :( \n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Failed encoding secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Encode secret file extension Unsuccessful\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed Encoding secret file extension size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Falied to Encode magic string\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failure: Copying Header\n");
                return e_failure;
            }

        }
        else
        {
            printf("Not possible to encode the data\n");
            return e_failure;
        }
    }
    else
    {
        printf("Falied opening the Files\n");
        return e_failure;
    }
    return e_success;
}
