#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include "file.h"
#include "parse.h"
#include "crypt.h"



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
    char *filePath = NULL;
    bool encrypt = false;



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
        if (create_db_file(filePath) != 0){
            printf("Database creation failed\n");
            return 1;
        }
    }

    if (encrypt){
        unsigned char *key = malloc(sizeof(char));
        if (key == NULL){
            printf("initial key malloc failed\n");
            return 1;
        }
        unsigned char *iv = malloc(sizeof(char));
        if (iv == NULL){
            printf("inital iv malloc failed\n");
            free(key);
            return 1;
        }
        if(generateKey(&key, &iv) != 0){
            printf("Key generation failed\n");
            free(key);
            free(iv);
            return 1;
        }
        free(key);
        free(iv);
    }

    printf("Newfile: %d\n", newFile);
    printf("Filepath: %s\n", filePath);
    

    return 0;
}