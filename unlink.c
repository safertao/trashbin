#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int unlink(const char *pathname) 
{
    char *buf = calloc(255, sizeof(char));
    strcpy(buf, getenv("HOME"));
    strcat(buf, "/trash");
    mkdir(buf, 0755);
    int len = strlen(pathname);
    int index = 0;
    for(int i = len - 1; i > 0; i--)
    {
        if(pathname[i] == '/') 
        {
            index = i + 1;
            break;
        }
    }
    strcat(buf, "/");
    strcat(buf, pathname+index);
    printf("%s\n%s\n", pathname, buf);
    rename(pathname, buf);
    return 0;
}

int unlinkat(int dirfd, const char *pathname, int flags) 
{
    char *buf = calloc(255, sizeof(char));
    strcpy(buf, getenv("HOME"));
    strcat(buf, "/trash");
    mkdir(buf, 0755);
    int len = strlen(pathname);
    int index = 0;
    for(int i = len - 1; i > 0; i--)
    {
        if(pathname[i] == '/') 
        {
            index = i + 1;
            break;
        }
    }
    strcat(buf, "/");
    strcat(buf, pathname+index);
    printf("%s\n%s\n", pathname, buf);
    rename(pathname, buf);
    return 0;
}