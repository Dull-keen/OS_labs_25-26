#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() 
{
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgrp = getpgrp();
    uid_t ruid = getuid();
    gid_t rgid = getgid();
    uid_t euid = geteuid();
    gid_t egid = getegid();

    printf("PID текущего процесса:               %d\n", pid);
    printf("PID родительского процесса:          %d\n", ppid);
    printf("ID группы процесса (pgrp):           %d\n", pgrp);
    printf("Реальный UID владельца (ruid):       %u\n", ruid);
    printf("Реальный GID владельца (rgid):       %u\n", rgid);
    printf("Эффективный UID владельца (euid):    %u\n", euid);
    printf("Эффективный GID владельца (egid):    %u\n", egid);

    return 0;
}