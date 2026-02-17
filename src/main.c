#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include "file.h"
#include "parse.h"
#include "common.h"
#include "network.h"


void cleanUp(int dbfd, struct dbheader_t *dbheader, struct employee_t *employees){
    
    if (dbfd != -1) {
        close(dbfd);
    }
    if (dbheader != NULL){
        free(dbheader);
    }
    if (employees != NULL){
        free(employees);
    }
    
    return;
}

void printUsage(char *argv[]) {

    printf("Usage: %s -n -f <databasefile.db>\n", argv[0]);
    printf("\t -n - Create a new database file\n");
    printf("\t -f (Req) Path to the database file\n ");
    printf("\t -a Add entry to the database\n ");
    printf("\t -r Remove entry from the database\n ");
    printf("\t -c Adjust Employee hours in the database");
    printf("\t -l List database entries\n ");

    return;
}

int main(int argc, char *argv[]){

    int c = 0;
    bool newFile = false;
    int dbfd = -1;
    char *filePath = NULL;
    char *addString = NULL;
    char *removeString = NULL;
    char *adjustString = NULL;
    bool encrypt = false;
    bool list = false;
    struct dbheader_t *dbheader = NULL;
    struct employee_t *employees = NULL;


    while ((c = getopt(argc, argv, "nf:a:lr:c:")) != -1){

        switch(c){
            case 'n':
                newFile = true;
                break;
            case 'a':
                addString = optarg;
                break;
            case 'r':
                removeString = optarg;
                break;
            case 'c':
                adjustString = optarg;
                break;
            case 'l':
                list = true;
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
        if (create_db_header(&dbheader) != 0){
            printf("Failed to create the database header\n");
            cleanUp(dbfd,dbheader,employees);
            return -1;
        }
    } else {
        dbfd = open_db_file(filePath);
        if (dbfd == -1){
            printf("Failed to open the database\n");
            return -1;
        }

        if (validate_db_header(dbfd, &dbheader) != 0){
            printf("Failed to validate database header\n");
            cleanUp(dbfd,dbheader,employees);
            return -1;
        }
    }
 
    if (read_employees(dbfd, dbheader, &employees) != 0){
        printf("Failed to read employees\n");
        cleanUp(dbfd,dbheader,employees);
        return -1;
    }

    if (addString) {
        if (add_employee(dbheader, &employees, addString) == -1){
            printf("Adding Employee failed!\n");
            cleanUp(dbfd,dbheader,employees);
            return -1;
        }
    }

    if (removeString) {
        if (remove_employee(dbheader, &employees, removeString) == -1){
            printf("Removing Employee failed!\n");
            cleanUp(dbfd,dbheader,employees);
            return -1;
        }
    }

    if (adjustString) {
        if (adjust_hours(dbheader, &employees, adjustString) == -1){
            printf("Adjusting hours failed\n");
            cleanUp(dbfd,dbheader,employees);
            return -1;
        }
    }

    if (list){
      if (list_employees(dbheader, employees) == -1){
        printf("Failed to list employees");
      }
    }

    if (output_file(dbfd, dbheader, employees) == -1) {
        printf("Failed to write into file\n");
        cleanUp(dbfd,dbheader,employees);
        return -1;
    }

    cleanUp(dbfd,dbheader,employees);

    return 0;
}