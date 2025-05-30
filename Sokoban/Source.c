#include <stdio.h>
#include <Windows.h>    // sleep 함수, 콘솔 창 글자 색상 변경 기능을 쓰기 위해
#include <conio.h>      // 동기화 관련
#include <mmsystem.h>   // bgm
#include <time.h>       // 타이머 기능

#pragma comment(lib, "winmm.lib")   // bgm

#include "Map.h"
#include "Render.h"
#include "Game.h"

struct ball
{
    int ballX;
    int ballY;
};

int main()
{
    char key = 0;
    int x = 2;
    int y = 1;
    int stageNumber = 1;
    int size;

    // 타이머
    clock_t start;

    char maze[MAX_SIZE][MAX_SIZE];
    char originalMap[MAX_SIZE][MAX_SIZE];

    char mapfile[100];
    char currentStage[50];

    size = GetStageSize(stageNumber);
    sprintf_s(mapfile, sizeof(mapfile), "Map%d.txt", stageNumber);
    LoadMap(mapfile, maze, originalMap, size);
    FindPlayer(maze, &x, &y, size);

    sprintf_s(currentStage, sizeof(currentStage), "Stage %d", stageNumber);

    Initialize();

    // 타이머 시작
    start = clock();

    // 게임 시작 시 바로 맵 렌더링
    DrawMaze(maze, originalMap);
    textColor(DARKYELLOW);
    Render(x, y, "★");
    textColor(WHITE);
    Render(0, 20, "『");
    Render(3, 20, currentStage);
    Render(12, 20, "』");
    Render(0, 22, "Press R to restart the stage!");
    Render(0, 23, "Press Q to quit the game!");

    PlaySound(TEXT("bgm.wav"), NULL, SND_ASYNC | SND_LOOP);

    int gameRunning = 1;

    while (gameRunning)
    {
        Flip();
        Clear();

        if (_kbhit()) {

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

            else if (key == 'Q' || key == 'q')
            {
                PlaySound(NULL, 0, 0);
                gameRunning = 0;
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

            int dirX = (nextX - x) / 2;
            int dirY = nextY - y;
            struct ball b = { nextX / 2 + dirX, nextY + dirY };

            if (maze[nextY][nextX / 2] != '1')
            {
                if (maze[nextY][nextX / 2] == 'B')
                {
                    if (maze[b.ballY][b.ballX] != '1' && maze[b.ballY][b.ballX] != 'B')
                    {
                        maze[nextY][nextX / 2] = originalMap[nextY][nextX / 2] == 'G' ? 'G' : '0';
                        maze[b.ballY][b.ballX] = 'B';

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

        DrawMaze(maze, originalMap);
        textColor(DARKYELLOW);
        Render(x, y, "★");
        textColor(WHITE);
        Render(0, 20, "『");
        Render(3, 20, currentStage);
        Render(12, 20, "』");
        Render(0, 22, "Press R to restart the stage!");
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

                        sprintf_s(mapfile, sizeof(mapfile), "Map%d.txt", stageNumber);
                        LoadMap(mapfile, maze, originalMap, size);
                        FindPlayer(maze, &x, &y, size);
                        sprintf_s(currentStage, sizeof(currentStage), "Stage %d", stageNumber);
                        break;
                    }

                    if (retryKey == 'Q' || retryKey == 'q')
                    {
                        PlaySound(NULL, 0, 0);	// 종료 시 BGM 멈춤
                        gameRunning = 0;
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

            DrawMaze(maze, originalMap);
        }

        textColor(DARKYELLOW);
        Render(x, y, "★");
        textColor(WHITE);
    }

    Release();
    return 0;
}

