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

#include "impl/impl.h"
#include "raylib.h"

int main(void) {
    InitAudioDevice();
    InitWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "raylib_pong");
    SetWindowIcon(LoadImage("res/img/icon.png"));

    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        BeginDrawing();
        UpdateCurrentScreen();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}