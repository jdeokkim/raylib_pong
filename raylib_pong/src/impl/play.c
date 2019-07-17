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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <process.h>
#include <time.h>

#include "impl.h"

static int elapsed_time = 0; 
static int frame_counter = -1;
static int result = 0;

static char timer_str[MAX_LEN]; // 남은 시간을 나타내는 문자열
static char left_score_str[MAX_LEN], right_score_str[MAX_LEN]; // 플레이어 점수를 나타내는 문자열

static int left_score, right_score; // 플레이어 점수

static const float ball_radius = 12.0f; // 공의 반지름
static float ball_speed = 0.0f; // 공의 이동 속도
static double ball_theta = 0.0; // 공이 날아가는 각도
static Vector2 ball_pos = { 0.0f }; // 공의 위치

static const int paddle_width = 24; // 패들의 가로 길이
static const int paddle_height = 108; // 패들의 세로 길이
static float paddle_speed = 0.0f; // 패들의 이동 속도
static Vector2 left_paddle_pos = { 0.0f }, right_paddle_pos = { 0.0f }; // 패들의 위치

static unsigned int collision_count = 0; // 공과 패들이 충돌한 횟수

/* 공의 위치와 각도를 초기화한다. */
void InitializeBall(void);

/* 프레임 카운터를 초기화한다. */
void ResetFrameCounter(void);

/* 배경과 FPS 카운터를 업데이트한다. */
void UpdateBackground(void);

/* 공의 위치를 업데이트한다. */
void UpdateBall(void);

/* 프레임 카운터를 업데이트한다. */
void UpdateFrameCounter(void);

/* 패들의 위치를 업데이트한다. */
void UpdatePaddles(void);

/* 점수를 업데이트한다. */
void UpdateScores(void);

/* 게임 플레이 화면을 초기화한다. */
void InitGameplayScreen(void) {
    srand((unsigned int) time(NULL) + (unsigned int) _getpid());

    sfx_left_paddle_bounce = LoadSound("res/sfx/left_paddle_bounce.wav");
    sfx_right_paddle_bounce = LoadSound("res/sfx/right_paddle_bounce.wav");
    sfx_update_score = LoadSound("res/sfx/update_score.wav");
    sfx_update_timer = LoadSound("res/sfx/update_timer.wav");

    left_paddle_pos = (Vector2) { 64.0f, (GetScreenHeight() - paddle_height) / 2.0f };
    right_paddle_pos = (Vector2) { GetScreenWidth() - 84.0f, (GetScreenHeight() - paddle_height) / 2.0f };

    InitializeBall();
}

/* 게임 플레이 화면을 업데이트한다. */
void UpdateGameplayScreen(void) {
    UpdateBackground();
    UpdateBall();
    UpdatePaddles();
    UpdateScores();

    switch (current_state) {
        case WAITING:
            UpdateFrameCounter();

            ball_speed = 0.0f;
            paddle_speed = 16.0f;

            elapsed_time = frame_counter / TARGET_FPS;

            if (elapsed_time < TIMEOUT) {
                snprintf(timer_str, MAX_LEN, "%d", TIMEOUT - elapsed_time);

                if (frame_counter % TARGET_FPS == 0)
                    PlaySound(sfx_update_timer);

                DrawText(
                    timer_str,
                    (GetScreenWidth() - MeasureText(timer_str, font_size[3])) / 2 - 5,
                    (GetScreenHeight() - font_size[3]) / 2,
                    font_size[3],
                    GREEN
                );
            }
            else {
                current_state = NORMAL;
                ResetFrameCounter();
            }

            break;

        case NORMAL:
            ball_speed = 11.0f + (0.28f * collision_count);
            paddle_speed = 16.0f;

            if (IsKeyPressed(KEY_SPACE))
                current_state = PAUSED;

            break;

        case PAUSED:
            ball_speed = 0.0f;
            paddle_speed = 0.0f;

            DrawBlinkingText(
                30,
                "PAUSED",
                (GetScreenWidth() - MeasureText("PAUSED", font_size[1])) / 2,
                (GetScreenHeight() - font_size[1]) / 2,
                font_size[1],
                RED
            );

            if (IsKeyPressed(KEY_SPACE))
                current_state = NORMAL;

            break;
    }
}

/* 게임 플레이 화면을 종료한다. */
int FinishGameplayScreen(void) {
    if (result) {
        UnloadSound(sfx_left_paddle_bounce);
        UnloadSound(sfx_right_paddle_bounce);
        UnloadSound(sfx_update_score);
        UnloadSound(sfx_update_timer);
    }

    return result;
}

/* 공의 위치와 각도를 초기화한다. */
void InitializeBall(void) {
    // 공의 이전 각도
    double prev_ball_theta = ball_theta;

    while (prev_ball_theta == ball_theta
        || (ball_theta > (M_PI / 3.0) && ball_theta < 2.0 * (M_PI / 3.0))
        || (ball_theta > 4.0 * (M_PI / 3.0) && ball_theta < 5.0 * (M_PI / 3.0)))
        ball_theta = /* 0도 ~ 360도 */ M_PI * ((double)rand() / RAND_MAX) * 2.0;

    ball_pos = (Vector2) { GetScreenWidth() / 2.0f, (GetScreenHeight() - 12.0f) / 2.0f };
}

/* 프레임 카운터를 초기화한다. */
void ResetFrameCounter(void) {
    elapsed_time = 0;
    frame_counter = 0;
}

/* 배경과 FPS 카운터를 업데이트한다. */
void UpdateBackground(void) {
    static Vector2 line_start_pos = { 0.0f }; // 점선 네트를 이루는 각 점선의 시작 위치
    static Vector2 line_end_pos = { 0.0f }; // 점선 네트를 이루는 각 점선의 끝 위치

    DrawFPS(12, 12);
    ClearBackground(BLACK);

    /* 점선 네트를 그린다. */
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
    ball_pos.x += ball_speed * (float) cos(ball_theta);
    ball_pos.y += ball_speed * (float) sin(ball_theta);

    if (ball_pos.x <= ball_radius || ball_pos.x >= GetScreenWidth() - ball_radius) {
        current_state = WAITING;

        collision_count = 0;

        if (ball_pos.x <= ball_radius)
            right_score++;
        else
            left_score++;

        if (left_score >= TARGET_SCORE || right_score >= TARGET_SCORE)
            result = 1;

        PlaySound(sfx_update_score);

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

        collision_count++;

        PlaySound(sfx_left_paddle_bounce);
    }
    else if (CheckCollisionCircleRec(ball_pos, ball_radius, (Rectangle) { right_paddle_pos.x, right_paddle_pos.y, paddle_width, paddle_height })) {
        double diff = /* 패들 위의 공의 접점 */ (ball_pos.y + ball_radius * sin(ball_theta)) - /* 패들 중앙점의 y좌표 */ (right_paddle_pos.y + (paddle_height / 2.0));
        ball_theta = M_PI - diff * (M_PI / 180);

        collision_count++;

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

    DrawCircle(ball_pos.x, ball_pos.y, ball_radius, WHITE);
}

/* 프레임 카운터를 업데이트한다. */
void UpdateFrameCounter(void) {
    frame_counter++;
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
