#pragma once
#include <stdio.h>

#define SIZE 10

void LoadMap(const char* map, char maze[SIZE][SIZE], char originalMap[SIZE][SIZE])
{
    FILE* file = fopen(map, "r");

    // 파일 열기 실패 시 처리
    if (file == NULL)
    {
        printf("Error: Could not open the file %s\n", map);
        return;
    }

    // 파일 읽기 시작
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            int ch = fgetc(file);

            // 줄바꿈 문자 건너뛰기
            while (ch == '\n' || ch == '\r')
            {
                ch = fgetc(file);
            }

            // 문자 저장
            maze[i][j] = (char)ch;
            originalMap[i][j] = (char)ch;
        }
    }

    fclose(file); // 파일 닫기
}
