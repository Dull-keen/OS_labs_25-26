#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define USERS_FILE "users.txt"
#define MAX_LOGIN 6
#define MAX_PIN_DIGITS 6

typedef struct 
{
    char login[MAX_LOGIN + 1];
    int pin;
    int sanctions;
} User;

int 
find_user(const char *login, User *u) 
{
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) return -1; 
    while (fscanf(f, "%6s %d %d", u->login, &u->pin, &u->sanctions) == 3) 
    {
        if (strcmp(u->login, login) == 0) 
        {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void 
save_user(const User *u) 
{
    FILE *f = fopen(USERS_FILE, "a");
    if (!f) 
    {
        printf("Ошибка открытия файла пользователей!\n");
        return;
    }
    fprintf(f, "%s %d %d\n", u->login, u->pin, u->sanctions);
    fclose(f);
}

void 
update_sanction(const char *login, int limit) 
{
    FILE *f = fopen(USERS_FILE, "r");
    if (!f) 
    {
        printf("Ошибка открытия файла");
        return;
    }
    FILE *tmp = fopen("tmp.txt", "w");
    if (!tmp) 
    { 
        fclose(f);
        printf("Ошибка открытия временного файла"); 
        return; 
    }

    User u;
    while (fscanf(f, "%6s %d %d", u.login, &u.pin, &u.sanctions) == 3) 
    {
        if (strcmp(u.login, login) == 0)
            u.sanctions = limit;
        fprintf(tmp, "%s %d %d\n", u.login, u.pin, u.sanctions);
    }

    fclose(f);
    fclose(tmp);
    remove(USERS_FILE);
    rename("tmp.txt", USERS_FILE);
    printf("Лимит для %s = %d\n", login, limit);
}

void 
show_time() 
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    printf("%02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void 
show_date()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    printf("%02d.%02d.%04d\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900); // +1 к месяцу и +1900 к году
}

void 
howmuch(const char *date_str, const char *time_str, const char *flag_str) 
{
    
    struct tm tm = {0};
    if (sscanf(date_str, "%d.%d.%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year) != 3) 
    {
        printf("Неверный формат. Пример: 18.10.2028 12:00:00 -s\n");
        return;
    }
    if (sscanf(time_str, "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 3) 
    {
        printf("Неверный формат. Пример: 18.10.2028 12:00:00 -s\n");
        return;
    }
    tm.tm_mon -= 1;
    tm.tm_year -= 1900;
    
    time_t t_given = mktime(&tm);
    time_t now = time(NULL);
    double diff = difftime(now, t_given);
    
    printf("Текущее время:  %s", ctime(&now));
    printf("Заданное время: %s", ctime(&t_given));

    if (strcmp(flag_str, "-s") == 0)
        printf("Прошло %.0f секунд\n", diff);
    else if (strcmp(flag_str, "-m") == 0)
        printf("Прошло %.1f минут\n", diff / 60);
    else if (strcmp(flag_str, "-h") == 0)
        printf("Прошло %.2f часов\n", diff / 3600);
    else if (strcmp(flag_str, "-y") == 0)
        printf("Прошло %.4f лет\n", diff / (365.25 * 24 * 3600));// Учитываем високосные годы
    else
        printf("Неверный флаг. Используйте -s, -m, -h или -y\n");
    
}

void 
flush_input() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int 
main(void) 
{
    char cmd[128];
    char login[MAX_LOGIN + 1];
    char pin_str[MAX_PIN_DIGITS + 1];
    User u;
    int logged_in = 0, count = 0;

    while (1) 
    {
        if (!logged_in) 
        {
            printf("\nOptions(1/2/3):\n1) Login\n2) Register\n3) Exit\n> ");
            fgets(cmd, sizeof(cmd), stdin);
            cmd[strcspn(cmd, "\n")] = 0;

            if (cmd[0] == '1') // Login
            {
                printf("Enter Login: ");
                scanf("%6s", login);
                flush_input();

                printf("PIN: ");
                scanf("%6s", pin_str);
                flush_input();

                u.pin = atoi(pin_str);
                // Если PIN не число или PIN>100000 или PIN<0 (Выходит за диапазон условия)
                if ((u.pin == 0 && strcmp(pin_str, "0") != 0) || (u.pin > 100000) || (u.pin < 0))
                {
                    printf("Неверный PIN формат.\n");
                    continue;
                }

                User tmp;
                if ((find_user(login, &tmp) == 1) && tmp.pin == u.pin) 
                {
                    printf("Вход выполнен.\n");
                    logged_in = 1;
                    strcpy(u.login, login);
                    u.sanctions = tmp.sanctions;
                    count = 0;
                } else 
                {
                    printf("Неверный логин или PIN.\n");
                }

            } else if (cmd[0] == '2') // Register
            {
                printf("Новый логин: ");
                scanf("%6s", u.login);
                flush_input();

                User tmp;
                if ((find_user(u.login, &tmp) == 1)) 
                {
                    printf("Логин уже существует.\n");
                    continue;
                }

                printf("PIN: ");
                scanf("%6s", pin_str); 
                flush_input();

                u.pin = atoi(pin_str);
                // Если PIN не число или PIN>100000 или PIN<0 (Выходит за диапазон условия)
                if ((u.pin == 0 && strcmp(pin_str, "0") != 0) || (u.pin > 100000) || (u.pin < 0))
                {
                    printf("Неверный PIN формат.\n");
                    continue;
                }

                u.sanctions = 0;
                save_user(&u);
                printf("Пользователь зарегистрирован.\n");

            } else if (cmd[0] == '3') // Exit
            {
                printf("Exited.\n");
                break;
            }

        } else // Залогинен
        {
            
            printf("%s> ", u.login);
            fgets(cmd, sizeof(cmd), stdin);
            cmd[strcspn(cmd, "\n")] = 0;

            if (strncasecmp(cmd, "Time", 4) == 0)
            {

                if (u.sanctions && count >= u.sanctions) 
                {
                    printf("Лимит команд исчерпан.\n");
                    continue;
                }

                show_time(); count++;

            } else if (strncasecmp(cmd, "Date", 4) == 0) 
            {

                if (u.sanctions && count >= u.sanctions) 
                {
                    printf("Лимит команд исчерпан.\n");
                    continue;
                }

                show_date(); count++;

            } else if (strncasecmp(cmd, "Howmuch", 7) == 0) 
            {

                if (u.sanctions && count >= u.sanctions) 
                {
                    printf("Лимит команд исчерпан.\n");
                    continue;
                }

                //char dt[64], fl[4]; 
                char date_part[12]; // Для дд.ММ.гггг (длина 10 + 2 запасных)
                char time_part[10]; // Для чч:мм:сс (длина 8 + 2 запасных)
                char flag[4]; // Для флага (длина 2 + 2 запасных)
                if (sscanf(cmd + 8, "%10s %8s %2s", date_part, time_part, flag) == 3)// Извлекаем дату, время и флаг
                    howmuch(date_part, time_part, flag);
                else
                    printf("Ошибка синтаксиса.\nПример использования: Howmuch 18.10.2028 12:00:00 -s\n");
                count++;

            } else if (strncasecmp(cmd, "Sanctions", 9) == 0) 
            {

                if(u.sanctions && count >= u.sanctions) 
                {
                    printf("Лимит команд исчерпан.\n");
                    continue;
                }

                char target[MAX_LOGIN + 2]; // +2 для безопасности
                if (sscanf(cmd + 10, "%6s", target) == 1) 
                {
                    if (strcmp(target, u.login) == 0) 
                    {
                        printf("Нельзя наложить санкции на себя.\n");
                        continue;
                    }

                    User tmp;
                    if ((find_user(target, &tmp) != 1)) 
                    {
                        printf("Логин не существует.\n");
                        continue;
                    }

                    int limit, confirm;
                    printf("Введите лимит: ");
                    scanf("%d", &limit);
                    flush_input();

                    printf("Для подтверждения введите ключевое число: ");
                    scanf("%d", &confirm);
                    flush_input();

                    if (confirm == 52) 
                    {
                        update_sanction(target, limit);
                    } else 
                    {
                        printf("Ошибка в ключевомм числе.\n");
                    }
                } else 
                {
                    printf("Использование: Sanctions <username>\n");
                }

            } else if (strncasecmp(cmd, "Logout", 6) == 0)
            {
                logged_in = 0;
                printf("Выход из аккаунта.\n");
            } else 
            {
                printf("Неизвестная команда.\n");
            }
        }
    }
    return 0;
}