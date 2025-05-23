#pragma once
#include <stdio.h>

#define SIZE 10

void LoadMap(const char* map, char maze[SIZE][SIZE], char originalMap[SIZE][SIZE])
{
    FILE* file = fopen(map, "r");

    // ���� ���� ���� �� ó��
    if (file == NULL)
    {
        printf("Error: Could not open the file %s\n", map);
        return;
    }

    // ���� �б� ����
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            int ch = fgetc(file);

            // �ٹٲ� ���� �ǳʶٱ�
            while (ch == '\n' || ch == '\r')
            {
                ch = fgetc(file);
            }

            // ���� ����
            maze[i][j] = (char)ch;
            originalMap[i][j] = (char)ch;
        }
    }

    fclose(file); // ���� �ݱ�
}
