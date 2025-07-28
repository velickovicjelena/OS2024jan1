#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <utime.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

extern char** environ;

int sigusr1 = 0;
int sigusr2 = 0;
int exitfl = 0;

void sigFunc(int signum){

    switch (signum)
    {
        case SIGUSR1:
            sigusr1 = 1;
            break;
        case SIGUSR2:
            sigusr2 = 1;
            break;
        case SIGQUIT:
            exitfl = 1;
            break;
    }  
}

int main(int argc, char** argv){

    checkError(argc == 2, "args");

    char* path = getenv("PUTANJA");
    checkError(path != NULL, "getenv");

    time_t new = atol(argv[1]);

    do{
        checkError(signal(SIGUSR1, &sigFunc) != SIG_ERR, "...");
        checkError(signal(SIGUSR2, &sigFunc) != SIG_ERR, "...");
        checkError(signal(SIGQUIT, &sigFunc) != SIG_ERR, "...");

        pause();

        if(sigusr1){
            
            char* prava = malloc(4);
            checkError(prava != NULL, "prava");
            strcpy(prava, "rwx");

            struct stat fInfo;
            checkError(stat(path, &fInfo) != -1, "stat");

            if((S_IRUSR & fInfo.st_mode) != S_IRUSR){
                prava[0] = '-';
            }
            if((S_IWUSR & fInfo.st_mode) != S_IWUSR){
                prava[1] = '-';
            }
            if((S_IXUSR & fInfo.st_mode) != S_IXUSR){
                prava[2] = '-';
            }
            printf("%s\n", prava);

            free(prava);
            sigusr1 = 0;
        }
        else if(sigusr2){
            struct utimbuf ut;

            ut.actime = new;
            ut.modtime = new;

            checkError(utime(path, &ut) != -1, "utime");
            sigusr2 = 0;
        }
    }while(exitfl == 0);

    exit(EXIT_SUCCESS);
}