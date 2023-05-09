#define _GNU_SOURCE
#define MAX_PATH_LEN 4096
#define MAX_FILENAME_LEN 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <dirent.h>

void println();
void print_menu();
void input_option(char *option);
void list_trash_files();
void restore_file(const char *filename);
void logger(const char *home_path, const char *path_name);
char *input_file_path();
void compute_trash_path();

char *trash_path = NULL;

int main()
{
    println();
    print_menu();
    char option;
    compute_trash_path();
    mkdir(trash_path, 0755);
    while(true)
    {
        input_option(&option);
        switch (option)
        {
            case 'l':
            {
                list_trash_files();
                break;
            }
            case 'r':
            {
                // char *file_path = input_file_path();
                // restore_file(file_path);
                // free(file_path);
                // break;
            }
            case 'q':
            {
                free(trash_path);
                exit(0);
                break;
            }
            default:
            {
                break;
            }
        }
        
    }
    return 0;
}

void println()
{
    printf("------------------------------------------------\n");
}

void compute_trash_path()
{
    trash_path = calloc(MAX_PATH_LEN, sizeof(*trash_path));
    char *home_path = getenv("HOME");
    if(!home_path)
    {
        fprintf(stderr, "ERROR: can't get home_path environment\n");
        exit(1);
    }
    sprintf(trash_path, "%s/trash", home_path);

}

void list_trash_files()
{
    DIR *dir = opendir(trash_path);
    struct dirent *read_dir;
    struct stat st;
    if (!dir)
    {
        if (errno)
        {
            fprintf(stderr, "trash : '%s' : Permission denied\n", trash_path);
            errno = 0;
            return;
        }
    }
    int files_count = 0;
    while((read_dir = readdir(dir)))
    {
        if(!files_count)
        {
            printf("trash files:\n");
        }
        if (!(strcmp(read_dir->d_name, ".") && strcmp(read_dir->d_name, "..")))
            continue;
        char tmp[MAX_PATH_LEN];
        strcpy(tmp, trash_path);
        strcat(tmp, "/");
        strcat(tmp, read_dir->d_name);
        printf("%s\n", tmp);
        files_count++;
    }
    if(!files_count)
    {
        fprintf(stderr, "trash: '%s' is empty\n", trash_path);
    }
    println();
    closedir(dir);
}

void input_option(char *option)
{
    char tmp; 
    while(true)                                            
    {
        scanf("%c", &tmp);
        if(tmp == 'l' || tmp == 'r' || tmp == 'm' || tmp == 'q') break;
    }
    *option = tmp;
    println();
}

void print_menu()
{
    printf("menu:\n");
    printf("q - exit\nl - list trash files\n");
    printf("m - print menu\nr - restore file from trash\n");
    println();
}