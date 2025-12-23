#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    const char *fifo_path = "/tmp/my_named_pipe";
    int length_counts[101] = {0};
    int fd;

    mkfifo(fifo_path, 0666);

    fd = open(fifo_path, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        int str_len;
        ssize_t res = read(fd, &str_len, sizeof(int));

        if (res <= 0)
        {
            break;
        }

        char *buffer = malloc(str_len + 1);
        read(fd, buffer, str_len);
        buffer[str_len] = '\0';

        if (str_len >= 1 && str_len <= 100)
        {
            length_counts[str_len]++;
            printf("Received string of length %d. Total for this length: %d\n", 
                   str_len, length_counts[str_len]);

            if (length_counts[str_len] >= 5)
            {
                printf("Condition met: 5 strings of length %d received. Exiting.\n", str_len);
                free(buffer);
                break;
            }
        }
        free(buffer);
    }

    close(fd);
    unlink(fifo_path);

    return 0;
}