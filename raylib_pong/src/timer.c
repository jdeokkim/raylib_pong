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

static int elapsed_time; /* 경과한 시간 */
static int frame_counter = -1; /* 프레임 카운터 */

/* 프레임 카운터를 확인한다. */
int CheckFrameCounter(void) {
    return frame_counter;
}
 
/* 타이머를 확인한다. */
int CheckTimer(void) {
    return elapsed_time;
}

/* 타이머를 초기화한다. */
void ResetTimer(void) {
    elapsed_time = frame_counter = 0;
}

/* 타이머를 업데이트한다. */
void UpdateTimer(void) {
    frame_counter++;
    elapsed_time = frame_counter / TARGET_FPS;
}