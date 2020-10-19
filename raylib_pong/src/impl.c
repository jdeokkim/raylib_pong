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

#include <math.h>

#include "impl.h"

Scene current_scene = INIT;
GameState current_state = WAITING;

/* 깜박이는 텍스트를 그린다. */
IMPL void DrawBlinkingText(int interval, const char *text, int posX, int posY, int fontSize, Color color) {
    static int frame_counter;

    if (((frame_counter / interval) % 2) == 0)
        DrawText(text, posX, posY, fontSize, color);
    else
        frame_counter %= 2 * interval;

    frame_counter++;
}

/* 페이드 효과를 적용한 텍스트를 그린다. */
IMPL void DrawFadingText(int interval, const char *text, int posX, int posY, int fontSize, Color color) {
    static int frame_counter;
    float alpha = (float) fabs((float) (interval - frame_counter) / 60);

    DrawText(text, posX, posY, fontSize, Fade(color, alpha));

    frame_counter %= 2 * interval;
    frame_counter++;
}

/* 게임의 다음 장면으로 넘어간다. */
IMPL void MoveTo(int next_scene) {
    if (current_scene != next_scene) {
        switch (next_scene) {
            case TITLE:
                InitTitleScreen();

                break;

            case OPTIONS:

                break;

            case GAMEPLAY:
                InitGameplayScreen();

                break;

            case GAMEOVER:
                InitGameoverScreen();

                break;
        }

        current_scene = next_scene;
    }
}

/* 게임의 현재 장면을 업데이트한다. */
IMPL void UpdateCurrentScreen(void) {
    BeginDrawing();

    switch (current_scene) {
        case INIT:
            SetMasterVolume(0.5f);
            MoveTo(TITLE);

            break;

        case TITLE:
            UpdateTitleScreen();

            if (FinishTitleScreen())
                MoveTo(GAMEPLAY);

            break;

        case OPTIONS:
            break;

        case GAMEPLAY:
            UpdateGameplayScreen();

            if (FinishGameplayScreen())
                MoveTo(GAMEOVER);

            break;

        case GAMEOVER:
            UpdateGameoverScreen();

            if (FinishGameoverScreen())
                MoveTo(TITLE);

            break;
    }

    EndDrawing();
}