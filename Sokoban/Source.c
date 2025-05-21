#include <stdio.h>
#include <windows.h>
#include <conio.h>	// ����ȭ ����
#include <string.h>	// string ����

int screenIndex;	// ������ �ε��� ��ȣ
HANDLE screen[2];

#define UP 72
#define LEFT 75 
#define RIGHT 77
#define DOWN 80

#define SIZE 10

char maze[SIZE][SIZE] = {
	{'1','1','1','1','1','1','1','1','1','1'},
	{'1','P','0','1','0','0','0','0','0','1'},
	{'1','0','0','1','0','1','1','1','0','1'},
	{'1','0','1','1','0','1','G','1','0','1'},
	{'1','0','1','0','0','1','0','1','0','1'},
	{'1','0','1','0','B','0','0','1','0','1'},
	{'1','0','1','1','1','1','0','1','0','1'},
	{'1','0','0','0','0','0','0','1','0','1'},
	{'1','0','0','0','0','0','0','0','0','1'},
	{'1','1','1','1','1','1','1','1','1','1'}
};

struct ball
{
	int ballX;
	int ballY;
};

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

void Restart(char resetMaze[SIZE][SIZE], int x, int y)
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			resetMaze[i][j] = maze[i][j];
		}
	}
	x = 2;	// �ʱ� �÷��̾� X ��ǥ
	y = 1;	// �ʱ� �÷��̾� Y ��ǥ
}

int main()
{
	// ���� ������� Ű �Է��� �޾ƺ� ���̴�.
	char key = 0;

	int x = 2;
	int y = 1;

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
			Restart(maze, x, y);
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
				if (maze[ballXY.ballY][ballXY.ballX] != '1')
				{
					maze[nextY][nextX / 2] = '0';
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
// Ÿ�� ��������� ���������� �����غ���.