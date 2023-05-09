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

void println();
void print_menu();
void input_option(char *option);
void list_trash_files();
void restore_file(const char *file_path);
void logger(const char *home_path, const char *path_name);
char *input_file_path();

int main()
{
    char option;
    println();
    print_menu();
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
                char *file_path = input_file_path();
                restore_file(file_path);
                free(file_path);
                break;
            }
            case 'q':
            {
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

void input_option(char *option)
{
    char tmp; 
    while(true)                                            
    {
        scanf("%c", &tmp);
        if(tmp == 'l' || tmp == 'r' || tmp == 'q') break;
    }
    *option = tmp;
    println();
}

void print_menu()
{
    printf("\n");
    printf("or input s<num> g<num> p<num>,\n");
    printf("where num - index of process\n");
}