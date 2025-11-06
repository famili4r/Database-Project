#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "parse.h"

int create_db_header(struct dbheader_t **headerOut){


    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == -1) {
        printf("Create  DBHeader Malloc failed\n");
        return 1;
    }
    header->version = 0x1;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return 0;
}

int validate_db_header(int fd, struct dbheader_t **headerOut){


    if (fd < 0) {
        printf("Bad dbfd\n");
        return 1;
    }
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == -1) {
        printf("Create  DBHeader Malloc failed\n");
        return 1;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("open");
        free(header);
        return 1;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if (header->version != 1) {
        printf("Improper header version\n");
        free(header);
        return -1;
    }
    if (header->magic != HEADER_MAGIC) {
        printf("Improper header magic\n");
        free(header);
        return 1;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size){
        printf("Corrupt DB\n");
        free(header);
        return 1;
    }
    *headerOut = header;
    return 0;


}

int output_file(int fd, struct dbheader_t *dbheader, struct employee_t *employees){
    
    if (fd < 0) {
        printf("Bad dbfd\n");
        return 1;
    }

    dbheader->magic = htonl(dbheader->magic);
    dbheader->filesize = htonl(dbheader->filesize);
    dbheader->count = htons(dbheader->count);
    dbheader->version = htons(dbheader->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, dbheader, sizeof(struct dbheader_t));

    return 0;
}