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

#include "raylib.h"

#define IMPL  

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768
#define TARGET_FPS 60
#define TARGET_SCORE 11
#define MAX_LEN 16
#define TIMEOUT 3

/* 게임 장면 목록 */
typedef enum scene {
    INIT,
    TITLE,
    OPTIONS,
    GAMEPLAY,
    GAMEOVER
} Scene;

/* 게임의 현재 상태 */
typedef enum game_state {
    WAITING,
    NORMAL,
    PAUSED
} GameState;

extern Scene current_scene;
extern GameState current_state;

static Sound sfx_left_paddle_bounce, sfx_right_paddle_bounce;
static Sound sfx_update_score, sfx_update_timer;

static const int font_size[4] = { 40, 80, 100, 160 };

/* 깜박이는 텍스트를 그린다. */
IMPL void DrawBlinkingText(int interval, const char *text, int posX, int posY, int fontSize, Color color);

/* 페이드 효과를 적용한 텍스트를 그린다. */
IMPL void DrawFadingText(int interval, const char *text, int posX, int posY, int fontSize, Color color);

/* 게임의 다음 장면으로 넘어간다. */
IMPL void MoveTo(int next_scene);

/* 게임의 현재 장면을 업데이트한다. */
IMPL void UpdateCurrentScreen(void);

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