#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "permissions.h"
#include "fperm.h"

int main(void)
{
    mode_t current_mode = 0;
    int has_mode = 0;
    char input[256];

    while (1) {
        printf("\n=== Программа расчета маски прав доступа ===\n");
        printf("1. Ввести права доступа в буквенном или цифровом обозначении\n");
        printf("2. Ввести имя файла для отображения прав (через stat)\n");
        printf("3. Изменить права (имитация chmod)\n");
        printf("4. Выход\n");
        printf("Выберите опцию: ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                printf("Введите права (rwxr-xr-x или 755): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                if (parse_permissions(input, &current_mode) == 0) {
                    has_mode = 1;
                    printf("Права успешно установлены!\n");
                    print_permissions(current_mode);
                } else {
                    printf("Ошибка: неверный формат прав!\n");
                }
                break;

            case 2:
                printf("Введите имя файла: ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                mode_t m = get_file_permissions(input);
                if (m != (mode_t)-1) {
                    current_mode = m;
                    has_mode = 1;
                    printf("Права файла получены (сравните с ls -l %s):\n", input);
                    print_permissions(current_mode);
                } else {
                    printf("Ошибка при получении прав доступа к файлу!\n");
                }
                break;

            case 3:
                if (!has_mode) {
                    printf("Сначала задайте начальные права (опции 1 или 2)!\n");
                    break;
                }
                printf("Введите команду модификации (u+x / g-r / a=rwx и т.п.): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = '\0';
                apply_chmod_command(&current_mode, input);
                printf("Права изменены (имитация, реальный файл НЕ изменён)!\n");
                print_permissions(current_mode);
                break;

            case 4:
                printf("Выход из программы.\n");
                return 0;

            default:
                printf("Неверный выбор!\n");
        }
    }
}