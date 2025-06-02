#pragma once
// 게임 로직

#include <stdio.h>

#include "Map.h"

#define MAX_SIZE 20
#define MAXSTAGE 10

extern int size;

void Restart(char resetMaze[MAX_SIZE][MAX_SIZE], int* x, int* y, int stageNumber)
{
	char mapfile[100];

	size = GetStageSize(stageNumber);

	sprintf_s(mapfile, sizeof(mapfile), "Map/Map%d.txt", stageNumber);
	LoadMap(mapfile, resetMaze, resetMaze, size);
	FindPlayer(resetMaze, x, y, size);
}

int NextStage(int* stageNumber, char maze[MAX_SIZE][MAX_SIZE], char originalMap[MAX_SIZE][MAX_SIZE], int* x, int* y)
{
	*stageNumber += 1;

	if (*stageNumber > MAXSTAGE)
	{
		return 0;	// 게임 종료
	}

	size = GetStageSize(*stageNumber);	// 다음 스테이지에 맞게 size 업데이트

	char mapfile[100];
	sprintf_s(mapfile, sizeof(mapfile), "Map/Map%d.txt", *stageNumber);
	LoadMap(mapfile, maze, originalMap, size);
	FindPlayer(maze, x, y, size);

	Initialize();	// 콘솔 크기 다시 설정

	return 1;	// 다음 스테이지로 넘어감
}

int StageClear(char maze[MAX_SIZE][MAX_SIZE], char originalMap[MAX_SIZE][MAX_SIZE])
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (originalMap[i][j] == 'G' && maze[i][j] != 'B')
			{
				return 0;	// G에 아직 B가 오지 않아서 아직 클리어 상태가 아님
			}
		}
	}
	return 1;
}