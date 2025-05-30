#pragma once
// 콘솔 렌더링, 색상 처리, 더블 버퍼

#include <Windows.h>	// sleep 함수, 콘솔 창 글자 색상 변경 기능을 쓰기 위해
#include <string.h>		// string 관련

#define MAX_SIZE 20

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

int screenIndex;	// 버퍼의 인덱스 번호
HANDLE screen[2];

void Initialize()	// 초기화 함수. Initialize Screen
{
	CONSOLE_CURSOR_INFO cursor;	// 커서에 대한 정보
	COORD bufferSize = { size * 2, size };	// 문자 폭 * 2 (한글 2byte 또는 Render(j*2, i, ...) 고려)
	SMALL_RECT windowSize = { 0, 0, size * 2 + 5, size + 5 };

	cursor.dwSize = 1;
	cursor.bVisible = FALSE; // FALSE = 0

	for (int i = 0; i < 2; i++)
	{
		// 화면 버퍼를 2개 생성합니다.
		screen[i] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
		SetConsoleScreenBufferSize(screen[i], bufferSize);	// 버퍼 크기 설정
		SetConsoleWindowInfo(screen[i], TRUE, &windowSize);	// 창 크기 설정
		SetConsoleCursorInfo(screen[i], &cursor);			// 커서 숨김
	}
}

void Flip()	// screen 2개를 교체시켜주는 함수 ; 플리핑. Flip Screen
{
	SetConsoleActiveScreenBuffer(screen[screenIndex]);
	screenIndex = !screenIndex;	// 0일 때는 1로, 1일 때는 0으로 바꿔주는 것.
}

void Clear()	// Clear Screen
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

void textColor(int color)
{
	SetConsoleTextAttribute(screen[screenIndex], color);
}

void Render(int x, int y, const char* str)	// 더블 버퍼를 쓸 때는 이것을 이용해서 렌더링해줘야 한다.
{
	DWORD dword;
	COORD position = { x, y };

	SetConsoleCursorPosition(screen[screenIndex], position);
	WriteFile(screen[screenIndex], str, strlen(str), &dword, NULL);	// string의 길이만큼 string을 출력해줌.
}

void DrawMaze(char maze[MAX_SIZE][MAX_SIZE], char originalMap[MAX_SIZE][MAX_SIZE])
{
	// 맵의 모든 위치에 대해 출력
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
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
				// 원래 자리가 골(G)이면 골 위에 있는 공으로 출력
				if (originalMap[i][j] == 'G')
				{
					textColor(PURPLE);
					Render(j * 2, i, "●");  // 예: 골 위에 있는 공 표시
				}
				else
				{
					textColor(GREEN);
					Render(j * 2, i, "●");
				}
			}
		}
	}
	textColor(WHITE);
}

void Release()	// Release Screen
{
	CloseHandle(screen[0]);
	CloseHandle(screen[1]);
}