#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ftw.h>
#include <pwd.h>

#define checkError(cond, usrMsg)\
    do{\
        if(!(cond)){\
            perror(usrMsg);\
            exit(EXIT_FAILURE);\
        }\
    }while(0)

int fn(const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf){

    if(!S_ISFIFO(sb->st_mode)){
        return 0;
    }
    if(((S_IRGRP | S_IWGRP) & sb->st_mode) != (S_IRGRP | S_IWGRP)){
        return 0;
    }

    struct passwd* f = getpwuid(sb->st_uid);
    checkError(f != NULL, "...");
    char* absPath = realpath(fpath, NULL);
    
    printf("%s: %s\n", absPath, f->pw_name);   

    free(absPath);     
    
    return 0;
}


int main(int argc, char** argv){

    checkError(argc == 2, "...");

    checkError(nftw(argv[1], fn, 50, 0) != -1, "...");

    exit(EXIT_SUCCESS);
}