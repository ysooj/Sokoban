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

int size;

int stageTimeLimits[21] = 
{
    0,    // 인덱스 0 unused
    10,   // stage 1
    20,   // stage 2
    25,   // stage 3
    30,   // stage 4
    40,   // stage 5
    20,  // stage 6
    25,   // stage 7
    30,   // stage 8
    45,  // stage 9
    50  // stage 10
};

int main()
{
    char key = 0;
    int x = 2;
    int y = 1;
    int stageNumber = 1;

    clock_t start;  // 타이머
    clock_t lastBallMove = clock(); // 마지막 공 이동 시간

    char maze[MAX_SIZE][MAX_SIZE];
    char originalMap[MAX_SIZE][MAX_SIZE];
    char currentStage[50];

    // 무작위성 시드 설정
    srand((unsigned int)time(NULL));

    SetConsoleTitle(TEXT("MOVE BALLS"));
    Initialize();   // 먼저 더블 버퍼 초기화

    // 타이틀 화면 출력
    MainTitle(); // 더블 버퍼 기반으로 렌더링

    while (!_kbhit())   // 아무 키 대기
    {
        Sleep(50);
    }
    _getch(); // 입력 소비

    // stage 1 맵 로드
    LoadStage(stageNumber, maze, originalMap, &x, &y, currentStage);

    // 타이머 시작
    start = clock();

    // 게임 시작 시 바로 맵 렌더링
    RenderMap(maze, originalMap, &x, &y, currentStage);

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
                
                start = clock();  // 타이머 재설정
            }

            else if (key == 'Q' || key == 'q')
            {
                PlaySound(NULL, 0, 0);
                gameRunning = 0;
                break;
            }

            // 개발을 용이하게 하기 위한 방법
            else if (key == 'T' || key == 't')
            {
                if (!NextStage(&stageNumber, maze, originalMap, &x, &y))
                {
                    Clear();
                    Ending();
                    Sleep(3000); // 3초 후 종료
                    gameRunning = 0;
                    break;
                }

                // 다음 스테이지로 넘어갔으니 타이머 초기화
                start = clock();

                // currentStage 문자열 업데이트
                sprintf_s(currentStage, sizeof(currentStage), "Stage %d", stageNumber);
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
                    if (maze[b.ballY][b.ballX] != '1' && maze[b.ballY][b.ballX] != 'B' && maze[b.ballY][b.ballX] != 'P')
                    {
                        maze[nextY][nextX / 2] = originalMap[nextY][nextX / 2] == 'G' ? 'G' : '0';
                        maze[b.ballY][b.ballX] = 'B';

                        // 이전 위치 복원
                        maze[y][x / 2] = (originalMap[y][x / 2] == 'G') ? 'G' : '0';

                        x = nextX;
                        y = nextY;

                        maze[y][x / 2] = 'P';
                    }
                }
                else
                {
                    // 이전 위치 복원
                    maze[y][x / 2] = (originalMap[y][x / 2] == 'G') ? 'G' : '0';

                    x = nextX;
                    y = nextY;

                    maze[y][x / 2] = 'P';
                }
            }
        }

        if (stageNumber == 4 || stageNumber == 5 || stageNumber == 9 || stageNumber == 10)
        {
            clock_t now = clock();
            double elapsedSinceBallMove = (double)(now - lastBallMove) / CLOCKS_PER_SEC;

            if (elapsedSinceBallMove >= 3.0)    // 공이 움직인 지 3초가 지나면
            {
                // 공 위치 저장
                struct ball balls[100];
                int ballCount = 0;

                for (int i = 0; i < size; i++)
                {
                    for (int j = 0; j < size; j++)
                    {
                        if (maze[i][j] == 'B' && originalMap[i][j] != 'G')  // 공이 목적지에 있지 않을 때만 이동 목록에 추가
                        {
                            balls[ballCount].ballY = i;
                            balls[ballCount].ballX = j;
                            ballCount++;
                        }
                    }
                }
            
                // 이동 방향 정의
                int dx[] = { 0, 0, -1, 1 };
                int dy[] = { -1, 1, 0, 0 };

                // 임시 맵 복사
                char tempMaze[MAX_SIZE][MAX_SIZE];
                memcpy(tempMaze, maze, sizeof(tempMaze));

                // 이동 시도 위치리를 확인할 마크 배열
                int moveMarked[MAX_SIZE][MAX_SIZE] = { 0 };

                for (int k = 0; k < ballCount; k++)
                {
                    int i = balls[k].ballY;
                    int j = balls[k].ballX;

                    // 랜덤한 방향 순서 만들기
                    int dir[4] = { 0, 1, 2, 3 };
                    for (int i = 0; i < 4; i++)
                    {
                        int r = rand() % 4;
                        int temp = dir[i];
                        dir[i] = dir[r];
                        dir[r] = temp;
                    }

                    // 가능한 방향 중 첫 번째 빈 공간으로 이동
                    for (int d = 0; d < 4; d++)
                    {
                        int ni = i + dy[dir[d]];
                        int nj = j + dx[dir[d]];

                        if (ni < 0 || ni >= size || nj < 0 || nj >= size)
                            continue;

                        char target = maze[ni][nj];

                        // 공끼리, 벽, 목적지, 플레이어 모두 피하고
                        // 동시에 다른 공이 이 자리에 오지 않았는지 확인
                        if (target != '1' && target != 'B' && target != 'G' && target != 'P' && moveMarked[ni][nj] == 0)  // 현재 맵 기준
                        {
                            // 이동 위치 마킹
                            moveMarked[ni][nj] = 1;

                            // 원래 자리에 G가 있었는지 확인해서 복원
                            if (originalMap[i][j] == 'G')
                                tempMaze[i][j] = 'G';
                            else
                                tempMaze[i][j] = '0';

                            tempMaze[ni][nj] = 'B';

                            break;  // 한 방향만 이동
                        }
                    }
                }

                // 최종 적용
                memcpy(maze, tempMaze, sizeof(tempMaze));
                lastBallMove = clock();
            }
        }

        RenderMap(maze, originalMap, &x, &y, currentStage);

        // 실시간 시간 출력 ; 게임 전체 타이머 설정 코드
        // clock_t current = clock();
        // double elapsed = (double)(current - start) / CLOCKS_PER_SEC;
        // double remainedTime = 300.0 - elapsed;

        // 남은 시간 계산 시 현재 스테이지 제한 시간 사용
        clock_t current = clock();

        double timeLimit = stageTimeLimits[stageNumber];
        double elapsed = (double)(current - start) / CLOCKS_PER_SEC;
        double remainedTime = timeLimit - elapsed;

        if (remainedTime <= 0.0)
        {
            Clear();
            textColor(WHITE);
            TimeUp("clock.txt");
            textColor(RED);
            Render(3, 16, "Time is up. You failed.");
            textColor(WHITE);
            Render(3, 18, "Press R to restart the stage!");
            textColor(YELLOW);
            Render(3, 20, "Press S to restart the game");
            Render(3, 21, "or Q to quit.");
            textColor(WHITE);

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
                        LoadStage(stageNumber, maze, originalMap, &x, &y, currentStage);
                        break;
                    }

                    else if (retryKey == 'R' || retryKey == 'r')
                    {
                        // 현재 스테이지 다시 시작
                        start = clock();
                        LoadStage(stageNumber, maze, originalMap, &x, &y, currentStage);
                        break;
                    }

                    else if (retryKey == 'Q' || retryKey == 'q')
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
        Render(3, 2, timeStr);
        textColor(WHITE);

        if (StageClear(maze, originalMap))
        {
            if (!NextStage(&stageNumber, maze, originalMap, &x, &y))
            {
                Clear();
                Ending();
                Sleep(3000); // 3초 후 종료
                gameRunning = 0;
                break;
            }

            start = clock();  // 새 스테이지 시작 시간 재설정

            // stageNumber가 바뀌었으므로 currentStage 갱신
            sprintf_s(currentStage, sizeof(currentStage), "Stage %d", stageNumber);

            DrawMaze(maze, originalMap);
        }

        textColor(DARKYELLOW);
        Render(MAP_START_X + x, MAP_START_Y + y, "★");
        textColor(WHITE);
    }

    Release();
    return 0;
}

// 콘솔 창 제목 바꾸기
// 공이 스스로 움직이게 하기