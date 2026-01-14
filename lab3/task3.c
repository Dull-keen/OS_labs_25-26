#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

int 
NOD(int a, int b) 
{
    while (b) 
    {
        a %= b;
        int t = a; a = b; b = t;
    }
    return a;
}

int 
main(void) 
{
    int fd1[2]; // pipe parent to child (transfer n)
    int fd2[2]; // pipe child to parent (transfer result)

    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        perror("Pipe failed");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) 
    {
        perror("Fork failed");
        return 1;
    }

    if (pid > 0) 
    {
        close(fd1[0]);
        close(fd2[1]);

        srand(time(NULL));
        
        while (1) 
        {
            int n = rand() % 100 + 2; 
            printf("[Parent] Generated n = %d. Sending to child...\n", n);
            
            write(fd1[1], &n, sizeof(int));

            int count;
            read(fd2[0], &count, sizeof(int));
            
            int *res = malloc(count * sizeof(int));
            read(fd2[0], res, count * sizeof(int));

            printf("[Parent] Received reduced residue system (size %d): ", count);
            for(int i = 0; i < count; i++) printf("%d ", res[i]);
            printf("\n\n");

            free(res);
            sleep(1); // Don't flood the output too quickly
        }
    } else 
    { 
        close(fd1[1]); 
        close(fd2[0]);

        int n;
        while (read(fd1[0], &n, sizeof(int)) > 0) 
        {
            int temp_res[n];
            int count = 0;

            for (int i = 1; i <= n; i++) 
            {
                if (NOD(i, n) == 1) 
                {
                    temp_res[count++] = i;
                }
            }

            write(fd2[1], &count, sizeof(int));
            write(fd2[1], temp_res, count * sizeof(int));
        }
    }

    return 0;
}