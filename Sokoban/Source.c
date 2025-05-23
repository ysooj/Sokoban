#include <stdio.h>
#include <windows.h>
#include <conio.h>	// ����ȭ ����
#include <string.h>	// string ����

#include "Map.h"

#define UP 72
#define LEFT 75 
#define RIGHT 77
#define DOWN 80

#define SIZE 10
#define MAXSTAGE 9

char maze[SIZE][SIZE];
char originalMap[SIZE][SIZE];

int screenIndex;	// ������ �ε��� ��ȣ
HANDLE screen[2];

void Initialize()	// �ʱ�ȭ �Լ�
{
	CONSOLE_CURSOR_INFO cursor;	// Ŀ���� ���� ����

	// ȭ�� ���۸� 2�� �����մϴ�.
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

	SetConsoleCursorInfo(screen[0], &cursor);
	SetConsoleCursorInfo(screen[1], &cursor);
}

void Flip()	// screen 2���� ��ü�����ִ� �Լ� ; �ø���
{
	SetConsoleActiveScreenBuffer(screen[screenIndex]);

	screenIndex = !screenIndex;	// 0�� ���� 1��, 1�� ���� 0���� �ٲ��ִ� ��.
}

void Clear()
{
	COORD position = { 0, 0 };

	DWORD dword;

	CONSOLE_SCREEN_BUFFER_INFO consoleBuffer;

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleScreenBufferInfo(handle, &consoleBuffer);

	int width = consoleBuffer.srWindow.Right - consoleBuffer.srWindow.Left + 1;		// ���۸� �簢�� ���·� ������ش�.
	int height = consoleBuffer.srWindow.Bottom - consoleBuffer.srWindow.Top + 1;	// ���۸� �簢�� ���·� ������ش�.

	FillConsoleOutputCharacter(screen[screenIndex], ' ', width * height, position, &dword);
}

void Render(int x, int y, const char* string)	// ���� ���۸� �� ���� �̰��� �̿��ؼ� ����������� �Ѵ�.
{
	DWORD dword;
	COORD position = { x, y };

	SetConsoleCursorPosition(screen[screenIndex], position);
	WriteFile(screen[screenIndex], string, strlen(string), &dword, NULL);	// string�� ���̸�ŭ string�� �������.
}

void DrawMaze(char maze[SIZE][SIZE])
{
	// ���� ��� ��ġ�� ���� ���
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
				Render(j * 2, i, "��");
			}

			else if (maze[i][j] == 'G')
			{
				Render(j * 2, i, "��");
			}

			else if (maze[i][j] == 'B')
			{
				Render(j * 2, i, "��");
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
	char map[20];
	sprintf_s(map, sizeof(map), "Map%d.txt", stageNumber);

	LoadMap(map, resetMaze, originalMap);

	* x = 2;
	* y = 1;
}

int NextStage(int* stageNumber, char map[SIZE][SIZE], char originalMap[SIZE][SIZE])
{
	*stageNumber += 1;

	if (*stageNumber >= MAXSTAGE)
	{
		return 0;	// ���� ����
	}

	int a = *stageNumber;

	char mapfile[20];

	sprintf_s(mapfile, sizeof(mapfile), "Map%d.txt", a);

	LoadMap(map, maze, originalMap);

	return 1;	// ���� ���������� �Ѿ
}

int StageClear(char maze[SIZE][SIZE], char originalMap[SIZE][SIZE])
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			if (originalMap[i][j] == 'G' && maze[i][j] != 'B')
			{
				return 0;	// G�� ���� B�� ���� �ʾƼ� ���� Ŭ���� ���°� �ƴ�
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
	// ���� ������� Ű �Է��� �޾ƺ� ���̴�.
	char key = 0;

	int x = 2;
	int y = 1;

	int stageNumber = 1;

	char map[20];
	sprintf_s(map, sizeof(map), "Map%d.txt", stageNumber);
	LoadMap(map, maze, originalMap);

	Initialize();

	// ���� ���� �� �ٷ� �� ������
	DrawMaze(maze);
	Render(x, y, "��");
	Render(0, 11, "Press R to restart the game!");

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
		Render(x, y, "��");
		Render(0, 11, "Press R to restart the game!");

		if (StageClear(maze, originalMap))
		{
			if (!NextStage(&stageNumber, map, originalMap))
			{
				Clear();
				Render(0, 5, "All stages cleared. Congratulations!");
				Flip();
				break;
			}

			x = 2;
			y = 1;

			// ���� ���������� �ڵ����� �ٷ� ���̵���!
			// �� �κ��� ���� �����ؾ� ��. ���� �ϼ��ϸ� �ڵ����� �Ѿ�� �Ϸ���
			// ���� �ڵ��ε�, �׽�Ʈ �غ��� ���� or �����ؾ� ��.
			Clear();
			DrawMaze(maze);
			Render(x, y, "��");
			Render(0, 11, "Press R to restart the game!");
			Flip();
		}
	}
	
	Release();
	return 0;
}

// (5.20)
// [�����δ� �̵����� ���ϰ� �ϱ�]
// x���� ������ ¦���� ��ġ
// ���� ��ġ���� ���� �ִ� ���� �̸� ����ؾ� �Ѵ�.

// [�� �ű��]
// ����ü�� �ϳ� ����, �� ����ü�� ������ �����ͼ� ��� �̵��ϰ� �غ���.


// (5.21)
// txt ���Ϸ� ���������� �����غ���.


// (5.22)
// �� ���� �� ����� �о����.
// ������ ��� B�� ��� G�� �־��� ��, ���� ���������� �Ѿ���� ������!


// (5.23)
// ���� ��� ��Ģ�̶�?
// �ܼ� â ũ�� ���ߴ� �Լ� �µ��
// or �� ũ�� Ű���
// ���� �������̽� �����
// BGM