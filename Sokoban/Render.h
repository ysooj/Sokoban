#pragma once
// 콘솔 렌더링, 색상 처리, 더블 버퍼

#include <Windows.h>	// sleep 함수, 콘솔 창 글자 색상 변경 기능을 쓰기 위해
#include <string.h>		// string 관련

#define MAX_SIZE 20
#define MAP_START_Y 4	// UI 아래쪽에 맵 렌더링
#define MAP_START_X 3	// 콘솔창 왼쪽 여백

extern int size;

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

struct Title
{
	const char* filename;
	int startX;
	int startY;
	int color;
};

struct Ending
{
	const char* filename;
	int startX;
	int startY;
	int color;
};

int screenIndex;	// 버퍼의 인덱스 번호
HANDLE screen[2];

void Initialize()	// 초기화 함수. Initialize Screen
{
	CONSOLE_CURSOR_INFO cursor;	// 커서에 대한 정보
	COORD bufferSize = { 15 * 2, 15 };	// 문자 폭 * 2 (한글 2byte 또는 Render(j*2, i, ...) 고려)
	SMALL_RECT windowSize = { 0, 0, 15 * 2 + 5, 15 + 7 };

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
			int renderY = MAP_START_Y + i;	// Y 위치를 아래로 내림
			int renderX = MAP_START_X + j * 2;

			if (maze[i][j] == '0')
			{
				Render(renderX , renderY, "  ");
			}

			else if (maze[i][j] == '1')
			{
				textColor(WHITE);
				Render(renderX, renderY, "■");
			}

			else if (maze[i][j] == 'G')
			{
				textColor(RED);
				Render(renderX, renderY, "○");
			}

			else if (maze[i][j] == 'B')
			{
				// 원래 자리가 골(G)이면 골 위에 있는 공으로 출력
				if (originalMap[i][j] == 'G')
				{
					textColor(PURPLE);
					Render(renderX, renderY, "●");  // 예: 골 위에 있는 공 표시
				}
				else
				{
					textColor(GREEN);
					Render(renderX, renderY, "●");
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

void MainTitle()	// 시작 이미지
{
	Clear();	// 더블 버퍼 초기화

	struct Title title[] =
	{
		{"title/M.txt", 2, 1, PURPLE},
		{"title/O.txt", 10, 1, DARKYELLOW},
		{"title/V.txt", 18, 1, GREEN},
		{"title/E.txt", 28, 1, BLUE},
		{"title/B.txt", 2, 7, BLUE},
		{"title/A.txt", 9, 7, DARKBLUE},
		{"title/L1.txt", 15, 7, GREEN},
		{"title/L2.txt", 21, 7, DARKYELLOW},
		{"title/S.txt", 27, 7, PURPLE},
		{"title/PUSHING.txt", 2, 13, YELLOW},
	};

	for (int i = 0; i < sizeof(title) / sizeof(title[0]); i++)
	{
		FILE* file;
		fopen_s(&file, title[i].filename, "r");

		if (file == NULL)
		{
			// 파일 못 읽으면 그냥 다음으로
			continue;
		}

		textColor(title[i].color);

		char line[128];
		int y = 0;  // Y 좌표 시작 위치
		while (fgets(line, sizeof(line), file))
		{
			// 줄바꿈 문자 제거
			line[strcspn(line, "\r\n")] = '\0';	// 줄바꿈 제거
			Render(title[i].startX, title[i].startY + y++, line);	// 위치 출력
		}

		fclose(file);
	}

	textColor(WHITE);	// 기본색 복구
	Render(6, 21, "Press any key to start!");

	Flip();	// 더블 버퍼로 출력
}

void Ending()	// 시작 이미지
{
	Clear();	// 더블 버퍼 초기화

	struct Ending ending[] =
	{
		{"ending/C.txt", 1, 2, RED},
		{"ending/L1.txt", 8, 2, DARKYELLOW},
		{"ending/E.txt", 13, 2, GREEN},
		{"ending/A.txt", 19, 2, SKYBLUE},
		{"ending/R.txt", 25, 2, BLUE},
		{"ending/yes.txt", 32, 2, PURPLE},
		{"ending/eye.txt", 6, 10, DARKGRAY},
		{"ending/eye.txt", 23, 10, DARKGRAY},
		{"ending/boll1.txt", 0, 13, DARKRED},
		{"ending/boll2.txt", 26, 13, DARKRED},
		{"ending/smile.txt", 13, 14, DARKGRAY},
	};

	for (int i = 0; i < sizeof(ending) / sizeof(ending[0]); i++)
	{
		FILE* file;
		fopen_s(&file, ending[i].filename, "r");

		if (file == NULL)
		{
			// 파일 못 읽으면 그냥 다음으로
			continue;
		}

		textColor(ending[i].color);

		char line[128];
		int y = 0;  // Y 좌표 시작 위치
		while (fgets(line, sizeof(line), file))
		{
			// 줄바꿈 문자 제거
			line[strcspn(line, "\r\n")] = '\0';	// 줄바꿈 제거
			Render(ending[i].startX, ending[i].startY + y++, line);	// 위치 출력
		}

		fclose(file);
	}

	textColor(WHITE);	// 기본색 복구
	Render(10, 19, "Congratulations!");

	Flip();	// 더블 버퍼로 출력
}

void TimeUp(const char* filename)
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error: Cannot open file %s\n", filename);
		return;
	}

	char line[128];
	int y = 0;
	while (fgets(line, sizeof(line), file)) {
		// 줄바꿈 문자 제거
		line[strcspn(line, "\r\n")] = '\0';	// 줄바꿈 제거
		Render(3, 1 + y++, line);	// 위치 출력
	}

	fclose(file);
}


void RenderMap(char maze[MAX_SIZE][MAX_SIZE], char originalMap[MAX_SIZE][MAX_SIZE], int* x, int* y, char* currentStage)
{
	DrawMaze(maze, originalMap);
	textColor(DARKYELLOW);
	textColor(WHITE);
	Render(3, 1, "『");
	Render(6, 1, currentStage);
	Render(15, 1, "』");
	Render(3, MAP_START_Y + size + 1, "Press R to restart the stage!");
	Render(3, MAP_START_Y + size + 2, "Press Q to quit the game!");
}