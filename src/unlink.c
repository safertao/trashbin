#define _GNU_SOURCE
#define MAX_PATH_LEN 4096
#define MAX_FILENAME_LEN 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

int unlink(const char *pathname) 
{
    char new_path[MAX_PATH_LEN];
    char *home = getenv("HOME");
    if(!home)
    {
        fprintf(stderr, "ERROR: can't get home environment\n");
        exit(1);
    }
    sprintf(new_path, "%s/trash", home);
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
    strcat(new_path, pathname+index);           // присоединяем имя файла    
    time_t rawtime;
    struct tm * timeinfo;
    if(rename(pathname, new_path))
    {
        perror("ERROR");
        exit(errno);  
    }         
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    char log_path[MAX_FILENAME_LEN];
    sprintf(log_path, "%s/trash.log", home);
    FILE *log = fopen(log_path, "a");  
    if(!log)
    {
        fprintf(stderr, "ERROD: %s can't be opened/created\n");
        exit(1);
    }
    fprintf(log, "%s was moved to %s/trash by %s syscall on %s", pathname, 	home, __func__, asctime(timeinfo)); 
	printf("%s was succesfully moved %s/trash by %s syscall\n",	pathname, home, __func__);
    return 0;
}

int unlinkat(int dirfd, const char *pathname, int flags) 
{
    char new_path[MAX_PATH_LEN];
    char *home = getenv("HOME");
    if(!home)
    {
        fprintf(stderr, "ERROR: can't get home environment\n");
        exit(1);
    }
        sprintf(new_path, "%s/trash", home);
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
    strcat(new_path, pathname+index);           // присоединяем имя файла 
    time_t rawtime;
    struct tm * timeinfo;
    if(rename(pathname, new_path))
    {
        perror("ERROR");
        exit(errno);  
    }         
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    char log_path[MAX_FILENAME_LEN];
    sprintf(log_path, "%s/trash.log", home);
    FILE *log = fopen(log_path, "a");  
    if(!log)
    {
        fprintf(stderr, "ERROD: %s can't be opened/created\n");
        exit(1);
    }
    fprintf(log, "%s was moved to %s/trash by %s syscall on %s", pathname, 	home, __func__, asctime(timeinfo)); 
	printf("%s was succesfully moved %s/trash by %s syscall\n",	pathname, home, __func__);
    return 0;
}       