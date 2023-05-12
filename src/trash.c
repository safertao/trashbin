#define _GNU_SOURCE
#define MAX_PATH_LEN 4096 // максимальная длина пути
#define MAX_FILENAME_LEN 256 // максимальная длина имени файла

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

void println(); // функция вывод линии из '-'
void print_menu(); // функция вывода меню на экран
void input_option(char *option); // функция ввода опции с проверками
void list_trash_files(); // функция вывод всех файлов, находящихся в корзине
// функция восстановления файла из корзины
void restore_file(const char *filename);
// функция для записи данных в журнал
void logger(const char *path_name, const char *new_path); 
void input_file_path(char *new_path); // функция для ввода имени файла
void compute_paths(); // функция для вычисления путей
// функции для нахождения первого и последнего '/', начиная с start_pos
int find_last_slash(const char *s, int start_pos); 
int find_first_slash(const char *s, int start_pos); 
int check_trash_log(); // функция для проверки заполненности корзины
// функция для удаления строки из файла журнала
void delete_line_from_trashlog_file(const char *dest); 
// функция для перманентного удаленного файла
void delete_file_permanently(const char *filename);   
void init();  // функция для инициализации данных
void clear_trash_log(); // функция для очистки файла журнала
void clear_trashbin(); // функция для очистки корзины
// функция для ввода полного или относительного пути
void input_full_or_relative_file_path(char *old_path_name, char *path_name, 
char *new_path);  
// функция для перемещения файла в корзину с проверками
void put_file_to_trash_with_checks(char *old_path_name, char *new_path); 

char cwd[MAX_PATH_LEN] = {0}; // рабочий каталог
char trash_log_path[MAX_PATH_LEN] = {0}; // путь до файла-журнала
char trash_path[MAX_PATH_LEN + 10] = {0}; // путь до корзины
char home_path[MAX_PATH_LEN] = {0}; // домашний каталог

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
                list_trash_files();
                break;
            }
            case 'd':
            {
                if(check_trash_log() == -1) break;
                char path_name[MAX_PATH_LEN];
                printf("input name of file you want to delete permanently:\n");
                input_file_path(path_name);
                delete_file_permanently(path_name);
                break;
            }
            case 'c':
            {
                if(check_trash_log() == -1) break;
                clear_trash_log();
                clear_trashbin();
                printf("trashbin was succesfully cleared\n");
                println();
                break;
            }
            case 'p':
            {
                char old_path_name[MAX_PATH_LEN] = {0};
                char path_name[MAX_FILENAME_LEN] = {0};
                char new_path[MAX_PATH_LEN] = {0};
                input_full_or_relative_file_path(old_path_name, path_name, 
                new_path);
                put_file_to_trash_with_checks(old_path_name, new_path);
                println();
                break;
            }
            
            case 'r':
            {
                if(check_trash_log() == -1) break;
                char new_path[MAX_PATH_LEN] = {0};
                printf("input name of file you want to restore:\n");
                input_file_path(new_path);
                restore_file(new_path);
                println();
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

void put_file_to_trash_with_checks(char *old_path_name, char *new_path)
{
    char tmp_path[MAX_PATH_LEN + 20];
    strcpy(tmp_path, new_path);
    FILE* f = fopen(new_path, "r");
    if(!f)
    {
        if(rename(old_path_name, new_path))
        {
            fprintf(stderr, "file with this name doesn't exist\n");
            return;
        }   
    }
    else
    {
        int value = 0;
        // подбираем имена файлу, пока есть коллизии
        do
        {
            value++;
            memset(tmp_path, 0, MAX_FILENAME_LEN);
            sprintf(tmp_path, "%s(%d)", new_path, value);
        } while(fopen(tmp_path, "r"));
        if(rename(old_path_name, tmp_path))
        {
            fprintf(stderr, "file with this name doesn't exist\n");
            return;
        }   
        fclose(f);
    }
    logger(old_path_name, tmp_path);
}

void input_full_or_relative_file_path(char *old_path_name, char *path_name, 
char *new_path)
{
    printf("input name of file you want to put into trashbin:\n");
    fflush(stdin);
    fgets(path_name, MAX_FILENAME_LEN, stdin);
    fgets(path_name, MAX_FILENAME_LEN, stdin);
    path_name[strlen(path_name) - 1] = '\0';

    int index = 0;
    if(*path_name == '.' && *(path_name + 1) == '.')
    {
        char tmp_path_name[MAX_PATH_LEN];
        // берем рабочий каталог, копируем имя предыдущего каталога
        strcpy(tmp_path_name, cwd);
        index = find_last_slash(tmp_path_name, strlen(tmp_path_name));
        strncpy(old_path_name, tmp_path_name, index - 1);
        strcat(old_path_name, path_name + 2); // пропускаем .. и получаем имя  
        index = find_last_slash(path_name, strlen(path_name));
    }
    else if(*path_name == '.' && *(path_name + 1) == '/')
    {
        strcpy(old_path_name, cwd);
        strcat(old_path_name, "/");
        strcat(old_path_name, path_name + 2); // пропускаем ./
        index = find_first_slash(path_name, 0);

    }
    else if(*path_name != '/')
    {
        strcpy(old_path_name, cwd);
        strcat(old_path_name, "/");
        strcat(old_path_name, path_name);
        index = find_first_slash(path_name, 0);
    }
    else 
    {
        // полный путь от корня
        index = find_last_slash(path_name, strlen(path_name));
        strcpy(old_path_name, path_name);
    }
    strcpy(new_path, trash_path);
    strcat(new_path, "/");
    strcat(new_path, path_name + index);
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
        fprintf(stderr, "trash is empty, put files to it first\n");
        println();
        return -1;
    }
    return 0;
}

void println()
{
    printf("------------------------------------------------\n");
}

void delete_file_permanently(const char *filename)
{
    FILE *f = fopen(trash_log_path, "r+");
    if(!f)
    {
        perror("fopen");
        exit(errno);
    }
    fseek(f, 0, SEEK_SET);
    char file_path[MAX_PATH_LEN];
    char dest[MAX_PATH_LEN];
    while(!feof(f))
    {
        memset(file_path, 0, MAX_PATH_LEN);
        memset(dest, 0, MAX_PATH_LEN);
        if(!fgets(file_path, MAX_PATH_LEN, f)) break;
        // становимся в нужную позицию, рассчитываем индексы для имен файлов
        char *path_end = strstr(file_path, " was renamed to ");
        if(path_end)
        {
            strncpy(dest, file_path, ((path_end - file_path)/sizeof(char)));
        }
        int dest_after_index = find_last_slash(dest, strlen(dest));
        int filename_index = find_last_slash(filename, strlen(filename));
        if(strcmp(dest + dest_after_index, filename + filename_index)) 
            continue;
        // удаляем найденный файл
        if(remove(filename))
        {
            perror("remove");
            exit(errno);  
        }
        printf("%s was deleted permanently from trashbin\n",filename);
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
    fseek(f, 0, SEEK_SET);
    char file_path[MAX_PATH_LEN];
    char dest[MAX_PATH_LEN];   
    while(!feof(f))
    {
        memset(file_path, 0, MAX_PATH_LEN);
        memset(dest, 0, MAX_PATH_LEN);
        if(!fgets(file_path, MAX_PATH_LEN, f)) break;
        char *path_end = strstr(file_path, " was renamed to ");
        // становимся в правильную позицию, рассчитываем индексы для сравнения
        // имен файлов
        if(path_end)
        {
            strncpy(dest, file_path, ((path_end - file_path)/sizeof(char)));
        }
        int dest_after_index = find_last_slash(dest, strlen(dest));
        int filename_index = find_last_slash(filename, strlen(filename));
        // сравниваем прошлое имя и текущее имя с введенным
        if(strcmp(dest + dest_after_index, filename + filename_index))
        {
            int last_idx = find_last_slash(file_path, strlen(file_path));
            int tmp = last_idx;
            int count = 0;
            while(*(file_path + tmp) != ' ')
            {
                tmp++;
                count++;
            }
            if(strncmp(filename + filename_index, file_path + last_idx, count))
                continue;
        }
        fclose(f);
        FILE *end = fopen(dest, "r"); 
        char tmp_path[MAX_PATH_LEN + 20];
        if(!end)
        {
            if(rename(filename, dest))
            {
                perror("rename");
                println();
                break;
            }   
            printf("%s was restored from trashbin and renamed to %s\n", 
            filename, dest);
        }
        else
        {
            int value = 0;
            // цикл подбора имен для избегания коллизий
            do
            {
                value++;
                memset(tmp_path, 0, MAX_FILENAME_LEN);
                sprintf(tmp_path, "%s(%d)", dest, value);
            } while(fopen(tmp_path, "r"));
            if(rename(filename, tmp_path))
            {
                perror("rename");
                println();
                break;
            }   
            printf("%s was restored from trashbin and renamed to %s because of" 
            " collisions\n", filename, tmp_path);
            fclose(end);
        }
        delete_line_from_trashlog_file(dest);
        return;
    }
    fclose(f);
    printf("there is no such file in trashbin\n");
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
    fstat(fd, &s); // для размера файла
    // отображаем файл в file_text, находим вхождение искомой строки
    char *file_text = (char*) mmap(NULL, s.st_size, PROT_READ | PROT_WRITE, 
    MAP_SHARED, fd, 0);
    char *line_begin = strstr(file_text, dest);
    char *line_end = line_begin;
    while(*line_end != '\n' && line_end) line_end++; 
    if(*line_end) line_end++;
    int new_file_size = s.st_size - (line_end - line_begin);
    // нашли '\n' или конец файла, новую длину файла
    // сдвигаем всю информацию влево
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

void clear_trashbin()
{
    struct dirent *read_dir;
    DIR *dir = opendir(trash_path);
    if (!dir && errno)
    {
        perror("opendir");
        errno = 0;
        return;
    }
    while((read_dir = readdir(dir)))// читаем все файлы изtrash_path,
    // кроме . и .. и удаляем их 
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
}

void clear_trash_log()
{
    FILE *f = fopen(trash_log_path, "w");
    if(!f)
    {
        perror("fopen");
        exit(errno);
    }
    fclose(f);
}

void compute_paths()
{
    strcpy(home_path, getenv("HOME"));
    if(!(*home_path))
    {
        fprintf(stderr, "ERROR: can't get home_path environment\n");
        exit(1);
    }
    sprintf(trash_path, "%s/trash", home_path);// вычисляем все пути
    strcpy(trash_log_path, home_path);
    strcat(trash_log_path, "/trash.log");  
    getcwd(cwd, sizeof(cwd));  
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
    while((read_dir = readdir(dir)))// читаем все файлы из trash_path, выводим
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
        if(tmp == 'l' || tmp == 'r' || tmp == 'd' || tmp == 'm' 
        || tmp == 'c' || tmp == 'p' || tmp == 'q') break;
    }
    *option = tmp;
    println();
}

void print_menu()
{
    printf("menu:\n");
    printf("q - exit\nl - list trash files\np - put file into trash\n");
    printf("r - restore file from trash\nd - delete file from trash ");
    printf("permanently\nc - clear trashbin\nm - print menu\n");
    println();
}

int find_last_slash(const char *s, int start_pos)
{
    int index = 0;
    for(int i = start_pos - 1; i > 0 && s[i]; i--)
    {
        if(s[i] == '/') 
        {
            index = i + 1;
            break;
        }
    }
    return index;
}

int find_first_slash(const char *s, int start_pos)
{
    int index = 0;
    for(int i = start_pos; s[i]; i++) 
    {
        if(s[i] == '/') 
        {
            index = i + 1;
            break;
        }
    }
    return index;
}

void logger(const char *path_name, const char *new_path)
{
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime(&rawtime); // переводим в локальное время
    FILE *log = fopen(trash_log_path, "a");  
    if(!log)
    {
        perror("fopen");
        exit(errno);
    }
    fseek(log, 0, SEEK_END);         
    fprintf(log, "%s was renamed to %s by user on %s", path_name, new_path, 
    asctime(timeinfo));
	printf("%s was renamed to %s by user\n", path_name, new_path);
    fclose(log);
}

void init()
{
    println();
    print_menu();
    compute_paths();
    mkdir(trash_path, 0755);
}