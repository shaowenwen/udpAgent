#ifndef RECSSL_H
#define RECSSL_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <string>
#include <fstream>
#include <string.h>
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include <memory.h>
#include "zlog.h"


#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string.h>
#include <string>
#include <iostream>

//#define uint8_t unsigned int
//#define BYTE    unsigned char
using namespace std;


class Recssl
{
public:
    Recssl();
    char * Base64Encode(const char * input, int length, bool with_new_line);
    char * Base64Decode(char * input, int length, bool with_new_line);
    string encrypto(char * Plaintext);
    void  Hex2Str( const char *sSrc,  char sDest[], int nSrcLen );
};

#endif // RECSSL_H
