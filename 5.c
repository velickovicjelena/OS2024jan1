#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

#define checkPthread(errNum, usrMsg)\
    do{\
        int _err = (errNum);\
        if(_err > 0){\
            errno = _err;\
            checkError(0, "...");\
        }\
    }while(0)

#define BROJ_FAJLOVA (20)
#define DUZINA_FAJLA (256)

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int globalSum = 0;

typedef struct{
    char path[DUZINA_FAJLA];
}inArgs_t;

void* threadFunc(void* args){

    inArgs_t* INargs = (inArgs_t*)args;

    FILE* f = fopen(INargs->path, "r");
    checkError(f != NULL, "fopen");

    char* rec = malloc(DUZINA_FAJLA * sizeof(char));
    checkError(rec != NULL, "malloc");
    int localSum = 0;
    while (fscanf(f, "%s", rec) != EOF)
    {
        localSum += strlen(rec);
    }
    
    checkPthread(pthread_mutex_lock(&mutex), "...");
    globalSum += localSum;
    checkPthread(pthread_mutex_unlock(&mutex), "...");
    
    free(rec);
    fclose(f);
    return NULL;
}


int main(int argc, char** argv){

    checkError(argc == 1, "...");

    int granica = BROJ_FAJLOVA;

    char** files = malloc(granica * sizeof(char*));
    checkError(files != NULL, "...");
    for(int i = 0; i < granica; i++){
        files[i] = malloc(DUZINA_FAJLA * sizeof(char));
        checkError(files[i] != NULL, "...");
    }
    char* file = malloc(DUZINA_FAJLA * sizeof(char));
    checkError(file != NULL, "...");

    int brojUnetih = 0;
    while(fscanf(stdin, "%s", file) != EOF){

        struct stat fInfo;
        checkError(stat(file, &fInfo) != -1, "...");
        checkError(S_ISREG(fInfo.st_mode), "...");
        
        strcpy(files[brojUnetih], file);

        brojUnetih++;
        if(brojUnetih == granica){
            granica *= 2;
            files = realloc(files, granica * sizeof(char*));
            checkError(files != NULL, "realloc");
        }
    }

    inArgs_t* args = malloc(brojUnetih * sizeof(inArgs_t));
    checkError(args != NULL, "mallocargs");

    pthread_t* tids = malloc(brojUnetih * sizeof(pthread_t));
    checkError(tids != NULL, "malloctids");

    for(int i = 0; i < brojUnetih; i++){
        strcpy(args[i].path, files[i]);
        checkPthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "create");
    }
   
    for(int i = 0; i < brojUnetih; i++){
        checkPthread(pthread_join(tids[i], NULL), "join");
    }
    printf("%d\n", globalSum);

    checkPthread(pthread_mutex_destroy(&mutex), "...");

    for(int i = 0; i < brojUnetih; i++){
        free(files[i]);
    }
    free(files);
    free(file);
    free(tids);
    free(args);

    exit(EXIT_SUCCESS);
}