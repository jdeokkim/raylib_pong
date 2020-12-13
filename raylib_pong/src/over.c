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

#include "core.h"

static int result = 0;

/* 게임 오버 화면을 초기화한다. */
void InitGameoverScreen(void) {
    sn_enter_key_pressed = LoadSound("res/sounds/enter_key_pressed.wav");
}

/* 게임 오버 화면을 업데이트한다. */
void UpdateGameoverScreen(void) {
    ClearBackground(BLACK);

    DrawText(
        "Game Over!",
        (GetScreenWidth() - MeasureText("Game Over!", 100)) / 2,
        (int) (GetScreenHeight() * (1.75 / 4.0)),
        100,
        WHITE
    );

    if (IsKeyPressed(KEY_ENTER)) {
        PlaySound(sn_enter_key_pressed);
        result = 1;
    }
}

/* 게임 오버 화면을 종료한다. */
int FinishGameoverScreen(void) {
    static int frame_counter = -1; // 프레임 카운터

    if (result) {
        // 효과음이 끝날 때까지 대기
        if (frame_counter < TARGET_FPS / 2) {
            frame_counter++;
            return 0;
        }

        UnloadSound(sn_enter_key_pressed);

        result = 0;
        return 1;
    }

    return result;
}