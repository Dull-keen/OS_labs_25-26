#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Подсчёт количества вхождений подстроки str в файле filename
int count_occurrences_in_file(const char *filename, const char *str)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        perror("fopen");
        return -1;
    }

    size_t len = strlen(str);
    if (len == 0)
    {
        fclose(f);
        return 0;
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int count = 0;

    while ((linelen = getline(&line, &linecap, f)) != -1)
    {
        char *p = line;
        while ((p = strstr(p, str)) != NULL)
        {
            count++;
            p += len;
        }
    }

    free(line);
    fclose(f);
    return count;
}

// Строим сбалансированное дерево процессов из n процессов
void fork_tree(int n)
{
    if (n <= 1)
    {
        return;
    }

    int left = (n - 1) / 2;
    int right = (n - 1) - left;

    pid_t pid;

    if (left > 0)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
        }
        else if (pid == 0)
        {
            fork_tree(left);
            pause();
            _exit(0);
        }
    }

    if (right > 0)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
        }
        else if (pid == 0)
        {
            fork_tree(right);
            pause();
            _exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <filelist> <string>\n", argv[0]);
        return 1;
    }

    const char *list_path = argv[1];
    const char *str = argv[2]; // не аргументом терминала. пустая строка даёт (лен+1)

    FILE *list = fopen(list_path, "r");
    if (list == NULL)
    {
        perror("fopen <filelist>");
        return 1;
    }

    char *filepath = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int child_count = 0;

    // Читаем файл со списком путей и для каждого пути создаём процесс
    while ((linelen = getline(&filepath, &linecap, list)) != -1)
    {
        if (linelen > 0 && filepath[linelen - 1] == '\n')
        {
            filepath[linelen - 1] = '\0';
        }

        if (filepath[0] == '\0')
        {
            continue;
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            continue;
        }
        else if (pid == 0)
        {
            int count = count_occurrences_in_file(filepath, str);
            if (count > 0)
            {
                printf("%s: %d\n", filepath, count);
                _exit(1);  // код 1 — строка найдена в этом файле
            }
            else
            {
                _exit(0);  // код 0 — не найдена
            }
        }
        else
        {
            child_count++;
        }
    }

    free(filepath);
    fclose(list);

    int any_found = 0;

    // Ждём всех потомков и смотрим их код возврата
    for (int i = 0; i < child_count; i++)
    {
        int status;
        pid_t ended = waitpid(-1, &status, 0);
        if (ended == -1)
        {
            perror("waitpid");
            break;
        }

        if (WIFEXITED(status))
        {
            int code = WEXITSTATUS(status);
            if (code == 1)
            {
                any_found = 1;
            }
        }
    }

    if (!any_found)
    {
        printf("Строка \"%s\" не найдена ни в одном файле\n", str);
        fork_tree((int)strlen(str));
        pause();  // чтобы главный процесс тоже не завершился мгновенно
    }

    return 0;
}
