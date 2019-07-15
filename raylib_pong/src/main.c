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
#include "raylib.h"

int main(void)
{
    InitAudioDevice();
    InitWindow(1024, 768, "raylib_pong");
    SetWindowIcon(LoadImage("res/img/icon.png"));

    SetTargetFPS(TARGET_FPS);

    InitializeGame();

    CurrentState current_state = TITLE;

    while (!WindowShouldClose()) {
        BeginDrawing();

        switch (current_state) {
            case TITLE:
                ShowTitleScreen();

                if (IsKeyPressed(KEY_ENTER))
                    current_state = GAMEPLAY;

                break;

            case PAUSED:
                PauseGame();

                if (IsKeyPressed(KEY_SPACE)) {
                    current_state = GAMEPLAY;
                    TogglePause();
                }

                break;
              
            case GAMEPLAY:
                UpdateGame();

                if (IsKeyPressed(KEY_SPACE)) {
                    current_state = PAUSED;
                    TogglePause();
                }

                break;
        }

        EndDrawing();
    }

    UnloadGame();

    CloseWindow();

    return 0;
}