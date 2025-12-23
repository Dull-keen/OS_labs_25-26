#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int pipefd[2];
    pid_t pid;
    const char *message = "Hello from parent process!";
    char buffer[1024];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid > 0)
    {
        close(pipefd[0]);

        write(pipefd[1], message, strlen(message) + 1);
        close(pipefd[1]);

        wait(NULL);
    }
    else
    {
        close(pipefd[1]);

        ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer));
        if (bytes_read > 0)
        {
            printf("Child received: %s\n", buffer);
        }

        close(pipefd[0]);
        _exit(0);
    }

    return 0;
}