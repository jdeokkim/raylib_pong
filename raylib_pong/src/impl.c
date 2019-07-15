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

#define _USE_MATH_DEFINES

#define MAX_LEN 16
#define TIMEOUT 3

#include <math.h>
#include <process.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "impl.h"
#include "raylib.h"
#include "timer.h"

static bool is_paused = false; // 게임의 일시정지 상태를 나타내는 변수
static bool wait_with_timeout = true; // 게임을 시작하기 전에 기다려야 되는지를 나타내는 변수

static char timer_str[MAX_LEN]; // 남은 시간을 나타내는 문자열
static char left_score_str[MAX_LEN]; // 플레이어 1의 점수를 나타내는 문자열
static char right_score_str[MAX_LEN]; // 플레이어 2의 점수를 나타내는 문자열

static Sound sfx_left_paddle_bounce, sfx_right_paddle_bounce; // 공이 패들과 충돌할 때 재생할 효과음
static Sound sfx_update_player_score; // 플레이어 점수가 올라갈 때 재생할 효과음
static Sound sfx_update_timer; // 남은 시간을 업데이트할 때 재생할 효과음

static int left_score, right_score; // 플레이어 점수

static float ball_speed = 0.0f; // 공의 이동 속도
static double ball_theta = 0.0; // 공이 날아가는 각도
static Vector2 ball_pos = { 0.0f }; // 공의 위치

static unsigned int paddle_collision_count = 0; // 공과 패들이 충돌한 횟수
static float paddle_speed = 16.0f; // 패들의 이동 속도
static Vector2 left_paddle_pos = { 0.0f }; // 플레이어 1의 패들 위치
static Vector2 right_paddle_pos = { 0.0f }; // 플레이어 2의 패들 위치

/* 깜박이는 텍스트를 그린다. */
static void DrawBlinkingText(int interval, const char *text, int posX, int posY, int fontSize, Color color) {
    static int frame_counter;

    if (((frame_counter / interval) % 2) == 0)
        DrawText(text, posX, posY, fontSize, color);
    else
        frame_counter %= 2 * interval;

    frame_counter++;
}

/* 페이드 효과를 적용한 텍스트를 그린다. */
static void DrawFadingText(int interval, const char *text, int posX, int posY, int fontSize, Color color) {
    static int frame_counter;
    float alpha = (float) fabs((float) (interval - frame_counter) / 60);

    DrawText(text, posX, posY, fontSize, Fade(color, alpha));

    frame_counter %= 2 * interval;
    frame_counter++;
}

/* 공의 위치와 각도를 초기화한다. */
void InitializeBall(void) {
    // 공의 이전 각도
    double prev_ball_theta = ball_theta;

    while (prev_ball_theta == ball_theta 
        || (ball_theta > (M_PI / 3.0) && ball_theta < 2.0 * (M_PI / 3.0)) 
        || (ball_theta > 4.0 * (M_PI / 3.0) && ball_theta < 5.0 * (M_PI / 3.0)))
        ball_theta = /* 0도 ~ 360도 */ M_PI * ((double) rand() / RAND_MAX) * 2.0;

    ball_pos = (Vector2) { GetScreenWidth() / 2.0f, (GetScreenHeight() - 12.0f) / 2.0f };
}

/* 게임을 초기화한다. */
void InitializeGame(void) {
    srand((unsigned int) time(NULL) + (unsigned int) _getpid());

    sfx_left_paddle_bounce = LoadSound("res/sfx/left_paddle_bounce.wav");
    sfx_right_paddle_bounce = LoadSound("res/sfx/right_paddle_bounce.wav");
    sfx_update_player_score = LoadSound("res/sfx/update_player_score.wav");
    sfx_update_timer = LoadSound("res/sfx/update_timer.wav");

    left_paddle_pos = (Vector2) { 64.0f, (GetScreenHeight() - paddle_height) / 2.0f };
    right_paddle_pos = (Vector2) { GetScreenWidth() - 84.0f, (GetScreenHeight() - paddle_height) / 2.0f };

    InitializeBall();
}

/* 게임을 일시정지한다. */
void PauseGame(void) {
    UpdateGame();

    DrawBlinkingText(
        30,
        "PAUSED",
        (GetScreenWidth() - MeasureText("PAUSED", font_size[1])) / 2,
        (GetScreenHeight() - font_size[1]) / 2,
        font_size[1],
        RED
    );
}

/* 시작 화면을 보여준다. */
void ShowTitleScreen(void) {
    ClearBackground(BLACK); // 배경 업데이트

    DrawText(
        "raylib_pong",
        (GetScreenWidth() - MeasureText("raylib_pong", font_size[2])) / 2,
        GetScreenHeight() * (1.0f / 4.0f),
        font_size[2],
        WHITE
    );

    DrawText(
        "by @epsimatt",
        (GetScreenWidth() - MeasureText("by @epsimatt", font_size[0])) / 2,
        GetScreenHeight() * (13.0f / 32.0f),
        font_size[0],
        WHITE
    );

    DrawFadingText(
        90,
        "_ Press Enter to Start _",
        (GetScreenWidth() - MeasureText("_ Press Enter to Start _", font_size[0])) / 2,
        GetScreenHeight() * (3.0f / 4.0f),
        font_size[0],
        WHITE
    );
}

/* 게임의 일시정지 상태를 변경한다. */
void TogglePause(void) {
    if (!is_paused) {
        is_paused = true;

        ball_speed = 0.0f;
        paddle_speed = 0.0f;
    }
    else {
        is_paused = false;
    }
}

/* 게임에 사용된 메모리를 모두 해제한다. */
void UnloadGame(void) {
    UnloadSound(sfx_left_paddle_bounce);
    UnloadSound(sfx_right_paddle_bounce);
    UnloadSound(sfx_update_player_score);
}

/* 배경과 FPS 카운터를 업데이트한다. */
void UpdateBackground(void) {
    static Vector2 line_start_pos = { 0.0f }; // 점선 네트를 이루는 각 점선의 시작 위치
    static Vector2 line_end_pos = { 0.0f }; // 점선 네트를 이루는 각 점선의 끝 위치

    DrawFPS(12, 12); // FPS 카운터 업데이트
    ClearBackground(BLACK); // 배경 업데이트

    /* 점선 네트 그리기 */
    for (int i = 0; i < 24; i++) {
        line_start_pos = (Vector2) { GetScreenWidth() / 2.0f, 0.0f + i * 32.0f };
        line_end_pos = (Vector2) { GetScreenWidth() / 2.0f, 16.0f + i * 32.0f };

        DrawLineEx(line_start_pos, line_end_pos, 10.0f, LIGHTGRAY);
    }
}

/* 공의 위치를 업데이트한다. */
void UpdateBall(void) {
    /*
        속도 벡터 `v` (`v` = (x, y) = (f(t), g(t)))가 x축의 양의 방향과 이루는 각의 크기를 θ라고 하자.
        이때 `v_x` = `v` * cosθ이고, `v_y`는 `v` * sinθ인 것을 그림으로 확인할 수 있다.

        따라서 `v_x` = `v` * cosθ = dx/dt, `v_y` = `v` * sinθ  = dy/dt이다.
    */
    ball_pos.x += ball_speed * cos(ball_theta);
    ball_pos.y += ball_speed * sin(ball_theta);

    if (ball_pos.x <= ball_radius || ball_pos.x >= GetScreenWidth() - ball_radius) {
        paddle_collision_count = 0;
        wait_with_timeout = true;

        if (ball_pos.x <= ball_radius)
            right_score++;
        else
            left_score++;

        PlaySound(sfx_update_player_score);

        InitializeBall();
    }

    /*
        공이 패들과 충돌할 때의 각도는 공의 그 다음 각도에 영향을 주지 않는다. 
        공의 그 다음 각도의 영향을 주는 것은 공의 중앙점과 패들의 중앙점 사이의 거리뿐이다.

        따라서 두 점 사이의 거리를 구한 다음, 정해진 최소 각도와 최대 각도에 따라 공을 날려보낸다.
    */
    if (CheckCollisionCircleRec(ball_pos, ball_radius, (Rectangle) { left_paddle_pos.x, left_paddle_pos.y, paddle_width, paddle_height })) {
        double diff = /* 패들 위의 공의 접점 */ (ball_pos.y + ball_radius * sin(M_PI - ball_theta)) - /* 패들 중앙점의 y좌표 */ (left_paddle_pos.y + (paddle_height / 2.0));
        ball_theta = 2 * M_PI * (diff < 0) + diff * (M_PI / 180);

        paddle_collision_count++;

        PlaySound(sfx_left_paddle_bounce);
    }
    else if (CheckCollisionCircleRec(ball_pos, ball_radius, (Rectangle) { right_paddle_pos.x, right_paddle_pos.y, paddle_width, paddle_height })) {
        double diff = /* 패들 위의 공의 접점 */ (ball_pos.y + ball_radius * sin(ball_theta)) - /* 패들 중앙점의 y좌표 */ (right_paddle_pos.y + (paddle_height / 2.0));
        ball_theta = M_PI - diff * (M_PI / 180);

        paddle_collision_count++;

        PlaySound(sfx_right_paddle_bounce);
    }
    else {
        if (ball_pos.y < ball_radius || ball_pos.y > GetScreenHeight() - ball_radius) {
            ball_theta = 2 * M_PI - ball_theta;

            if (ball_pos.y < ball_radius)
                ball_pos.y = ball_radius;
            else
                ball_pos.y = GetScreenHeight() - ball_radius;
        }
    }

    /* TODO: from `CLITERAL{ 255, 255, 255, 255 }` to `CLITERAL{ 230, 41, 55, 255 }` */
    DrawCircle(ball_pos.x, ball_pos.y, ball_radius, WHITE);
}

/* 게임 화면을 업데이트한다. */
void UpdateGame(void) {
    UpdateBackground();
    UpdateBall();
    UpdatePaddles();
    UpdateScores();
    UpdateTimer();

    if (wait_with_timeout && !is_paused && CheckTimer() < TIMEOUT) {
        snprintf(timer_str, MAX_LEN, "%d", TIMEOUT - CheckTimer());

        if (CheckFrameCounter() % TARGET_FPS == 0)
            PlaySound(sfx_update_timer);

        DrawText(
            timer_str,
            (GetScreenWidth() - MeasureText(timer_str, font_size[3])) / 2 - 5,
            (GetScreenHeight() - font_size[3]) / 2,
            font_size[3],
            GREEN
        );

        ball_speed = 0.0f;
    }
    else {
        if (!is_paused) {
            wait_with_timeout = false;
            ball_speed = 11.0f + (0.28f * paddle_collision_count);
        }

        ResetTimer();
    }
}

/* 패들의 위치를 업데이트한다. */
void UpdatePaddles(void) {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_S)) {
        if (IsKeyDown(KEY_W))
            left_paddle_pos.y -= paddle_speed;
        else
            left_paddle_pos.y += paddle_speed;
    }

    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN)) {
        if (IsKeyDown(KEY_UP))
            right_paddle_pos.y -= paddle_speed;
        else
            right_paddle_pos.y += paddle_speed;
    }

    if (left_paddle_pos.y <= 0)
        left_paddle_pos.y = 0;
    else if (left_paddle_pos.y >= GetScreenHeight() - paddle_height)
        left_paddle_pos.y = GetScreenHeight() - paddle_height;

    if (right_paddle_pos.y <= 0)
        right_paddle_pos.y = 0;
    else if (right_paddle_pos.y >= GetScreenHeight() - paddle_height)
        right_paddle_pos.y = GetScreenHeight() - paddle_height;

    DrawRectangle(left_paddle_pos.x, left_paddle_pos.y, paddle_width, paddle_height, WHITE);
    DrawRectangle(right_paddle_pos.x, right_paddle_pos.y, paddle_width, paddle_height, WHITE);
}

/* 점수를 업데이트한다. */
void UpdateScores(void) {
    snprintf(left_score_str, MAX_LEN, "%d", left_score);
    snprintf(right_score_str, MAX_LEN, "%d", right_score);

    DrawText(
        left_score_str, 
        (GetScreenWidth() - MeasureText(left_score_str, font_size[1])) / 2 - 128,
        64, 
        font_size[1],
        WHITE
    );

    DrawText(
        right_score_str, 
        (GetScreenWidth() - MeasureText(left_score_str, font_size[1])) / 2 + 128,
        64, 
        font_size[1],
        WHITE
    );
}