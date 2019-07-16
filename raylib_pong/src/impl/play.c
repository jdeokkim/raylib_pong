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

#include <stdlib.h>
#include <time.h>

#include "impl.h"

static int result = 0;

/* 배경과 FPS 카운터를 업데이트한다. */
void UpdateBackground(void);

/* 게임 플레이 화면을 초기화한다. */
void InitGameplayScreen(void) {
    srand((unsigned int) time(NULL) + (unsigned int) _getpid());

    sfx_left_paddle_bounce = LoadSound("res/sfx/left_paddle_bounce.wav");
    sfx_right_paddle_bounce = LoadSound("res/sfx/right_paddle_bounce.wav");
    sfx_update_score = LoadSound("res/sfx/update_score.wav");
    sfx_update_timer = LoadSound("res/sfx/update_timer.wav");
}

/* 게임 플레이 화면을 업데이트한다. */
void UpdateGameplayScreen(void) {
    UpdateBackground();
}

/* 게임 플레이 화면을 종료한다. */
int FinishGameplayScreen(void) {
    return result;
}

/* 배경과 FPS 카운터를 업데이트한다. */
void UpdateBackground(void) {
    static Vector2 line_start_pos = { 0.0f }; // 점선 네트를 이루는 각 점선의 시작 위치
    static Vector2 line_end_pos = { 0.0f }; // 점선 네트를 이루는 각 점선의 끝 위치

    DrawFPS(12, 12);
    ClearBackground(BLACK);

    /* 점선 네트 그리기 */
    for (int i = 0; i < 24; i++) {
        line_start_pos = (Vector2) { GetScreenWidth() / 2.0f, 0.0f + i * 32.0f };
        line_end_pos = (Vector2) { GetScreenWidth() / 2.0f, 16.0f + i * 32.0f };

        DrawLineEx(line_start_pos, line_end_pos, 10.0f, LIGHTGRAY);
    }
}