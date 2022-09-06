# Steganography
Steganography is the technique of hiding secret data within an ordinary, non-secret, file or message in order to avoid detection; the secret data is then extracted at its destination. This is the same principal that uses behind encrypting data. Here we can encrypt any text data in images and we can unencrypt them whenever the needed.

In these code there are two modes.
i) Encode mode
ii) Decode mode

Encode mode -> This mode can be accessed by giving "-e" with a.out.
Decode mode -> This mode can be accessed by giving "-d" with a.out.

In this code a sample image named as beautiful.bmp and a sample text file called secret.txt is added.The syntax for running the code after generating binary is

For encoding :  ./a.out -e <image_in_which_you_want_to_encode> <secret_msg_you_want_to_encode> <encoded_file_name_(optional)>

For decoding :  ./a.out -d <decoded_image_name> <decoded_secret_file_name_(optional)>

