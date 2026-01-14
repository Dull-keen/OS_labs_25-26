#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct VisitedFile
{
    dev_t dev;
    ino_t ino;
    struct VisitedFile *next;
} VisitedFile;

VisitedFile *visited_head = NULL;

int 
mark_visited(dev_t dev, ino_t ino)
{
    VisitedFile *current = visited_head;
    while (current != NULL)
    {
        if (current->dev == dev && current->ino == ino)
        {
            return 0; 
        }
        current = current->next;
    }

    VisitedFile *new_node = malloc(sizeof(VisitedFile));
    if (!new_node)
    {
        perror("malloc");
        exit(1);
    }
    new_node->dev = dev;
    new_node->ino = ino;
    new_node->next = visited_head;
    visited_head = new_node;
    return 1;
}

const char *
get_extension(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
    {
        return ""; 
    }
    return dot + 1;
}

void 
process_dir(const char *path, int current_depth, int recmin, int recmax)
{
    if (current_depth > recmax)
    {
        return;
    }

    DIR *dir = opendir(path);
    if (!dir)
    {
        // Если не удалось открыть (например, нет прав), просто пропускаем
        // Но если это стартовый аргумент, можно вывести ошибку, 
        // здесь для рекурсии лучше просто return.
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char full_path[PATH_MAX];
        int n = snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (n < 0 || n >= sizeof(full_path))
        {
            continue; 
        }

        struct stat st;

        if (lstat(full_path, &st) == -1)
        {
            continue;
        }


        if (current_depth >= recmin && current_depth <= recmax)
        {

            if (mark_visited(st.st_dev, st.st_ino))
            {
                printf("Name: %-20s | Ext: %-5s | Inode: %lu\n", 
                       entry->d_name, 
                       get_extension(entry->d_name), 
                       (unsigned long)st.st_ino);
            }
        }

        if (S_ISDIR(st.st_mode))
        {
            process_dir(full_path, current_depth + 1, recmin, recmax);
        }
    }

    closedir(dir);
}

void 
free_visited(void)
{
    VisitedFile *current = visited_head;
    while (current != NULL)
    {
        VisitedFile *temp = current;
        current = current->next;
        free(temp);
    }
}

int 
main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <recmin> <recmax> <dir1> [dir2 ...]\n", argv[0]);
        return 1;
    }

    int recmin = atoi(argv[1]);
    int recmax = atoi(argv[2]);

    if (recmin < 0 || recmax < 0 || recmin > recmax)
    {
        fprintf(stderr, "Invalid recmin/recmax values\n");
        return 1;
    }

    for (int i = 3; i < argc; i++)
    {
        char *path = argv[i];
        size_t len = strlen(path);
        if (len > 1 && path[len - 1] == '/')
        {
            path[len - 1] = '\0';
        }

        process_dir(path, 1, recmin, recmax);
    }

    free_visited();
    return 0;
}