#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Создание файла
    if (argc < 2)
    {
        printf("Ввод должен быть в формате: %s <filename>\n", argv[0]);
        return 1;
    }

    const unsigned char data[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    FILE *f = fopen(argv[1], "wb");
    if (!f)
    {
        printf("Не удалось открыть/создать файл для записи\n");
        return 1;
    }

    fwrite(data, sizeof(unsigned char), sizeof(data), f);
    fclose(f);


    // Чтение побайтово
    f = fopen(argv[1], "rb");
    if (!f)
    {
        printf("Не удалось открыть файл для чтения\n");
        return 1;
    }

    unsigned char byte;
    while (fread(&byte, 1, 1, f) == 1)
    {
        printf("Byte: %u | _flags=%d | _IO_read_ptr=%p | _IO_read_end=%p | _IO_read_base=%p | "
            "_IO_write_base=%p | _IO_write_ptr=%p | _IO_write_end=%p | _IO_buf_base=%p | "
            "_IO_buf_end=%p | _IO_save_base=%p | _IO_backup_base=%p | _IO_save_end=%p | "
            "_markers=%p | _chain=%p | _fileno=%d | _flags2=%d | _old_offset=%lld | "
            "_cur_column=%hu | _vtable_offset=%d | _shortbuf[0]=%d | _lock=%p\n\n",
            byte,
            f->_flags,
            f->_IO_read_ptr,
            f->_IO_read_end,
            f->_IO_read_base,
            f->_IO_write_base,
            f->_IO_write_ptr,
            f->_IO_write_end,
            f->_IO_buf_base,
            f->_IO_buf_end,
            f->_IO_save_base,
            f->_IO_backup_base,
            f->_IO_save_end,
            f->_markers,
            f->_chain,
            f->_fileno,
            f->_flags2,
            (long long)f->_old_offset,
            f->_cur_column,
            f->_vtable_offset,
            f->_shortbuf[0],
            f->_lock
        );
    }
    fclose(f);



    // Чтение со смещением
    f = fopen(argv[1], "rb");
    if (!f)
    {
        printf("Не удалось открыть файл для чтения\n");
        return 1;
    }

    fseek(f, 3, SEEK_SET);

    unsigned char buffer[4];
    size_t n = fread(buffer, 1, 4, f);

    printf("Прочитано %zu байт(ов):\n", n);
    for (size_t i = 0; i < n; i++)
    {
        printf("%u ", buffer[i]);
    }
    printf("\n");

    fclose(f);


    return 0;
}