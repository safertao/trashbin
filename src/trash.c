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
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

void println();
void print_menu();
void input_option(char *option);
void list_trash_files();
void restore_file(const char *filename);
void logger(const char *home_path, const char *path_name);
char *input_file_path();
void compute_trash_path();
int find_last_slash(const char *s);

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
                char file_path[MAX_PATH_LEN];
                strcpy(file_path, trash_path);
                strcat(file_path, "/");
                //char *filename = input_file_path();
                char filename[10] = "2";
                strcat(file_path, filename);
                restore_file(file_path);
                //free(filename);
                break;
            }
            case 'm':
            {
                print_menu();
                break;
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
            fprintf(stderr, "trash : '%s' : permission denied\n", trash_path);
            errno = 0;
            return;
        }
    }
    int files_count = 0;
    while((read_dir = readdir(dir)))
    {
        if (!(strcmp(read_dir->d_name, ".") && strcmp(read_dir->d_name, "..")))
            continue;
        if(!files_count)
        {
            printf("trash files:\n");
        }
        char tmp[MAX_PATH_LEN];
        strcpy(tmp, read_dir->d_name);
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

void restore_file(const char *filename)
{
    char trash_log_path[MAX_PATH_LEN];
    char *home_path = getenv("HOME");
    if(!home_path)
    {
        fprintf(stderr, "ERROR: can't get home_path environment\n");
        exit(1);
    }
    strcpy(trash_log_path, home_path);
    strcat(trash_log_path, "/trash.log");
    FILE *f = fopen(trash_log_path, "r+t");
    if(!f)
    {
        if(errno == 2)
        {
            fprintf(stderr, "trash.log doesn't exist");
            errno = 0;
            return;
        }
        perror("fopen");    // 
        exit(errno);
    }
    char file_path[MAX_PATH_LEN];
    char dest[MAX_PATH_LEN];
    int lines_count = 0;
    while(!feof(f))
    {
        if(!fgets(file_path, MAX_PATH_LEN, f))
        {
            if(!lines_count) 
            {
                printf("trash.log is empty\n");
                println();
            }
            fclose(f);
            return; 
        }
        char *path_end = strstr(file_path, " was moved to /home/");
        if(path_end)
        {
            strncpy(dest, file_path, ((path_end - file_path)/sizeof(char)));
        }
        int dest_index = find_last_slash(dest);
        int filename_index = find_last_slash(filename);
        if(strcmp(dest + dest_index, filename + filename_index)) continue;
        if(rename(filename, dest))
        {
            perror("rename");
            exit(errno);  
        }
        char dest_path[MAX_PATH_LEN];
        strncpy(dest_path, dest, dest_index);
        printf("%s was successfully restored from trashbin to %s\n",filename, dest_path);
        fclose(f);
        int fd = open(trash_log_path, O_RDWR);
        if(!fd)
        {
            perror("open");
            exit(errno);
        }
        struct stat s;
        fstat(fd, &s);
        char *file_text = (char*) mmap(NULL, s.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        char *line_begin = strstr(file_text, dest);
        char *line_end = line_begin;
        while(*line_end != '\n' && line_end) line_end++;
        if(*line_end) line_end++;
        int bytes = 0;
        while (line_end < file_text + s.st_size)
        {
            *line_begin = *line_end;
            line_begin++;
            line_end++;
            bytes++;
        }
        if(!bytes) ftruncate(fd, s.st_size - (line_end - line_begin)/sizeof(char));
        else ftruncate(fd, s.st_size - bytes);
        //munmap(file_text, )
        close(fd);
        return;
    }
    printf("there is no such file %s in trashbin\n", filename);
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