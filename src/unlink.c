#define _GNU_SOURCE
#define MAX_PATH_LEN 4096
#define MAX_FILENAME_LEN 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

char cwd[MAX_PATH_LEN];

int find_last_slash(const char *s);
int find_first_slash(const char *s);
void logger(const char *home_path,const char *path_name, const char *function);
int my_unlink(const char *path_name, const char *func_name);

int unlink(const char *path_name) 
{
    my_unlink(path_name, __func__);
    return 0;
}

int unlinkat(int dirfd, const char *path_name, int flags) 
{
    my_unlink(path_name, __func__);
    return 0;
}      
int my_unlink(const char *path_name, const char *func_name)
{ 
    char old_path_name[MAX_PATH_LEN];
    strcpy(old_path_name, getcwd(cwd, sizeof(cwd)));
    strcat(old_path_name, "/");
    int index = find_first_slash(path_name);
    strcat(old_path_name, path_name + index);
    char new_path[MAX_PATH_LEN];
    char *home_path = getenv("HOME");
    if(!home_path)
    {
        fprintf(stderr, "ERROR: can't get home_path environment\n");
        exit(1);
    }
    sprintf(new_path, "%s/trash", home_path);
    mkdir(new_path, 0755);                        // если каталог корзины существует, ничего не делать 
    index = find_last_slash(old_path_name);       // находим позицию последнего '/'
    strcat(new_path, "/");
    strcat(new_path, old_path_name + index);          // присоединяем имя файла    
    if(rename(old_path_name, new_path))
    {
        perror("rename");
        exit(errno);  
    }         
    logger(home_path, old_path_name, func_name);      // логирование сообщений в stdout и в файл trash.log в домашнем каталоге 
    return 0;
}

void logger(const char *home_path, const char *path_name, const char *function)
{
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    char log_path[MAX_FILENAME_LEN];
    sprintf(log_path, "%s/trash.log", home_path);
    FILE *log = fopen(log_path, "a");  
    if(!log)
    {
        fprintf(stderr, "ERROD: %s can't be opened/created\n", log_path);
        exit(1);
    }
    fprintf(log, "%s was moved to %s/trash by %s syscall on %s", path_name, home_path, function, asctime(timeinfo));
	printf("%s was succesfully moved to %s/trash by %s syscall\n",	path_name, home_path, function);
}

int find_last_slash(const char *s)
{
    int index = 0;
    int len = strlen(s);
    for(int i = len - 1; i > 0; i--)             // идем с конца строки до первого '/' для получения имени удаляемого файла
    {
        if(s[i] == '/') 
        {
            index = i + 1;
            break;
        }
    }
    return index;
}

int find_first_slash(const char *s)
{
    int index = 0;
    int len = strlen(s);
    for(int i = 0; i < len; i++)             // идем с конца строки до первого '/' для получения имени удаляемого файла
    {
        if(s[i] == '/') 
        {
            index = i + 1;
            break;
        }
    }
    return index;
}
