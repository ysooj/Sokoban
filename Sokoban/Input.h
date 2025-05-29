#pragma once
// 키보드 입력 처리

#include <conio.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "Map.h"
#include "Game.h"
#include "Sound.h"

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80

// 방향 이동 처리를 위한 구조체
struct ball {
	int ballX;
	int ballY;
};

// 비동기 입력 여부 확인
int HasInput()
{
	return _kbhit();
}

// 키 입력 받아오기
char GetInput()
{
	char key = _getch();

	if (key == -32)
	{
		key = _getch();
	}
	// 화살표 키 등 특수 키 처리

	return key;
}

// 입력 처리 함수
// 반환값: 1 = 게임 계속 진행, 0 = 게임 종료
int ProcessInput(GameState* state, char key)
{
	if (key == 'Q' || key == 'q')
	{
		StopBGM();
		state->gameRunning = 0;
		return 0;
	}

	if (key == 'R' || key == 'r')
	{
		RestartStage(state);
		return 1;
	}

	if (key == 'S' || key == 's') {
		InitGame(state, 1); // 첫 스테이지로 초기화
		return 1;
	}

	int nextX = state->x;
	int nextY = state->y;switch (key)
	{
	case 72:  if (state->y > 0) { nextY--; };
		   break;

	case 75: if (state->x > 0) { nextX -= 2; };
		   break;

	case 77: nextX += 2;
		break;

	case 80: nextY++;
		break;
	}

	int directionX = (nextX - state->x) / 2;
	int directionY = nextY - state->y;

	struct ball ballXY = { nextX / 2 + directionX, nextY + directionY };

	if (state->maze[nextY][nextX / 2] != '1')
	{
		if (state->maze[nextY][nextX / 2] == 'B')
		{
			if (state->maze[ballXY.ballY][ballXY.ballX] != '1' && state->maze[ballXY.ballY][ballXY.ballX] != 'B')
			{
				state->maze[nextY][nextX / 2] = state->originalMap[nextY][nextX / 2] == 'G' ? 'G' : '0';
				state->maze[ballXY.ballY][ballXY.ballX] = 'B';

				state->x = nextX;
				state->y = nextY;
			}
		}

		else
		{
			state->x = nextX;
			state->y = nextY;
		}
	}

	return 1;
}