#pragma once
#include <stdio.h>

#define SIZE 20

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

void FindPlayer(char maze[SIZE][SIZE], int* px, int* py)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (maze[i][j] == 'P')
            {
                *px = j * 2; // x 좌표는 출력 시 2칸씩 띄우므로 *2
                *py = i;

                return;
            }
        }
    }
    // 만약 못 찾으면 기본값 설정
    *px = 2;
    *py = 1;
}