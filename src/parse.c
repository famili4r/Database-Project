#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "parse.h"
#include "common.h"

int create_db_header(struct dbheader_t **headerOut){


    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("Create  DBHeader Malloc failed\n");
        return -1;
    }
    header->version = HEADER_VERSION;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return 0;
}

int validate_db_header(int fd, struct dbheader_t **headerOut){


    if (fd < 0) {
        printf("Bad dbfd\n");
        return -1;
    }
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
    if (header == NULL) {
        printf("Create DBHeader Malloc failed\n");
        return -1;
    }

    if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("open");
        free(header);
        return -1;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if (header->version != HEADER_VERSION) {
        printf("Improper header version\n");
        free(header);
        return -1;
    }
    if (header->magic != HEADER_MAGIC) {
        printf("Improper header magic\n");
        free(header);
        return -1;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);
    if (header->filesize != dbstat.st_size){
        printf("Corrupt DB\n");
        free(header);
        return -1;
    }
    *headerOut = header;
    return 0;


}

int output_file(int fd, struct dbheader_t *dbheader, struct employee_t *employees){

    if (fd < 0) {
        printf("Bad dbfd\n");
        return -1;
    }
    int realCount = dbheader->count; 
    dbheader->magic = htonl(dbheader->magic);
    dbheader->filesize = htonl(sizeof(struct dbheader_t) + sizeof(struct employee_t) * realCount);
    dbheader->count = htons(dbheader->count);
    dbheader->version = htons(dbheader->version);

    lseek(fd, 0, SEEK_SET);

    write(fd, dbheader, sizeof(struct dbheader_t));

  
    int i = 0;

    for(; i < realCount; i++){
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
        if(ftruncate(fd, sizeof(struct dbheader_t) + sizeof(struct employee_t) * realCount) != 0){
            perror("ftrancate error");
            return -1;
        }
    }


    return 0;
}

int read_employees(int fd, struct dbheader_t *dbheader, struct employee_t **employeesOut){
    if (fd < 0) {
        printf("Bad dbfd\n");
        return 1;
    }
    int count = dbheader->count;

    struct employee_t *employees = calloc(count, sizeof(struct employee_t));
    if (employees == NULL){
        printf("employee Malloc failed!\n");
        return 1;
    }
    read(fd, employees, count*sizeof(struct employee_t));

    int i = 0;
    for (; i < count; i++){
        employees[i].hours = htonl(employees[i].hours);
    }

    *employeesOut = employees;

    return 0;
}

int add_employee(struct dbheader_t *dbheader, struct employee_t **employees, char *addString){


    if (NULL == dbheader){
        return -1;
    }
    if (NULL == employees){
        return -1;
    }
    if (NULL == *employees){
        return -1;
    }
    if (NULL == addString){
        return -1;
    }

    char *name = strtok(addString, ",");
    if (NULL == name){
        return -1;
    }
    char *address = strtok(NULL, ",");
    if (NULL == address){
        return -1;
    }
    char *hours = strtok(NULL, ",");
    if (NULL == hours){
        return -1;
    }
    struct employee_t *employeesDR = *employees;
    employeesDR = realloc(employeesDR, sizeof(struct employee_t)*(dbheader->count+1));
    if (employeesDR == NULL){
        return -1;
    }

    dbheader->count++;
    strncpy(employeesDR[dbheader->count-1].name, name, sizeof(employeesDR[dbheader->count-1].name)-1);

    strncpy(employeesDR[dbheader->count-1].address, address, sizeof(employeesDR[dbheader->count-1].address)-1);
    employeesDR[dbheader->count-1].hours = atoi(hours);
    
    *employees = employeesDR;

    return 0;
}

int remove_employee(struct dbheader_t *dbheader, struct employee_t **employees, char *removeString){
    
    if (NULL == dbheader){
        return -1;
    }
    if (NULL == employees){
        return -1;
    }
    if (NULL == *employees){
        return -1;
    }
    if (NULL == removeString){
        return -1;
    }
    
    struct employee_t *employeesDR = *employees;
    employeesDR = realloc(employeesDR, sizeof(struct employee_t)*(dbheader->count));
    if (employeesDR == NULL){
        return -1;
    }



    int i = 0;

    for(;i < dbheader->count; i++){
        if (strcmp(employeesDR[i].name, removeString) == 0){

            
            int j = i;
            printf("%s Deleted\n", employeesDR[j].name);
            for (;j < dbheader->count;j++){
                employeesDR[j] = employeesDR[j+1];
            }
            
            struct employee_t *employeesShrink = employeesDR;
            if (employeesShrink == NULL){
                return -1;
            }
            employeesShrink = realloc(employeesShrink, sizeof(struct employee_t)*(dbheader->count-1));
            dbheader->count--;
            employeesDR = employeesShrink;
            break;
        }
    }
    *employees = employeesDR;
    return 0;
}

int adjust_hours(struct dbheader_t *dbheader, struct employee_t **employees, char *adjustString){
        
    if (NULL == dbheader){
        return -1;
    }
    if (NULL == employees){
        return -1;
    }
    if (NULL == *employees){
        return -1;
    }
    if (NULL == adjustString){
        return -1;
    }

    char *name = strtok(adjustString, ",");
    if (NULL == name){
        return -1;
    }
    char *hours = strtok(NULL, ",");
    if (NULL == hours){
        return -1;
    }

    struct employee_t *employeesDR = *employees;

    int i = 0;

    for(;i < dbheader->count; i++){
        if (strcmp(employeesDR[i].name, name) == 0){
            employeesDR[i].hours = atoi(hours); 
            break;
        }
    }
    *employees = employeesDR;
    return 0;
}

int list_employees(struct dbheader_t *dbheader, struct employee_t *employees){
    
    if (NULL==dbheader){
        return -1;
    }
    if(NULL==employees){
        return -1;
    }

    int i = 0;
    for(;i < dbheader->count; i++){
        printf("Employee: %d\n", i);
        printf("\t Name: %s\n", employees[i].name);
        printf("\t Address: %s\n", employees[i].address);
        printf("\t Hours: %d\n", employees[i].hours);
    }
    return 0;
}