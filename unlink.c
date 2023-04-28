#define _GNU_SOURCE
#define MAX_PATH_LEN 4096

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int unlink(const char *pathname) 
{
     char *new_path = calloc(MAX_PATH_LEN, sizeof(char));
    char *home = getenv("HOME");
    if(!home)
    {
        fprintf(stderr, "ERROR: could not get home environment\n");
        exit(1);
    }
    strcpy(new_path, home);
    strcat(new_path, "/trash");
    mkdir(new_path, 0755);                       // если каталог корзины существует, ничего не делать 
    int len = strlen(pathname);
    int index = 0;
    for(int i = len - 1; i > 0; i--)             // идем с конца строки до первого '/' для получения имени удаляемого файла
    {
        if(pathname[i] == '/') 
        {
            index = i + 1;
            break;
        }
    }
    strcat(new_path, "/");
    strcat(new_path, pathname+index);            // присоединяем имя файла
    if(rename(pathname, new_path))
    {
        perror("ERROR");
        exit(errno);  
    }                  
    return 0;
}

int unlinkat(int dirfd, const char *pathname, int flags) 
{
    char *new_path = calloc(MAX_PATH_LEN, sizeof(char));
    char *home = getenv("HOME");
    if(!home)
    {
        fprintf(stderr, "ERROR: could not get home environment\n");
        exit(1);
    }
    strcpy(new_path, home);
    strcat(new_path, "/trash");
    mkdir(new_path, 0755);                       // если каталог корзины существует, ничего не делать 
    int len = strlen(pathname);
    int index = 0;
    for(int i = len - 1; i > 0; i--)             // идем с конца строки до первого '/' для получения имени удаляемого файла
    {
        if(pathname[i] == '/') 
        {
            index = i + 1;
            break;
        }
    }
    strcat(new_path, "/");
    strcat(new_path, pathname+index);            // присоединяем имя файла
    if(rename(pathname, new_path))
    {
        perror("ERROR");
        exit(errno);  
    }                  
    return 0;
}       