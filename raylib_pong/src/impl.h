/*
    Copyright (c) 2019 epsimatt <epsimatt@protonmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdbool.h>

#include "raylib.h"

#define TARGET_FPS 60

typedef enum current_state {
    TITLE, /* 타이틀 화면 */
    PAUSED, /* 일시정지 */
    GAMEPLAY /* 게임 중 */
} CurrentState; // 게임의 현재 상태

static const float ball_radius = 12.0f; // 공의 반지름

static const int paddle_width = 24; // 패들의 가로 길이
static const int paddle_height = 108; // 패들의 세로 길이

static const int font_size[4] = { 40, 80, 100, 160 }; // 게임 메시지의 글자 크기

/* 깜박이는 텍스트를 그린다. */
void DrawBlinkingText(int interval, const char *text, int posX, int posY, int fontSize, Color color);

/* 페이드 효과를 적용한 텍스트를 그린다. */
void DrawFadingText(int interval, const char *text, int posX, int posY, int fontSize, Color color);

/* 공의 위치와 각도를 초기화한다. */
void InitializeBall(void);

/* 게임을 초기화한다. */
void InitializeGame(void);

/* 게임을 일시정지한다. */
void PauseGame(void);

/* 게임 시작 화면을 보여준다. */
void ShowTitleScreen(void);

/* 게임의 일시정지 상태를 변경한다. */
void TogglePause(void);

/* 게임에 사용된 메모리를 모두 해제한다. */
void UnloadGame(void);

/* 배경과 FPS 카운터를 업데이트한다. */
void UpdateBackground(void);

/* 공의 위치를 업데이트한다. */
void UpdateBall(void);

/* 게임 화면을 업데이트한다. */
void UpdateGame(void);

/* 패들의 위치를 업데이트한다. */
void UpdatePaddles(void);

/* 점수를 업데이트한다. */
void UpdateScores(void);