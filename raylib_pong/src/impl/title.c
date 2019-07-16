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

#include "impl.h"

static int result = 0;

/* 게임 시작 화면을 초기화한다. */
void InitTitleScreen(void) {
}

/* 게임 시작 화면을 업데이트한다. */
void UpdateTitleScreen(void) {
    ClearBackground(BLACK);

    DrawText(
        "raylib_pong",
        (GetScreenWidth() - MeasureText("raylib_pong", font_size[2])) / 2,
        GetScreenHeight() * (1.0 / 4.0),
        font_size[2],
        WHITE
    );

    DrawText(
        "by @epsimatt",
        (GetScreenWidth() - MeasureText("by @epsimatt", font_size[0])) / 2,
        GetScreenHeight() * (13.0 / 32.0),
        font_size[0],
        WHITE
    );

    DrawFadingText(
        90,
        "_ Press Enter to Start _",
        (GetScreenWidth() - MeasureText("_ Press Enter to Start _", font_size[0])) / 2,
        GetScreenHeight() * (3.0 / 4.0),
        font_size[0],
        WHITE
    );

    if (IsKeyPressed(KEY_ENTER))
        result = 1;
}

/* 게임 시작 화면을 종료한다. */
int FinishTitleScreen(void) {
    return result;
}