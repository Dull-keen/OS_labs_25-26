#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

int main()
{
    const char *fifo_path = "/tmp/my_named_pipe";
    int fd;

    srand(time(NULL));

    fd = open(fifo_path, O_WRONLY);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    while (1)
    {
        int str_len = rand() % 100 + 1;
        char *str = malloc(str_len);

        for (int i = 0; i < str_len; i++)
        {
            str[i] = 'a' + (rand() % 26);
        }

        if (write(fd, &str_len, sizeof(int)) == -1)
        {
            free(str);
            break;
        }

        if (write(fd, str, str_len) == -1)
        {
            free(str);
            break;
        }

        free(str);
        usleep(100000); 
    }

    close(fd);
    return 0;
}