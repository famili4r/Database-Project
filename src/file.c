#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "file.h"

int create_db_file(char *filePath) {
    
    int fd;
    fd = open(filePath, O_RDONLY);
    if (fd != -1){
        printf("File already exists!\n");
        close(fd);
        return -1;
    }

    fd = open(filePath, O_RDWR | O_CREAT, 0644);
    if (fd == -1){
        perror("open");
        return -1;
    }
    return fd;
}

int open_db_file(char *filePath) {

    int fd;

    fd = open(filePath, O_RDWR);
    if(fd == -1){
        perror("open");
        return -1;
    }
    return fd;
}
