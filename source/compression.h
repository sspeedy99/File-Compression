/*
Author : ssp3edy 
Date : 2019-June-06
*/

#define MAX 16
// padding is done to ensure that the code generated for each charater will fit byte size.
// i.e : 4 byte + 3bits will be consider as 5 bits.
char padding;
unsigned char N;


// Code table regarding every character in the file
typedef struct codeTable
{
    char x;
    char code[MAX];
} codeTable;

char compressed_extension[]  = ".spd";
char decompressed_extension[] = ".txt";


