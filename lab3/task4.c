#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define FIFO_FMT "/tmp/joseph_fifo_%d"

typedef struct 
{
    int counter;
} token_t;

void 
get_fifo_name(char *buffer, int index) 
{
    sprintf(buffer, FIFO_FMT, index);
}

void 
child_routine(int id, int n, int k) 
{
    // Отключаем буферизацию, чтобы логи не перемешивались
    setvbuf(stdout, NULL, _IONBF, 0);

    char my_fifo[64];
    get_fifo_name(my_fifo, id);

    while (1) 
    {
        token_t token;
        int fd_in = open(my_fifo, O_RDONLY);
        if (fd_in < 0) exit(1);

        if (read(fd_in, &token, sizeof(token_t)) <= 0) 
        {
            close(fd_in);
            exit(0);
        }
        close(fd_in);

        token.counter--;

        int next_id = (id + 1) % n;
        char next_fifo[64];
        int fd_out = -1;

        if (token.counter == 0) 
        {
            // ПРОЦЕСС ВЫБЫВАЕТ
            printf("[Order Log] Process #%d (PID: %d) is OUT at this step.\n", id + 1, getpid());
            
            token.counter = k; 
            while (1) 
            {
                if (next_id == id) break; 
                get_fifo_name(next_fifo, next_id);
                fd_out = open(next_fifo, O_WRONLY | O_NONBLOCK);
                if (fd_out >= 0) break;
                next_id = (next_id + 1) % n;
            }
            
            if (fd_out >= 0) 
            {
                fcntl(fd_out, F_SETFL, fcntl(fd_out, F_GETFL) & ~O_NONBLOCK);
                write(fd_out, &token, sizeof(token_t));
                close(fd_out);
            }
            
            unlink(my_fifo);
            // Небольшая пауза, чтобы Parent успел зафиксировать смерть в wait() до того, 
            // как следующий процесс тоже вызовет exit()
            usleep(15000); 
            exit(0); 
        } 
        else 
        {
            // ПРОЦЕСС ПЕРЕДАЕТ ХОД
            while (1) 
            {
                if (next_id == id) 
                {
                    printf("[Order Log] Process #%d (PID: %d) is the LAST SURVIVOR!\n", id + 1, getpid());
                    unlink(my_fifo);
                    usleep(15000);
                    exit(0);
                }
                get_fifo_name(next_fifo, next_id);
                fd_out = open(next_fifo, O_WRONLY | O_NONBLOCK);
                if (fd_out >= 0) break;
                next_id = (next_id + 1) % n;
            }

            int flags = fcntl(fd_out, F_GETFL);
            fcntl(fd_out, F_SETFL, flags & ~O_NONBLOCK);
            write(fd_out, &token, sizeof(token_t));
            close(fd_out);
        }
    }
}

int 
main(int argc, char *argv[]) 
{
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc < 3) 
    { 
        fprintf(stderr, "Usage: %s <n> <k>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int k = atoi(argv[2]);

    if (n <= 1 || k <= 1 || k >= n) 
    {
        fprintf(stderr, "Error: constraints n > 1 and 1 < k < n\n");
        return 1;
    }

    pid_t *pids = malloc(n * sizeof(pid_t));

    for (int i = 0; i < n; i++) 
    {
        char name[64];
        get_fifo_name(name, i);
        unlink(name);
        mkfifo(name, 0666);
    }

    for (int i = 0; i < n; i++) 
    {
        pid_t p = fork();
        if (p == 0) 
        {
            free(pids);
            child_routine(i, n, k);
            exit(0);
        }
        pids[i] = p;
    }

    token_t start_token = { k };
    char start_fifo[64];
    get_fifo_name(start_fifo, 0);

    printf("Modeling problem: n=%d, k=%d\n", n, k);
    printf("------------------------------------------\n");

    int fd = open(start_fifo, O_WRONLY);
    write(fd, &start_token, sizeof(token_t));
    close(fd);

    pid_t *term_order = malloc(n * sizeof(pid_t));
    for (int i = 0; i < n; i++) 
    {
        term_order[i] = wait(NULL);
    }

    for (int i = 0; i < n; i++) 
    {
        char name[64];
        get_fifo_name(name, i);
        unlink(name);
    }
    free(pids);
    free(term_order);

    return 0;
}