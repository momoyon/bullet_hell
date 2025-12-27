#include "raylib.h"
#include <spawner.h>
#include <config.h>

Spawner make_spawner(Vector2 pos, float spawn_rate) {
    Spawner res = {
        .pos = pos,
        .alarm = { .alarm_time = spawn_rate },
    };

    return res;
}

void update_spawner(Spawner *s) {

}

void draw_spawner(Spawner *s) {
    if (DEBUG_DRAW) {
        DrawCircleV(s->pos, 32, GREEN);
        DrawCircleLinesV(s->pos, 32, WHITE);
    }
}

