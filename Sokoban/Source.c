#include <stdio.h>
#include <windows.h>
#include <conio.h>		// 동기화 관련
#include <string.h>		// string 관련
#include <windows.h>	// sleep 함수를 쓰기 위해

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

void Initialize()	// 초기화 함수
{
	CONSOLE_CURSOR_INFO cursor;	// 커서에 대한 정보
	COORD bufferSize = { SIZE * 2, SIZE };	// 문자 폭 * 2 (한글 2byte 또는 Render(j*2, i, ...) 고려)
	SMALL_RECT windowSize = { 0, 0, SIZE * 2 + 3, SIZE + 3 };

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
				Render(j * 2, i, "■");
			}

			else if (maze[i][j] == 'G')
			{
				Render(j * 2, i, "○");
			}

			else if (maze[i][j] == 'B')
			{
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
	// 지도 자체를 저장하는 배열이 아님. 일반적으로 이 배열은 문자 하나하나에 대응하는 값, 즉 문자 → 의미(숫자)의 매핑을 저장하기 위한 배열
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

	char map[100];
	sprintf_s(map, sizeof(map), "Map%d.txt", stageNumber);
	LoadMap(map, maze, originalMap);
	FindPlayer(maze, &x, &y);

	Initialize();

	// 게임 시작 시 바로 맵 렌더링
	DrawMaze(maze);
	Render(x, y, "★");
	Render(0, 21, "Press R to restart the game!");
	Render(0, 23, "Press Q to quit the game!");

	while (1)
	{
		Flip();

		Clear();

		key = _getch();

		if (key == -32)
		{
			key = _getch();
		}

		if (key == 'R' || key == 'r')
		{
			Restart(maze, &x, &y, stageNumber);
		}

		if (key == 'Q' || key == 'q')
		{
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

		DrawMaze(maze);
		Render(x, y, "★");
		Render(0, 21, "Press R to restart the game!");
		Render(0, 23, "Press Q to quit the game!");

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

			//	플레이어 위치를 P로 하기 위해서 주석처리 해놓음.
			//	x = 2;
			//	y = 1;

			// 다음 스테이지가 자동으로 바로 보이도록!
			// 이 부분은 내일 수정해야 해. 맵을 완성하면 자동으로 넘어가게 하려고
			// 넣은 코드인데, 테스트 해보고 삭제 or 수정해야 함.
			//Clear();
			DrawMaze(maze);
			//Render(x, y, "★");
			//Render(0, 11, "Press R to restart the game!");
			//Flip();
		}
		Render(x, y, "★");
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