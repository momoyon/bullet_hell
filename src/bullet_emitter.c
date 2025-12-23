#include "config.h"
#include <bullet_emitter.h>
#include <common.h>
#include <lauxlib.h>

Bullet_emitter make_bullet_emitter(lua_State *L, Vector2 pos, Bullets *bullets_ptr, float fire_rate, const char *pattern_funcname, void *userdata) {
	Bullet_emitter em = {
        .L = L,
		.pos = pos,
		.bullets_ptr = bullets_ptr,
		.lua_pattern_funcname = pattern_funcname,
		.userdata = userdata,
	};
	ASSERT(bullets_ptr, "bullets_ptr is NULL");
	em.alarm = (Alarm) { .alarm_time = fire_rate };

	return em;
}

void update_bullet_emitter(Bullet_emitter *em) {
	if (on_alarm(&em->alarm, modified_delta)) {
		if (em->lua_pattern_funcname) {
            lua_getglobal(em->L, "Pattern");
            lua_pushnumber(em->L, em->pos.x);
            lua_pushnumber(em->L, em->pos.y);
            lua_pushnumber(em->L, modified_delta);

            if (lua_check(em->L, lua_pcall(em->L, 3, 2, 0))) {
                luaL_checktype(em->L, -2, LUA_TTABLE);
                luaL_checktype(em->L, -1, LUA_TNUMBER);

                int bullets_count = lua_tonumber(em->L, -1);
                lua_pop(em->L, 1);
                log_debug("Bullets Count: %d", bullets_count);

                Bullets bs = {0};

                for (int i = 1; i <= bullets_count; ++i) {
                    lua_pushnumber(em->L, i);
                    lua_gettable(em->L, -2);

                    Bullet b = parse_bullet_from_lua(em->L);

                    darr_append(bs, b);

                    lua_pop(em->L, 1); // Pop i

                }

                for (int i = 0; i < bs.count; ++i) {
                    darr_append(bullets, bs.items[i]);
                }

                darr_free(bs);
            }
		}
	}
}
