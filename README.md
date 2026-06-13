# LSB-Image-Steganography
This project implements LSB (Least Significant Bit) steganography — a technique for concealing secret data inside a BMP image file without producing any visible change to the image. Every pixel in a 24-bit BMP is stored as three bytes representing its red, green, and blue channel values. By replacing the least significant bit of each byte with one bit of the secret message, the pixel color shifts by at most one unit out of 255 — a difference completely invisible to the human eye. Because no pixel is added or removed and the file dimensions stay identical, the stego image is indistinguishable from the original when viewed normally, making it an effective tool for covert communication.

Beyond the core bit-manipulation technique, the program embeds structured metadata alongside the secret payload so that decoding is self-contained and reliable. Before any secret data is written, a magic string (#*) is encoded into the image as a fingerprint, allowing the decoder to immediately verify whether a given BMP file actually contains hidden data. Following the magic string, the extension of the secret file, its byte length, and finally the secret content itself are each encoded in sequence. This layered encoding scheme means that a decoder reading the stego image can reconstruct the original file completely — correct file type, correct size, and correct content — without needing any external metadata file or prior knowledge of what was hidden.

File structure
FilePurposetest_encode.cEntry point. Parses the -e/-d flag and delegates to encode or decode.encode.c / encode.hAll encoding logic and the EncodeInfo structure.decode.c / decode.hAll decoding logic and the DecodeInfo structure.types.hShared type definitions: Status, OperationType, uint.common.hDefines the magic string #* used to verify a stego image.

Encoding pipeline
Invoked with: ./a.out -e beautiful.bmp secret.txt [stego.bmp]

Validate arguments — confirms .bmp source and .txt secret file; defaults output to stego.bmp if not provided.
Open files — opens source image, secret file, and stego output.
Check capacity — computes available pixel bytes vs. the total bits needed: magic × 8 + 32 + ext_size × 8 + 32 + secret_size × 8. Returns failure if the image is too small.
Copy BMP header — writes the first 54 bytes of the source image unchanged into the stego file, preserving all BMP metadata.
Encode magic string (#*) — the user-entered magic string is encoded into LSBs so decoders can verify the image is stego-encoded.
Encode extension size — stores the byte-length of the secret file's extension as a 32-bit integer across 32 pixel bytes.
Encode extension — embeds the extension string (e.g. .txt) so the decoder knows what file type to reconstruct.
Encode file size — embeds the secret file's byte count as another 32-bit integer.
Encode secret data — reads the secret file byte by byte, spreading each byte's 8 bits across 8 consecutive pixel bytes.
Copy remaining image data — all unmodified pixel bytes are written through to the stego file.

Decoding pipeline
Invoked with: ./a.out -d stego.bmp [output.txt]

Validate arguments — confirms .bmp stego input; defaults output to output.txt.
Open files — opens the stego image for reading and the destination for writing.
Seek past header — skips the 54-byte BMP header to reach pixel data.
Verify magic string — reads and reconstructs 2 bytes from LSBs; if they don't match #*, decoding aborts.
Decode extension size — reads 32 pixel bytes and reconstructs the integer from their LSBs.
Decode extension — reconstructs the file extension string character by character.
Decode file size — reads the next 32 pixel bytes to reconstruct the secret file's byte count.
Decode secret data — reads groups of 8 pixel bytes, extracts the LSB of each, assembles them into bytes, and writes them to the output file.

Core encoding mechanism
The encode_byte_to_lsb function is the heart of the system. For each bit of a data byte (MSB first), it clears the LSB of the corresponding image buffer byte with & 0xFE, then ORs in the data bit. The decoder reverses this by collecting the LSB of each image byte and shifting them back into position.

# Compile
gcc test_encode.c encode.c decode.c -o a.out

# Encode
./a.out -e beautiful.bmp secret.txt stego.bmp

# Decode
./a.out -d stego.bmp output.txt

Known limitations / bugs to note

open_de_files checks deInfo->stego_image_fname == NULL instead of deInfo->fptr_stego_image == NULL — the NULL check will never trigger correctly.
Files are opened in text mode ("r", "w") rather than binary mode ("rb", "wb"), which can corrupt BMP data on Windows due to newline translation.
decode_magic initialises j = 0 but increments it before the first use, so magic_data[0] is never written and index 1 is used as the first character.
copy_remaining_img_data uses ftell position comparison against image_capacity (a pixel count), which may not equal the actual file offset — a potential infinite loop.
