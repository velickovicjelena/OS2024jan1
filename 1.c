#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

// prolazi, ali nisam sigurna da li je dobro po pitanju apsolutnih putanja
// ./1.c ref na dir
// ../1.c ref na naddir
// /home/jelena/Desktop/jan1os2024/1.c abs path
// ../jan1os2024/1.c rel path

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

int main(int argc, char** argv){

    checkError(argc >= 2, "...");

    int indeks = 1, maxSize = 0;
    int brojPostojecih = 0;
 
    struct stat fInfo;

    for(int i = 1; i < argc; i++){
        if(stat(argv[i], &fInfo) != -1){
        
            if(fInfo.st_size > maxSize){
                maxSize = fInfo.st_size;
                indeks = i;
            }
            brojPostojecih++;
        }
    }

    checkError(brojPostojecih != 0, "...");

    char* name = strrchr(argv[indeks], '/');
    if(name == NULL){
        printf("%s\n", argv[indeks]);
    }
    else{
        printf("%s\n", name + 1);
    }

    exit(EXIT_SUCCESS);
}