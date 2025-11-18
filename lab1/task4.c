#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

// Провера на простоту
int is_prime(unsigned int x) 
{
    if (x < 2) return 0;
    if (x == 2 || x == 3) return 1;
    if (x % 2 == 0) return 0;
    for (unsigned int i = 3; i * i <= x; i += 2)
    {
        if (x % i == 0) return 0;
    }
    return 1;
}

// xor8: XOR всех байтов
void process_xor8(FILE *fp)
{
    unsigned char byte;
    unsigned char result = 0;

    while (fread(&byte, 1, 1, fp) == 1)
    {
        result ^= byte;
    }

    printf("%u\n", result);
}

// xorodd: XOR всех 4-байтовых подпоследовательностей, где хотя бы один байт является простым числом
void process_xorodd(FILE *fp)
{
    unsigned char buf[4];
    size_t n = fread(buf, 1, 4, fp);
    if (n < 4)
    {
        printf("Ошибка\n");
        return;
    }

    uint32_t result = 0;
    do
    {
        int has_prime = 0;
        for (int i = 0; i < 4; i++)
        {
            if (is_prime(buf[i]))
            {
                has_prime = 1;
                break;
            }
        }

        if (has_prime)
        {
            // собираем число в «естественном» порядке файла
            uint32_t value = ((uint32_t)buf[0] << 24) |
                             ((uint32_t)buf[1] << 16) |
                             ((uint32_t)buf[2] << 8) |
                             ((uint32_t)buf[3]);
            result ^= value;
        }

        // сдвигаем окно на 1 байт
        int c = fgetc(fp);
        if (c == EOF) break;
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = buf[3];
        buf[3] = (unsigned char)c;

    } while (1);

    printf("%u\n", result);
}

// mask: подсчет 4-ёх байтовых чисел, удовлетворяющих маске
void process_mask(FILE *fp, uint32_t mask) 
{
    unsigned char buf[4];
    size_t n = fread(buf, 1, 4, fp);
    if (n < 4) {
        printf("Ошибка\n");
        return;
    }

    unsigned long long count = 0;
    do {
        // собираем число в «естественном» порядке файла
        uint32_t value = ((uint32_t)buf[0] << 24) |
                         ((uint32_t)buf[1] << 16) |
                         ((uint32_t)buf[2] << 8) |
                         ((uint32_t)buf[3]);

        if ((value & mask) == mask) {
            count++;
        }

        // сдвигаем окно на 1 байт
        int c = fgetc(fp);
        if (c == EOF) break;
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = buf[3];
        buf[3] = (unsigned char)c;

    } while (1);

    printf("%llu\n", count);
}

int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Ввод должен быть в формате: %s <filename> <xor8|xorodd|mask> [mask_hex]\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) 
    {
        printf("Не удалось открыть файл для чтения\n");
        return 1;
    }

    if (strcmp(argv[2], "xor8") == 0)
    {
        process_xor8(fp);
    } 
    else if (strcmp(argv[2], "xorodd") == 0)
    {
        process_xorodd(fp);
    } 
    else if (strcmp(argv[2], "mask") == 0)
    {
        if (argc < 4)
        {
            printf("Не указана маска: %s <filename> <xor8|xorodd|mask> [mask_hex]\n", argv[0]);
            fclose(fp);
            return 1;
        }
        char *endptr;
        uint32_t mask = (uint32_t)strtoul(argv[3], &endptr, 16);
        if (*endptr != '\0') 
        {
            printf("Неправильный формат маски\n");
            fclose(fp);
            return 1;
        }
        process_mask(fp, mask);
    } 
    else 
    {
        printf("Неправильно указан режим работы - %s\n", argv[2]);
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}