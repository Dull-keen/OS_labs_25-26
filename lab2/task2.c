#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int 
main(void)
{
    int choice;
    printf("Выберите пример(1/2/3/4): ");
    if (scanf("%d", &choice) != 1) 
    {
        printf("Ошибка ввода.\n");
        return 1;
    }


    if (choice == 1)
    {
        printf("\nПечатается PID дочернего процесса (отец) или 0 (сын) в случайном порядке:\n\n");
        int pid;
        pid=fork();
        printf("%d\n", pid);
        return 0;
    }
    else if (choice == 2)
    {
        printf("\nПечатается 'Hi' 6 раз в случайном порядке (2 раза отец, 2 раза сын, 1 раз другой сын, 1 раз внук):\n\n");
        fork();
        printf("Hi\n");
        fork();
        printf("Hi\n");
        return 0;
    }
    else if (choice == 3)
    {
        printf("\nПечатается 'forked!' и (PID/0) 10 раз в случайном порядке, причем ровно 5 раз будет 0 (на листьях дерева процессов):\n\n");
        if (fork() || fork()) fork();
        printf("forked! %d ", fork());
        return 0;
    }
    else if (choice == 4)
    {
        printf("\nПечатается число 52 ровно 7 раз в случайном порядке (1 отец, 2 сына, 2 внука, 2 правнука):\n\n");
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