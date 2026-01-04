#include "raylib.h"
#include <spawner.h>
#include <config.h>
#include <common.h>
#include <lualib.h>
#include <lauxlib.h>

void define_spawner_struct_in_lua(lua_State *L) {
    if (!lua_check(L, luaL_dostring(L,
                "Spawner = {}\n"
                "Spawner.__index = Spawner\n"
                "function Spawner.new(x, y, start, rate, count)\n"
                "local self = setmetatable({}, Spawner)\n"
                "self.x = x\n"
                "self.y = y\n"
                "self.start_time = start\n"
                "self.alarm_rate = rate\n"
                "self.spawn_count = count\n"
                "return self\n"
                "end\n"
                  ))) {
        log_error("Failed to define Spawner struct in LUA!");
        exit(1);
    }
    log_debug("Defined Spawner struct in LUA!");

}

Spawner parse_spawner_from_lua(lua_State *L) {
	lua_getfield(L, -1, "x");
	float x = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "y");
	float y = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "start_time");
	float start_time = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "alarm_rate");
	float alarm_rate = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "spawn_count");
	int spawn_count = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	Spawner s = make_spawner(v2(x, y), start_time, alarm_rate, spawn_count);
	return s;
}

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

bool save_spawners_to_lua(Spawners *spawners, const char *filepath) {
	FILE *f = fopen(filepath, "w");
	if (!f) {
		log_error("%s", strerror(errno));
		return false;
	}

	fprintf(f, "Spawners = {\n");
	for (int i = 0; i < spawners->count; ++i) {
		Spawner *s = &spawners->items[i];
		fprintf(f, "Spawner.new(%f, %f, %f, %lf, %d),\n", s->pos.x, s->pos.y, s->start_time, s->alarm.alarm_time, s->spawn_count);
	}
	fprintf(f, "}\n");

	fclose(f);
	return true;
}

bool load_spawners_from_lua(Spawners *spawners, const char *filepath) {
	if (!lua_check(L, luaL_dofile(L, filepath))) { return false; }

	lua_getglobal(L, "Spawners");
	luaL_checktype(L, -1, LUA_TTABLE);
	size_t count = lua_rawlen(L, -1);
	spawners->count = 0;

	for (int i = 1; i <= count; ++i) {
		lua_pushnumber(L, i);
		lua_gettable(L, -2);

		Spawner s = parse_spawner_from_lua(L);
		darr_append(*spawners, s);

		lua_pop(L, 1); // Pop i
	}

	return true;
}
