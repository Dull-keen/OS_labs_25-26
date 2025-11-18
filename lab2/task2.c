#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int choice;
    printf("Выберите пример(1/2/3/4): ");
    scanf("%d", &choice);


    if (choice == 1)
    {
        int pid;
        pid=fork();
        printf("%d\n", pid);
        return 0;
    }
    else if (choice == 2)
    {
        fork();
        printf("Hi\n");
        fork();
        printf("Hi\n");
        return 0;
    }
    else if (choice == 3)
    {
        if (fork() || fork()) fork();
        printf("forked! %d", fork());
        return 0;
    }
    else if (choice == 4)
    {
        if (fork() && (!fork()))
        {
        if (fork() || fork()) fork();
        }
        printf("52\n");
        return 0;
    }
    else
    {
        printf("Неверный выбор.\n");
        return 1;
    }

}