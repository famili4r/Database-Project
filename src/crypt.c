#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include "crypt.h"


int generateKey(unsigned char **outKey, unsigned char **outIv){
    int fd;
    size_t keySize = sizeof(char)*32;
    size_t ivSize = sizeof(char)*8;

    if ((fd = open ("/dev/random", O_RDONLY)) == -1){
        printf("fd failed to open\n");
        return 1;
    }
    unsigned char *key = malloc(keySize);
    if (key == NULL){
        printf("Key buffer malloc failed\n");
        close(fd);
        return 1;
    }
    unsigned char *iv = malloc(ivSize);
    if (iv == NULL){
        printf("IV buffer malloc failed\n");
        free(key);
        close(fd);
        return 1;
    }
    if (realloc(*outKey, keySize) == NULL){
        printf("Key realloc failed\n");
        free(key);
        free(iv);
        close(fd);
        return 1;
    }
     if (realloc(*outIv, ivSize) == NULL){
        printf("IV realloc failed\n");
        free(key);
        free(iv);
        close(fd);
        return 1;
    }

    if (read(fd, key, keySize) == -1){
        printf("Read to Key buffer failed\n");
        free(key);
        free(iv);
        close(fd);
        return 1;
    }
    
    if (read(fd, iv, ivSize) == -1){
        printf("Read to IV buffer failed\n");
        free(key);
        free(iv);
        close(fd);
        return 1;
    }


    *outKey = key;
    *outIv = iv;

    close(fd);
    return 0;
}
