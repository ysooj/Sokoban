#pragma once
// 맵 로딩 및 플레이어 위치 찾기

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 20

int GetStageSize(int stageNumber)
{
    if (stageNumber >= 1 && stageNumber <= 5)   // 1 ~ 5 단계는 10 x 10 크기
    {
        return 10;
    }

    else  // 6 ~ 10 단계는 15 x 15 크기
    {
        return 15;
    }
}

void LoadMap(const char* map, char maze[MAX_SIZE][MAX_SIZE], char originalMap[MAX_SIZE][MAX_SIZE], int size)
{
    FILE* file = fopen(map, "r");

    // 파일 열기 실패 시 처리
    if (file == NULL)
    {
        printf("Error: Could not open the file %s\n", map);
        return;
    }

    // 파일 읽기 시작
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
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

void FindPlayer(char maze[MAX_SIZE][MAX_SIZE], int* x, int* y, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (maze[i][j] == 'P')
            {
                *x = j * 2; // x 좌표는 출력 시 2칸씩 띄우므로 *2
                *y = i;

                return;
            }
        }
    }
    // 만약 못 찾으면 기본값 설정
    *x = 2;
    *y = 1;
}