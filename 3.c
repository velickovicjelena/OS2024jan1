#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

#define RD_END (0)
#define WR_END (1)

#define MAX_SIZE (256)

int main(int argc, char** argv){

    checkError(argc == 2, "....");

    int parToCld[2];
    checkError(pipe(parToCld) != -1, "...");

    pid_t childPid = fork();
    checkError(childPid != -1, "...");

    if(childPid > 0){

        close(parToCld[RD_END]);

        char* nameDir = malloc(MAX_SIZE);
        checkError(nameDir != NULL, "...");
        strcpy(nameDir, argv[1]);
        strcat(nameDir, ".dir");

        checkError(write(parToCld[WR_END], nameDir, sizeof(nameDir)) != -1, "...");

        free(nameDir);
        close(parToCld[WR_END]);
    }
    else if(childPid == 0){
        
        close(parToCld[WR_END]);

        char* buffer = malloc(MAX_SIZE);
        checkError(buffer != NULL, "...");

        checkError(read(parToCld[RD_END], buffer, MAX_SIZE) != -1, "...");

        mode_t oldUmask = umask(0);

        checkError(mkdir(buffer, 0777) != -1, "...");

        umask(oldUmask);
        free(buffer);

        close(parToCld[RD_END]);

        exit(EXIT_SUCCESS);
    }

    int status = 0;
    checkError(wait(&status)!= -1, "...");

    if(WEXITSTATUS(status) && (WIFEXITED(status) == EXIT_FAILURE)){
        exit(EXIT_FAILURE);
    }
    else{
        exit(EXIT_SUCCESS);
    }
}