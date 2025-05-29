#pragma once
// 게임 로직

#include <time.h>	// 타이머 기능을 사용하기 위해

#include "Map.h"
#include "Render.h"
#include "Sound.h"

#define SIZE 20
#define MAXSTAGE 10

typedef struct
{
	int x, y;
	int stage;
	int gameRunning;
	clock_t startTime;
	char key;
	char maze[SIZE][SIZE];
	char originalMap[SIZE][SIZE];
} GameState;

void InitGame(GameState* state)
{
	state->stage = 1;
	state->gameRunning = 1;
	state->startTime = clock();

	char mapfile[100];
	sprintf_s(mapfile, sizeof(mapfile), "Map%d.txt", state->stage);
	LoadMap(mapfile, state->maze, state->originalMap);
	FindPlayer(state->maze, &state->x, &state->y);
}

void RestartStage(GameState* state)
{
	char mapfile[100];
	sprintf_s(mapfile, sizeof(mapfile), "Map%d.txt", state->stage);
	LoadMap(mapfile, state->maze, state->originalMap);
	FindPlayer(state->maze, &state->x, &state->y);
	state->startTime = clock();
}

int StageClear(GameState* state)
{
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			if (state->originalMap[i][j] == 'G' && state->maze[i][j] != 'B')
				return 0;
	return 1;
}

int NextStage(GameState* state)
{
	state->stage++;
	if (state->stage > MAXSTAGE)
	{
		return 0;	// 게임 종료
	}
	RestartStage(state);
	return 1;
}