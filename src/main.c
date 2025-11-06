#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include "file.h"
#include "parse.h"
#include "common.h"


void printUsage(char *argv[]) {
    printf("Usage: %s -n -f <databasefile.db>\n", argv[0]);
    printf("\t -n - Create a new database file\n");
    printf("\t -f (Req) Path to the database file\n ");
    printf("\t -e Encrypt the database\n");
    return;
}



int main(int argc, char *argv[]){
    int c = 0;
    bool newFile = false;
    int dbfd = -1;
    char *filePath = NULL;
    bool encrypt = false;
    struct dbheader_t *dbheader = NULL;
    struct employee_t *employees = NULL;


    while ((c = getopt(argc, argv, "nef:")) != -1){

        switch(c){
            case 'n':
                newFile = true;
                break;
            case 'e':
                encrypt = true;
            case 'f':
                filePath = optarg;
                break;
            
            case '?':
                printf("Unknown option -%c\n", c);
                break;

            default:
                return -1;
        }
    }

    if (filePath == NULL) {
        printf("Filepath is a required argument\n");
        printUsage(argv);
        
        return 0;
    }

    if (newFile){
        dbfd = create_db_file(filePath);
        if (dbfd == -1){
            printf("Failed to create the database\n");
            return -1;
        }
        if (create_db_header(&dbheader) == 1){
            printf("Failed to create the database header\n");
            return -1;
        }
    } else {
        dbfd = open_db_file(filePath);
        if (dbfd == -1){
            printf("Failed to open the database\n");
            return -1;
        }

        if (validate_db_header(dbfd, &dbheader) == 1){
            printf("Failed to validate database header\n");
            return -1;
        }
    }
 
    if (output_file(dbfd, dbheader, employees) == 1) {
        printf("Failed to write into file\n");
        return -1;
    }
    printf("Newfile: %d\n", newFile);
    printf("Filepath: %s\n", filePath);


    if (dbfd != -1) {
        close(dbfd);
    }


    return 0;
}