/*
    Copyright (c) 2020 jdeokkim

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

#ifndef CORE_H
#define CORE_H

#include <math.h>
#include <stdlib.h>

#include "raylib.h"

#define PONG_IMPL 

#define TARGET_FPS 120

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

#define BALL_RADIUS 12.0f

#define PADDLE_WIDTH 24
#define PADDLE_HEIGHT 112

#define END_SCORE 11

#define TIMEOUT 3

/* 게임 장면 목록 */
typedef enum scene {
    INIT,
    TITLE,
    GAMEPLAY,
    GAMEOVER
} Scene;

/* 게임의 현재 상태 */
typedef enum game_state {
    WAITING,
    NORMAL,
    PAUSED
} GameState;

extern GameState current_state;

Image im_linear_gradient;

Sound sn_enter_key_pressed;
Sound sn_left_paddle_bounce;
Sound sn_right_paddle_bounce;
Sound sn_update_score;
Sound sn_update_timer;

/* 깜박이는 텍스트를 그린다. */
PONG_IMPL void DrawBlinkingText(int interval, const char *text, int posX, int posY, int fontSize, Color color);

/* 페이드 효과를 적용한 텍스트를 그린다. */
PONG_IMPL void DrawFadingText(int interval, const char *text, int posX, int posY, int fontSize, Color color);

/* 게임의 다음 장면으로 넘어간다. */
PONG_IMPL void MoveTo(int next_scene);

/* 게임을 '깔끔하게' 종료한다. */
PONG_IMPL void QuitWindow(void);

/* 게임의 현재 장면을 업데이트한다. */
PONG_IMPL void UpdateCurrentScreen(void);

/* 게임 시작 화면을 초기화한다. */
void InitTitleScreen(void);

/* 게임 시작 화면을 업데이트한다. */
void UpdateTitleScreen(void);

/* 게임 시작 화면을 종료한다. */
int FinishTitleScreen(void);

/* 게임 플레이 화면을 초기화한다. */
void InitGameplayScreen(void);

/* 게임 플레이 화면을 업데이트한다. */
void UpdateGameplayScreen(void);

/* 게임 플레이 화면을 종료한다. */
int FinishGameplayScreen(void);

/* 게임 오버 화면을 초기화한다. */
void InitGameoverScreen(void);

/* 게임 오버 화면을 업데이트한다. */
void UpdateGameoverScreen(void);

/* 게임 오버 화면을 종료한다. */
int FinishGameoverScreen(void);

#endif