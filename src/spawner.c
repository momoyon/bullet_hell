#include "raylib.h"
#include <spawner.h>
#include <config.h>
#include <common.h>

Spawner make_spawner(Vector2 pos, float start_time, float spawn_rate, int count) {
    Spawner res = {
		.start_time = start_time,
        .pos = pos,
        .alarm = { .alarm_time = spawn_rate },
		.spawn_count = count,
    };

    return res;
}

void update_spawner(Spawner *s) {

}

void draw_spawner(Spawner *s, float a) {
	int ts = lua_getint(L, "TILESIZE");
    if (DEBUG_DRAW) {
        DrawCircleV(s->pos, ts/2, ColorAlpha(GREEN, a));
        DrawCircleLinesV(s->pos, ts/2, ColorAlpha(WHITE, a));
    }
}

