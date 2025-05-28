#include <stdio.h>
#include <windows.h>
#include <conio.h>		// 동기화 관련
#include <string.h>		// string 관련
#include <Windows.h>	// sleep 함수, 콘솔 창 글자 색상 변경 기능을 쓰기 위해
#include <mmsystem.h>	// bgm을 넣기 위해
#include <time.h>		// 타이머 기능을 사용하기 위해

#pragma comment(lib, "winmm.lib")	// bgm을 넣기 위해

#include "Map.h"

#define UP 72
#define LEFT 75 
#define RIGHT 77
#define DOWN 80

#define SIZE 20
#define MAXSTAGE 10

char maze[SIZE][SIZE];
char originalMap[SIZE][SIZE];

int screenIndex;	// 버퍼의 인덱스 번호
HANDLE screen[2];

enum Color
{
	BLACK,
	DARKBLUE,
	DARKGREEN,
	DARKSKYBLUE,
	DARKRED,
	DARKVIOLET,
	DARKYELLOW,
	GRAY,
	DARKGRAY,
	BLUE,
	GREEN,
	SKYBLUE,
	RED,
	PURPLE,
	YELLOW,
	WHITE
};

void Initialize()	// 초기화 함수
{
	CONSOLE_CURSOR_INFO cursor;	// 커서에 대한 정보
	COORD bufferSize = { SIZE * 2, SIZE };	// 문자 폭 * 2 (한글 2byte 또는 Render(j*2, i, ...) 고려)
	SMALL_RECT windowSize = { 0, 0, SIZE * 2 + 5, SIZE + 5 };

	// 화면 버퍼를 2개 생성합니다.
	screen[0] = CreateConsoleScreenBuffer
	(
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL
	);

	screen[1] = CreateConsoleScreenBuffer
	(
		GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL
	);

	cursor.dwSize = 1;
	cursor.bVisible = FALSE; // FALSE = 0

	for (int i = 0; i < 2; i++)
	{
		SetConsoleScreenBufferSize(screen[i], bufferSize);	// 버퍼 크기 설정
		SetConsoleWindowInfo(screen[i], TRUE, &windowSize);	// 창 크기 설정
		SetConsoleCursorInfo(screen[i], &cursor);			// 커서 숨김
	}
}

void Flip()	// screen 2개를 교체시켜주는 함수 ; 플리핑
{
	SetConsoleActiveScreenBuffer(screen[screenIndex]);

	screenIndex = !screenIndex;	// 0일 때는 1로, 1일 때는 0으로 바꿔주는 것.
}

void Clear()
{
	COORD position = { 0, 0 };

	DWORD dword;

	CONSOLE_SCREEN_BUFFER_INFO consoleBuffer;

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(handle, &consoleBuffer);

	int width = consoleBuffer.srWindow.Right - consoleBuffer.srWindow.Left + 1;		// 버퍼를 사각형 형태로 만들어준다.
	int height = consoleBuffer.srWindow.Bottom - consoleBuffer.srWindow.Top + 1;	// 버퍼를 사각형 형태로 만들어준다.

	FillConsoleOutputCharacter(screen[screenIndex], ' ', width * height, position, &dword);
}

void Render(int x, int y, const char* string)	// 더블 버퍼를 쓸 때는 이것을 이용해서 렌더링해줘야 한다.
{
	DWORD dword;
	COORD position = { x, y };

	SetConsoleCursorPosition(screen[screenIndex], position);
	WriteFile(screen[screenIndex], string, strlen(string), &dword, NULL);	// string의 길이만큼 string을 출력해줌.
}

void textColor(int color)
{
	SetConsoleTextAttribute(screen[screenIndex], color);
}

void DrawMaze(char maze[SIZE][SIZE])
{
	// 맵의 모든 위치에 대해 출력
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (maze[i][j] == '0')
			{
				Render(j * 2, i, "  ");
			}

			else if (maze[i][j] == '1')
			{
				textColor(WHITE);
				Render(j * 2, i, "■");
			}

			else if (maze[i][j] == 'G')
			{
				textColor(RED);
				Render(j * 2, i, "○");
			}

			else if (maze[i][j] == 'B')
			{
				textColor(GREEN);
				Render(j * 2, i, "●");
			}
		}
	}
}

void Release()
{
	CloseHandle(screen[0]);
	CloseHandle(screen[1]);
}

void Restart(char resetMaze[SIZE][SIZE], int * x, int * y, int stageNumber)
{
	char map[100];
	// map 자체를 저장하는 배열이 아님. 일반적으로 이 배열은 문자 하나하나에 대응하는 값, 즉 문자 → 의미(숫자)의 매핑을 저장하기 위한 배열
	// 예) map['P'] = 2; // 플레이어는 2
	//	   map['0'] = 0; // 빈공간은 0
	//	   map['1'] = 1; // 벽은 1
	//	   map['B'] = 3; // 블럭은 3
	//	   map['G'] = 4; // 골은 4
	// 총 5개의 문자가 등장하지만, 중요한 건 등장 문자 개수가 아니라 문자의 ASCII 값 중 '최대값'
	// 문자	|  ASCII 코드
	// '0'	|  48
	// '1'	|  49
	// 'P'	|  80
	// 'B'	|  66
	// 'G'	|  71
	// 가장 큰 ASCII 값: 'P' = 80
	// 따라서 map[81]이면 map[0] ~ map[80]까지 접근 가능 → 충분함
	// 그러나 조금 여유 있게 하고 싶다면 map[100] 정도로 설정하는 것이 안전하고도 메모리 낭비가 거의 없음

	sprintf_s(map, sizeof(map), "Map%d.txt", stageNumber);

	LoadMap(map, resetMaze, originalMap);
	FindPlayer(resetMaze, x, y);
}

int NextStage(int* stageNumber, char maze[SIZE][SIZE], char originalMap[SIZE][SIZE], int * x, int * y)
{
	*stageNumber += 1;

	if (*stageNumber > MAXSTAGE)
	{
		return 0;	// 게임 종료
	}

	char mapfile[100];

	sprintf_s(mapfile, sizeof(mapfile), "Map%d.txt", *stageNumber);

	FILE* test = fopen(mapfile, "r");
	if (test == NULL)
	{
		printf("다음 스테이지(%s)를 열 수 없습니다.\n", mapfile);
		system("pause");
		exit(1);
	}
	fclose(test);

	LoadMap(mapfile, maze, originalMap);
	FindPlayer(maze, x, y);

	return 1;	// 다음 스테이지로 넘어감
}

int StageClear(char maze[SIZE][SIZE], char originalMap[SIZE][SIZE])
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (originalMap[i][j] == 'G' && maze[i][j] != 'B')
			{
				return 0;	// G에 아직 B가 오지 않아서 아직 클리어 상태가 아님
			}
		}
	}
	return 1;
}

struct ball
{
	int ballX;
	int ballY;
};

int main()
{
	// 동기 방식으로 키 입력을 받아볼 것이다.
	char key = 0;

	int x = 2;
	int y = 1;

	int stageNumber = 1;

	// 타이머 관련
	clock_t start;

	char map[100];
	sprintf_s(map, sizeof(map), "Map%d.txt", stageNumber);
	LoadMap(map, maze, originalMap);
	FindPlayer(maze, &x, &y);

	char currentStage[50];
	sprintf_s(currentStage, sizeof(currentStage), "Stage %d", stageNumber);

	Initialize();

	// 타이머 시작
	start = clock();

	// 게임 시작 시 바로 맵 렌더링
	DrawMaze(maze);
	textColor(DARKYELLOW);         // 보라색으로 설정
	Render(x, y, "★");         // 플레이어 출력
	textColor(WHITE);          // 색상 원상복구 (안 하면 UI도 보라색됨)
	Render(0, 20, "『");
	Render(3, 20, currentStage);
	Render(12, 20, "』");
	Render(0, 22, "Press R to restart the game!");
	Render(0, 23, "Press Q to quit the game!");

	PlaySound(TEXT("bgm.wav"), NULL, SND_ASYNC | SND_LOOP);	// 시작할 때 BGM 재생
	// SND_ASYNC : 비동기 재생. 사운드를 백그라운드에서 재생하면서 다음 코드 계속 실행.
	// SND_LOOP : 반복 재생. SND_ASYNC와 함께 사용해야 함.

	while (1)
	{
		Flip();

		Clear();

		// 키 입력 감지 ; _getch()는 키 입력이 없으면 프로그램 흐름이 멈추기 때문에
		// Render(...), clock() 등 모든 로직도 정지된다.
		// 따라서 _kbhit()로 입력을 비동기 체크해야 실시간으로 타이머와 화면을 갱신할 수 있다.
		if (_kbhit())
		{
			key = _getch();

			if (key == -32)
			{
				key = _getch();
			}

			if (key == 'R' || key == 'r')
			{
				Restart(maze, &x, &y, stageNumber);
				sprintf_s(currentStage, sizeof(currentStage), "Stage %d", stageNumber);
			}

			if (key == 'Q' || key == 'q')
			{
				PlaySound(NULL, 0, 0);	// 종료 시 BGM 멈춤
				break;
			}

			int nextX = x;
			int nextY = y;

			switch (key)
			{
			case 72:  if (y > 0) { nextY--; };
				   break;

			case 75: if (x > 0) { nextX -= 2; };
				   break;

			case 77: nextX += 2;
				break;

			case 80: nextY++;
				break;
			}

			int directionX = (nextX - x) / 2;
			int directionY = nextY - y;

			struct ball ballXY = { nextX / 2 + directionX, nextY + directionY };

			if (maze[nextY][nextX / 2] != '1')
			{
				if (maze[nextY][nextX / 2] == 'B')
				{
					if (maze[ballXY.ballY][ballXY.ballX] != '1' && maze[ballXY.ballY][ballXY.ballX] != 'B')
					{
						maze[nextY][nextX / 2] = originalMap[nextY][nextX / 2] == 'G' ? 'G' : '0';
						maze[ballXY.ballY][ballXY.ballX] = 'B';

						x = nextX;
						y = nextY;
					}
				}

				else
				{
					x = nextX;
					y = nextY;
				}
			}
		}

		DrawMaze(maze);
		textColor(DARKYELLOW);         // 보라색으로 설정
		Render(x, y, "★");         // 플레이어 출력
		textColor(WHITE);          // 색상 원상복구 (안 하면 UI도 보라색됨)
		Render(0, 20, "『");
		Render(3, 20, currentStage);
		Render(12, 20, "』");
		Render(0, 22, "Press R to restart the game!");
		Render(0, 23, "Press Q to quit the game!");

		// 실시간 시간 출력
		clock_t current = clock();
		double elapsed = (double)(current - start) / CLOCKS_PER_SEC;
		double remainedTime = 60.0 - elapsed;

		if (remainedTime <= 0.0)
		{
			Clear();
			textColor(RED);
			Render(2, 10, "Time is up. You failed.");
			textColor(WHITE);
			Render(2, 12, "Press S to restart the game or Q to quit.");
			Flip();

			while (1)
			{
				if (_kbhit())
				{
					char retryKey = _getch();
					if (retryKey == 'S' || retryKey == 's')
					{
						stageNumber = 1;
						start = clock();

						sprintf_s(map, sizeof(map), "Map%d.txt", stageNumber);
						LoadMap(map, maze, originalMap);
						FindPlayer(maze, &x, &y);
						sprintf_s(currentStage, sizeof(currentStage), "Stage %d", stageNumber);
						break;
					}

					if (retryKey == 'Q' || retryKey == 'q')
					{
						PlaySound(NULL, 0, 0);	// 종료 시 BGM 멈춤
						break;
					}
				}
			}
			continue;
		}

		char timeStr[50];
		sprintf_s(timeStr, sizeof(timeStr), "Time Left: %.1f seconds", remainedTime);
		textColor(YELLOW);
		Render(17, 20, timeStr);
		textColor(WHITE);

		if (StageClear(maze, originalMap))
		{
			if (!NextStage(&stageNumber, maze, originalMap, &x, &y))
			{
				Clear();
				Render(0, 5, "All stages cleared. Congratulations!");
				Flip();
				Sleep(3000);	// 3초 후 break
				break;
			}

			// stageNumber가 바뀌었으므로 currentStage 갱신
			sprintf_s(currentStage, sizeof(currentStage), "Stage %d", stageNumber);

			DrawMaze(maze);
		}

		textColor(DARKYELLOW);         // 보라색으로 설정
		Render(x, y, "★");         // 플레이어 출력
		textColor(WHITE);          // 색상 원상복구 (안 하면 UI도 보라색됨)
	}
	
	Release();
	return 0;
}

// (5.20)
// [벽으로는 이동하지 못하게 하기]
// x축은 무조건 짝수로 위치
// 현재 위치에서 벽이 있는 곳을 미리 계산해야 한다.

// [공 옮기기]
// 구조체를 하나 만들어서, 그 구조체의 정보를 가져와서 대신 이동하게 해보자.


// (5.21)
// txt 파일로 스테이지를 관리해보자.


// (5.22)
// 맵 여러 개 만들고 읽어오기.
// 내일은 모든 B를 모든 G에 넣었을 때, 다음 스테이지로 넘어가도록 만들어보자!


// (5.23)
// 개방 폐쇄 원칙이란?
// 콘솔 창 크기 맞추는 함수 맞들기
// or 맵 크기 키우기
// 유저 인터페이스 만들기
// BGM


// (5.26)
// 다음 스테이지로 넘어가면 플레이어 위치가 내가 플레이하던 그대로다.
// P 위치로 플레이어 위치를 지정해야 한다.


// (5.27)
// BGM
// 맵 파일 6부터 수정해야 함.
// wav 파일. 안 되면 mp3 파일.
// playsound() 함수를 사용하여 bgm 넣기.
// 색 입히기.


// (5.28)
// 타이머 기능
// S 누르면 게임 전체 재시작
// 코드 모듈화