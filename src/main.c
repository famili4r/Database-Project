#include <stdio.h>
#include <stdbool.h>
#include "file.h"
#include "parse.h"
#include <getopt.h>


void printUsage(char *argv[]) {
    printf("Usage: %s -n -f <databasefile.db>\n", argv[0]);
    printf("\t -n - Create a new database file\n");
    printf("\t -f (Req) Path to the database file\n ");
    return;
}



int main(int argc, char *argv[]){
    int c = 0;
    bool newFile = false;
    char *filePath = NULL;



    while ((c = getopt(argc, argv, "nf:")) != -1){

        switch(c){
            case 'n':
                newFile = true;
                break;
            
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
        printf("Filepath is a required argument\n")
        printUsage(argv);
        
        return 0;
    }

    if (newFile) {
        create_db_file();
    }

    printf("Newfile: %d\n", newFile);
    printf("Filepath: %s\n", filePath);
    

    return 0;
}