#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Ввод должен быть в формате: %s <source_file> <destination_file>\n", argv[0]);
        return 1;
    }

    const char *source_file = argv[1];
    const char *dest_file = argv[2];

    FILE *src = fopen(source_file, "rb");
    if (!src)
    {
        printf("Не удалось открыть исходный файл для чтения\n");
        return 1;
    }

    FILE *dst = fopen(dest_file, "wb");
    if (!dst)
    {
        printf("Не удалось открыть файл, куда копируем\n");
        fclose(src);
        return 1;
    }

    //Если 2 раза указать 1 файл, то обнуляется, надо ли править?
    //int fd1 = fileno(src);
    //int fd2 = fileno(dst);
    //printf("%d = %d", fd1, fd2);
    //Можно привести к абсолютному пути и проерить, но мне кажется это defined behaviour.

    unsigned char buffer[BUFFER_SIZE];
    size_t bytesRead;

    // Цикл чтения и записи
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, src)) > 0)
    {
        size_t bytesWritten = fwrite(buffer, 1, bytesRead, dst);
        if (bytesWritten != bytesRead)
        {
            printf("Не удалось совершить запись в файле\n");
            fclose(src);
            fclose(dst);
            return 1;
        }
    }

    fclose(src);
    fclose(dst);

    printf("Копирование завершено.\n");
    return 0;
}