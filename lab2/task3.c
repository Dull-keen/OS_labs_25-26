#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096

const char *SEARCH_STR = "target\nstring";

int 
count_occurrences_in_file(const char *filename, const char *str)
{
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        perror("fopen");
        return -1;
    }

    size_t search_len = strlen(str);
    if (search_len == 0)
    {
        fclose(f);
        return 0;
    }

    size_t current_buf_size = BUFFER_SIZE;
    if (search_len >= current_buf_size)
    {
        current_buf_size = search_len * 2;
    }

    char *buffer = malloc(current_buf_size + 1);
    if (!buffer)
    {
        perror("malloc");
        fclose(f);
        return -1;
    }

    int count = 0;
    size_t overlap = search_len - 1;
    size_t bytes_in_buffer = 0;

    size_t read_count = fread(buffer, 1, current_buf_size, f);
    bytes_in_buffer = read_count;
    buffer[bytes_in_buffer] = '\0';

    while (bytes_in_buffer > 0)
    {
        char *p = buffer;
        char *found_ptr;

        while ((found_ptr = strstr(p, str)) != NULL)
        {
            count++;
            p = found_ptr + search_len;
        }

        if (feof(f))
        {
            break;
        }

        size_t bytes_to_keep = (bytes_in_buffer < overlap) ? bytes_in_buffer : overlap;

        memmove(buffer, buffer + bytes_in_buffer - bytes_to_keep, bytes_to_keep);

        size_t bytes_to_read = current_buf_size - bytes_to_keep;
        read_count = fread(buffer + bytes_to_keep, 1, bytes_to_read, f);

        bytes_in_buffer = bytes_to_keep + read_count;
        buffer[bytes_in_buffer] = '\0';

        if (read_count == 0)
        {
            break;
        }
    }

    free(buffer);
    fclose(f);
    return count;
}

void 
fork_tree(int n)
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

int 
main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filelist>\n", argv[0]);
        return 1;
    }

    const char *list_path = argv[1];
    FILE *list = fopen(list_path, "r");
    if (list == NULL)
    {
        perror("fopen list");
        return 1;
    }

    char *filepath = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int child_count = 0;

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
            int count = count_occurrences_in_file(filepath, SEARCH_STR);
            if (count > 0)
            {
                printf("%s: %d\n", filepath, count);
                free(filepath);
                fclose(list);
                _exit(1);
            }
            else
            {
                free(filepath);
                fclose(list);
                _exit(0);
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

    for (int i = 0; i < child_count; i++)
    {
        int status;
        pid_t ended = waitpid(-1, &status, 0);
        if (ended == -1)
        {
            perror("waitpid");
            continue;
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
        printf("String not found. Starting fork tree.\n");
        size_t n_procs = strlen(SEARCH_STR);
        fork_tree((int)n_procs);
        if (n_procs > 0)
        {
            pause();
        }
    }

    return 0;
}