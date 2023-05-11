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
void input_file_path(char *new_path);
void compute_paths();
int find_last_slash(const char *s);
int find_first_slash(const char *s);
int check_trash_log();
void delete_line_from_trashlog_file(const char *dest);
void delete_file_permanently(const char *filename);
void init();

char cwd[MAX_PATH_LEN];
char trash_log_path[MAX_PATH_LEN];
char trash_path[MAX_PATH_LEN + 10];
char home_path[MAX_PATH_LEN];

int main()
{
    init();
    char option;
    while(true)
    {
        input_option(&option);
        switch (option)
        {
            case 'l':
            {
                // TODO: fix bug with files that have equal names
                // add clear trash && delete file permanently
                // wrap everything into functions
                // bug if filenames are equal, they will be logged, but first file will be replaced with the second one
                // fix bug when files are listed but can't be restored from trash
                // implement putting files into trash with .. actions
                list_trash_files();
                break;
            }
            case 'd':
            {
                char path_name[MAX_PATH_LEN];
                printf("input name of file you want to delete permanently:\n");
                input_file_path(path_name);
                delete_file_permanently(path_name);
                break;
            }
            case 'c':
            {
                FILE *f = fopen(trash_log_path, "w");
                if(!f)
                {
                    perror("fopen");
                    exit(errno);
                }
                fclose(f);

                DIR *dir = opendir(trash_path);
                struct dirent *read_dir;
                if (!dir && errno)
                {
                    perror("opendir");
                    errno = 0;
                    break;
                }
                int files_count = 0;
                while((read_dir = readdir(dir)))
                {
                    if (!(strcmp(read_dir->d_name, ".") && strcmp(read_dir->d_name, "..")))
                        continue;
                    char file_path[MAX_PATH_LEN];
                    strcpy(file_path, trash_path);
                    strcat(file_path, "/");
                    strcat(file_path, read_dir->d_name);
                    if(remove(file_path))
                    {
                        perror("remove");
                    }
                }
                closedir(dir);
                printf("trashbin was succesfully cleared\n");
                println();
                break;
            }
            case 'p':
            {
                char old_path_name[MAX_PATH_LEN];
                

                char path_name[MAX_FILENAME_LEN];
                printf("input name of file you want to put into trashbin:\n");
                fflush(stdin);
                fgets(path_name, sizeof(path_name)/sizeof(*path_name), stdin);
                fgets(path_name, sizeof(path_name)/sizeof(*path_name), stdin);
                path_name[strlen(path_name) - 1] = '\0';

                char new_path[MAX_PATH_LEN];
                int index;
                if(*path_name != '/')
                {
                    strcpy(old_path_name, getcwd(cwd, sizeof(cwd)));
                    strcat(old_path_name, "/");
                    strcat(old_path_name, path_name + index);
                    index = find_first_slash(path_name);
                }
                else 
                {
                    index = find_last_slash(path_name);
                    strcpy(old_path_name, path_name);
                }
                strcpy(new_path, trash_path);
                strcat(new_path, "/");
                strcat(new_path, path_name + index);
                if(rename(old_path_name, new_path))
                {
                    fprintf(stderr, "file with this name doesn't exist\n");
                    fprintf(stderr, "------------------------------------------------\n");
                    break;
                }   
                logger(home_path, old_path_name);
                println();
                break;
            }
            
            case 'r':
            {
                if(check_trash_log() == -1) break;
                char new_path[MAX_PATH_LEN];
                printf("input name of file you want to restore:\n");
                input_file_path(new_path);
                restore_file(new_path);
                break;
            }
            case 'm':
            {
                print_menu();
                break;
            }
            case 'q':
            {
                exit(0);
                break;
            }
        }   
    }
    return 0;
}

void init()
{
    println();
    print_menu();
    compute_paths();
    mkdir(trash_path, 0755);
}

void input_file_path(char *new_path)
{
    char path_name[MAX_FILENAME_LEN];
    fflush(stdin);
    fgets(path_name, sizeof(path_name)/sizeof(*path_name), stdin);
    fgets(path_name, sizeof(path_name)/sizeof(*path_name), stdin);
    path_name[strlen(path_name) - 1] = '\0';

    strcpy(new_path, trash_path);
    strcat(new_path, "/");
    strcat(new_path, path_name);
}

int check_trash_log()
{
    int fd = open(trash_log_path, O_RDWR);
    if(!fd)
    {
        perror("open");
        exit(errno);
    }
    struct stat s;
    fstat(fd, &s);
    close(fd);
    if(!s.st_size) 
    {
        fprintf(stderr, "trash.log is empty, put files to trashbin first\n");
        fprintf(stderr, "------------------------------------------------\n");
        return -1;
    }
    return 0;
}

void println()
{
    printf("------------------------------------------------\n");
}

void compute_paths()
{
    strcpy(home_path, getenv("HOME"));
    if(!(*home_path))
    {
        fprintf(stderr, "ERROR: can't get home_path environment\n");
        exit(1);
    }
    sprintf(trash_path, "%s/trash", home_path);
    strcpy(trash_log_path, home_path);
    strcat(trash_log_path, "/trash.log");    
}

void delete_file_permanently(const char *filename)
{
    FILE *f = fopen(trash_log_path, "r+");
    if(!f)
    {
        perror("fopen");
        exit(errno);
    }
    char file_path[MAX_PATH_LEN];
    char dest[MAX_PATH_LEN];
    fseek(f, 0, SEEK_SET);
    while(!feof(f))
    {
        if(!fgets(file_path, MAX_PATH_LEN, f)) break;
        char *path_end = strstr(file_path, " was moved to /home/");
        if(path_end)
        {
            strncpy(dest, file_path, ((path_end - file_path)/sizeof(char)));
        }
        int dest_index = find_last_slash(dest);
        int filename_index = find_last_slash(filename);
        if(strcmp(dest + dest_index, filename + filename_index)) continue;
        if(remove(filename))
        {
            perror("remove");
            exit(errno);  
        }
        char dest_path[MAX_PATH_LEN];
        strncpy(dest_path, dest, dest_index);
        printf("%s was successfully deleted permanently from trashbin to %s\n",filename, dest_path);
        println();
        delete_line_from_trashlog_file(dest);
        fclose(f);
        return;
    }
    fclose(f);
    printf("there is no such file in trashbin\n");
    println();
}

void restore_file(const char *filename)
{
    FILE *f = fopen(trash_log_path, "r+");
    if(!f)
    {
        perror("fopen");
        exit(errno);
    }
    char file_path[MAX_PATH_LEN];
    char dest[MAX_PATH_LEN];
    fseek(f, 0, SEEK_SET);
    while(!feof(f))
    {
        if(!fgets(file_path, MAX_PATH_LEN, f)) break;
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
        printf("%s was successfully deleted permanently from trashbin\n",filename);
        println();
        delete_line_from_trashlog_file(dest);
        fclose(f);
        return;
    }
    fclose(f);
    printf("there is no such file in trashbin\n");
    println();
}

void delete_line_from_trashlog_file(const char *dest)
{
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
    int new_file_size = s.st_size - (line_end - line_begin);
    while (line_end < file_text + s.st_size)
    {
        *line_begin = *line_end;
        line_begin++;
        line_end++;
    }
    ftruncate(fd, new_file_size);
    munmap(file_text, s.st_size);
    close(fd);
}

void list_trash_files()
{
    DIR *dir = opendir(trash_path);
    struct dirent *read_dir;
    if (!dir && errno)
    {
        perror("opendir");
        errno = 0;
        return;
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
        printf("%s\n", read_dir->d_name);
        files_count++;
    }
    if(!files_count)
    {
        fprintf(stderr, "trash is empty\n");
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
        if(tmp == 'l' || tmp == 'r' || tmp == 'd' || tmp == 'm' || tmp == 'c' || tmp == 'p' || tmp == 'q') break;
    }
    *option = tmp;
    println();
}

void print_menu()
{
    printf("menu:\n");
    printf("q - exit\nl - list trash files\np - put file into trash\n");
    printf("c - clear trashbin\nd - delete file from trash permanently\n");
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

void logger(const char *home_path, const char *path_name)
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
        perror("fopen");
        exit(errno);
    }
    fseek(log, 0, SEEK_END);
    fprintf(log, "%s was moved to %s/trash by user on %s", path_name, home_path, asctime(timeinfo));
	printf("%s was succesfully moved to %s/trash by user\n", path_name, home_path);
    fclose(log);
}

// tm FILE time_t stat DIR dirent