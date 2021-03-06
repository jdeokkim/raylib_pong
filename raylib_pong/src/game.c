﻿/*
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

#define _USE_MATH_DEFINES

#if defined(__WIN32)
    #include <process.h>
    #define PROC_ID ((unsigned int) _getpid())
#else
    #include <sys/types.h> 
    #include <unistd.h>
    #define PROC_ID (getpid())
#endif

#include <stdbool.h>
#include <time.h>

#include "core.h"

static int left_score, right_score; // 플레이어 점수
static unsigned int collision_count; // 공과 패들이 충돌한 횟수

static Color ball_color = { 255, 255, 255, 255 }; // 공의 색깔
static Vector2 ball_pos = { 0.0f }; // 공의 위치

static float ball_speed = 0.0f; // 공의 이동 속도
static double ball_theta = 0.0; // 공이 날아가는 각도
static unsigned char *ball_gradient; // 공의 색깔을 결정하는 데이터

static Vector2 left_paddle_pos = { 0.0f };
static Vector2 right_paddle_pos = { 0.0f };

static float paddle_speed; // 패들의 이동 속도

static int result; // 게임 플레이 화면의 결과값

/* 공의 위치와 각도를 초기화한다. */
static void InitializeBall(void);

/* 배경과 FPS 카운터를 업데이트한다. */
static void UpdateBackground(void);

/* 공의 위치를 업데이트한다. */
static void UpdateBall(void);

/* 패들의 위치를 업데이트한다. */
static void UpdatePaddles(void);

/* 점수를 업데이트한다. */
static void UpdateScores(void);

/* 타이머를 업데이트한다. */
static void UpdateTimer(void);

/* 게임 플레이 화면을 초기화한다. */
void InitGameplayScreen(void) {
    srand((unsigned int) time(NULL) + PROC_ID);

    im_linear_gradient = LoadImage("res/images/linear_gradient.png");
    ball_gradient = (unsigned char *) im_linear_gradient.data;

    sn_left_paddle_bounce = LoadSound("res/sounds/left_paddle_bounce.wav");
    sn_right_paddle_bounce = LoadSound("res/sounds/right_paddle_bounce.wav");
    sn_update_score = LoadSound("res/sounds/update_score.wav");
    sn_update_timer = LoadSound("res/sounds/update_timer.wav");

    left_paddle_pos = (Vector2) { 64.0f, (GetScreenHeight() - PADDLE_HEIGHT) / 2.0f };
    right_paddle_pos = (Vector2) { GetScreenWidth() - 84.0f, (GetScreenHeight() - PADDLE_HEIGHT) / 2.0f };

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
            ball_speed = 0.0f;
            paddle_speed = 10.0f;

            UpdateTimer();

            break;

        case NORMAL:
            ball_speed = 6.0f + (0.15f * collision_count);
            paddle_speed = 10.0f;

            if (IsKeyPressed(KEY_SPACE))
                current_state = PAUSED;

            break;

        case PAUSED:
            ball_speed = 0.0f;
            paddle_speed = 0.0f;

            DrawBlinkingText(
                TARGET_FPS / 2,
                "PAUSED",
                (GetScreenWidth() - MeasureText("PAUSED", 80)) / 2,
                (GetScreenHeight() - 80) / 2,
                80,
                RED
            );

            if (IsKeyPressed(KEY_SPACE))
                current_state = WAITING;

            break;
    }
}

/* 게임 플레이 화면을 종료한다. */
int FinishGameplayScreen(void) {
    static int frame_counter = -1;

    if (result) {
        // 효과음이 끝날 때까지 대기
        if (frame_counter < TARGET_FPS) {
            frame_counter++;
            return 0;
        }

        UnloadImage(im_linear_gradient);

        UnloadSound(sn_left_paddle_bounce);
        UnloadSound(sn_right_paddle_bounce);
        UnloadSound(sn_update_score);
        UnloadSound(sn_update_timer);

        result = 0;
        frame_counter = 0;
        left_score = right_score = 0;

        return 1;
    }
    
    return result;
}

/* 공의 위치와 각도를 초기화한다. */
static void InitializeBall(void) {
    // 공의 이전 각도를 미리 저장
    double prev_ball_theta = ball_theta;

    while (prev_ball_theta == ball_theta
        || (ball_theta > (M_PI / 3.0) && ball_theta < 2.0 * (M_PI / 3.0))
        || (ball_theta > 4.0 * (M_PI / 3.0) && ball_theta < 5.0 * (M_PI / 3.0)))
        ball_theta = /* 0도 ~ 360도 */ M_PI * ((double) rand() / RAND_MAX) * 2.0;

    ball_pos = (Vector2) { GetScreenWidth() / 2.0f, (GetScreenHeight() - 12.0f) / 2.0f };
}

/* 배경과 FPS 카운터를 업데이트한다. */
static void UpdateBackground(void) {
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
static void UpdateBall(void) {
    // 공과 패들의 충돌 횟수가 한번에 일정 횟수 이상 올라가지 않게 하는 변수
    static bool rate_limited = false; 
    static int frame_counter = -1;

    if (rate_limited && frame_counter < (TARGET_FPS / 2))
        frame_counter++;
    else {
        frame_counter = 0;
        rate_limited = false;
    }

    ball_pos.x += ball_speed * (float) cos(ball_theta);
    ball_pos.y += ball_speed * (float) sin(ball_theta);

    if (ball_pos.x <= BALL_RADIUS || ball_pos.x >= GetScreenWidth() - BALL_RADIUS) {
        current_state = WAITING;
        
        collision_count = 0;

        if (ball_pos.x <= BALL_RADIUS)
            right_score++;
        else
            left_score++;

        if (left_score >= END_SCORE || right_score >= END_SCORE)
            result = 1;

        PlaySound(sn_update_score);

        InitializeBall();
    }

    /*
        공이 패들과 충돌할 때의 각도는 공의 그 다음 각도에 영향을 주지 않는다.
        공의 그 다음 각도의 영향을 주는 것은 공의 중앙점과 패들의 중앙점 사이의 거리뿐이다.
        따라서 두 점 사이의 거리를 구한 다음, 정해진 최소 각도와 최대 각도에 따라 공을 날려보낸다.
    */
    if (CheckCollisionCircleRec(ball_pos, BALL_RADIUS, (Rectangle) { left_paddle_pos.x, left_paddle_pos.y, (float) PADDLE_WIDTH, (float) PADDLE_HEIGHT })) {
        double diff = /* 패들 위의 공의 접점 */ (ball_pos.y + BALL_RADIUS * sin(M_PI - ball_theta)) - /* 패들 중앙점의 y좌표 */ (left_paddle_pos.y + (PADDLE_HEIGHT / 2.0));
        ball_theta = 2 * M_PI * (diff < 0) + diff * (M_PI / 180);

        if (!rate_limited) {
            rate_limited = true;

            if (collision_count < 40)
                collision_count++;

            PlaySound(sn_left_paddle_bounce);
        }
    }
    else if (CheckCollisionCircleRec(ball_pos, BALL_RADIUS, (Rectangle) { right_paddle_pos.x, right_paddle_pos.y, (float) PADDLE_WIDTH, (float) PADDLE_HEIGHT })) {
        double diff = /* 패들 위의 공의 접점 */ (ball_pos.y + BALL_RADIUS * sin(ball_theta)) - /* 패들 중앙점의 y좌표 */ (right_paddle_pos.y + (PADDLE_HEIGHT / 2.0));
        ball_theta = M_PI - diff * (M_PI / 180);

        if (!rate_limited) {
            rate_limited = true;

            if (collision_count < 40)
                collision_count++;

            PlaySound(sn_right_paddle_bounce);
        }
    }
    else {
        if (ball_pos.y < BALL_RADIUS || ball_pos.y > GetScreenHeight() - BALL_RADIUS) {
            ball_theta = 2 * M_PI - ball_theta;

            if (ball_pos.y < BALL_RADIUS)
                ball_pos.y = BALL_RADIUS;
            else
                ball_pos.y = GetScreenHeight() - BALL_RADIUS;
        }
    }

    ball_color.r = ball_gradient[9 * collision_count];
    ball_color.g = ball_gradient[9 * collision_count + 1];
    ball_color.b = ball_gradient[9 * collision_count + 2];

    DrawCircle((int) ball_pos.x, (int) ball_pos.y, BALL_RADIUS, ball_color);
}

/* 패들의 위치를 업데이트한다. */
static void UpdatePaddles(void) {
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

    // left_paddle_pos.y = ball_pos.y - 4 * BALL_RADIUS;
    // right_paddle_pos.y = ball_pos.y - 4 * BALL_RADIUS;

    if (left_paddle_pos.y <= 0)
        left_paddle_pos.y = 0;
    else if (left_paddle_pos.y >= GetScreenHeight() - PADDLE_HEIGHT)
        left_paddle_pos.y = (float) (GetScreenHeight() - PADDLE_HEIGHT);

    if (right_paddle_pos.y <= 0)
        right_paddle_pos.y = 0;
    else if (right_paddle_pos.y >= GetScreenHeight() - PADDLE_HEIGHT)
        right_paddle_pos.y = (float) (GetScreenHeight() - PADDLE_HEIGHT);

    DrawRectangle((int) left_paddle_pos.x, (int) left_paddle_pos.y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
    DrawRectangle((int) right_paddle_pos.x, (int)right_paddle_pos.y, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);
}

/* 점수를 업데이트한다. */
static void UpdateScores(void) {
    DrawText(
        TextFormat("%d", left_score),
        (GetScreenWidth() - MeasureText(TextFormat("%d", left_score), 80)) / 2 - 128,
        64,
        80,
        WHITE
    );

    DrawText(
        TextFormat("%d", right_score),
        (GetScreenWidth() - MeasureText(TextFormat("%d", right_score), 80)) / 2 + 128,
        64,
        80,
        WHITE
    );
}

/* 타이머를 업데이트한다. */
static void UpdateTimer(void) {
    static int elapsed_time;
    static int frame_counter = -1;

    elapsed_time = frame_counter / TARGET_FPS;

    if (elapsed_time < TIMEOUT) {
        if (IsKeyPressed(KEY_SPACE))
            current_state = PAUSED;

        if (frame_counter % TARGET_FPS == 0)
            PlaySound(sn_update_timer);

        DrawText(
            TextFormat("%d", TIMEOUT - elapsed_time),
            (GetScreenWidth() - MeasureText(TextFormat("%d", TIMEOUT - elapsed_time), 160)) / 2 - 5,
            (GetScreenHeight() - 160) / 2,
            160,
            GREEN
        );

        frame_counter++;
    }
    else {
        frame_counter = -1;
        current_state = NORMAL;
    }
}
