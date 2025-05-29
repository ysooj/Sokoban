#pragma once
// BGM 재생, 정지

void PlayBGM()
{
	PlaySound(TEXT("bgm.wav"), NULL, SND_ASYNC | SND_LOOP);	// 시작할 때 BGM 재생
	// SND_ASYNC : 비동기 재생. 사운드를 백그라운드에서 재생하면서 다음 코드 계속 실행.
	// SND_LOOP : 반복 재생. SND_ASYNC와 함께 사용해야 함.
}

void StopBGM()
{
	PlaySound(NULL, 0, 0);	// 종료 시 BGM 멈춤
}