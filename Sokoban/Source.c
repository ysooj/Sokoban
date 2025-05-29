#include <stdio.h>
#include <time.h>

#include "Game.h"
#include "Render.h"
#include "Input.h"
#include "Sound.h"

int main() {
    GameState state;
    InitGame(&state, 1);  // 1번 스테이지부터 시작

    Initialize();
    PlayBGM("bgm.wav");

    while (state.gameRunning) {
        Flip();
        Clear();

        if (HasInput()) {
            char key = GetInput();
            if (!ProcessInput(&state, key)) break;
        }

        // 시간 체크
        clock_t now = clock();
        double elapsed = (double)(now - state.startTime) / CLOCKS_PER_SEC;
        double remaining = 60.0 - elapsed;

        if (remaining <= 0.0) {
            Clear();
            textColor(RED);
            Render(2, 10, "Time is up. You failed.");
            textColor(WHITE);
            Render(2, 12, "Press S to restart the game or Q to quit.");
            Flip();

            while (1) {
                if (HasInput()) {
                    char retryKey = GetInput();
                    if (retryKey == 'S' || retryKey == 's') {
                        InitGame(&state, 1);
                        break;
                    }
                    if (retryKey == 'Q' || retryKey == 'q') {
                        StopBGM();
                        state.gameRunning = 0;
                        break;
                    }
                }
            }
            continue;
        }

        DrawMaze(state.maze);
        textColor(DARKYELLOW);
        Render(state.x, state.y, "★");
        textColor(WHITE);

        char stageStr[50];
        sprintf(stageStr, "Stage %d", state.stage);
        Render(0, 20, "『");
        Render(3, 20, stageStr);
        Render(12, 20, "』");
        Render(0, 22, "Press R to restart the stage!");
        Render(0, 23, "Press Q to quit the game!");

        char timerStr[50];
        sprintf(timerStr, "Time Left: %.1f seconds", remaining);
        textColor(YELLOW);
        Render(17, 20, timerStr);
        textColor(WHITE);

        if (StageClear(&state)) {
            if (!NextStage(&state)) {
                Clear();
                Render(0, 5, "All stages cleared. Congratulations!");
                Flip();
                Sleep(3000);
                break;
            }
        }
    }

    StopBGM();
    ReleaseRender();
    return 0;
}
