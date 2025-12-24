#include "hitbox.h"
#include "raylib.h"
#include <bullet.h>
#include <config.h>
#include <engine.h>
#include <common.h>
#include <lualib.h>
#include <lauxlib.h>

void define_bullet_struct_in_lua(lua_State *L) {
    if (!lua_check(L, luaL_dostring(L,
                "Bullet = {}\n"
                "Bullet.__index = Bullet\n"
                "function Bullet.new(x, y, tex_offset_x, tex_offset_y, tex_w, tex_h, hframes, vframes, min_speed, max_speed, speed, speed_dt, dir_degrees, hbox)\n"
                "local self = setmetatable({}, Bullet)\n"
                "self.x = x\n"
                "self.y = y\n"
                "self.tex_offset_x = tex_offset_x\n"
                "self.tex_offset_y = tex_offset_y\n"
                "self.tex_w = tex_w\n"
                "self.tex_h = tex_h\n"
                "self.texname = texname\n"
                "self.hframes = hframes\n"
                "self.vframes = vframes\n"
                "self.min_speed = min_speed\n"
                "self.max_speed = max_speed\n"
                "self.speed = speed\n"
                "self.speed_dt = speed_dt\n"
                "self.dir_degrees = dir_degrees\n"
                "self.hitbox = hbox\n"
                "return self\n"
                "end\n"
                  ))) {
        log_error("Failed to define Bullet struct in LUA!");
        exit(1);
    }
    log_debug("Defined Bullet struct in LUA!");
}

Bullet make_bullet(Vector2 pos, Vector2i tex_offset, Vector2i tex_size, int hframes, int vframes, float direction_degrees, float speed, Hitbox hbox) {
	Bullet b = {
		.pos = pos,
		.dir = v2_from_degrees(direction_degrees),
		.hitbox = hbox,
        .spawn_scale = 1.f,
        .dead_scale = 0.f,
        .spawning = true,
        .dying = false,
        .dead = false,
        .anim_speed = 4.f,
	};

	ASSERT(load_texture(&tm, lua_getstring(L, "BULLET_SHEET_TEXPATH"), &b.tex), "Bullet Sheet Texture fail");
    ASSERT(init_sprite_from_sheet(&b.spr, b.tex, tex_offset, tex_size, hframes, vframes), "SPRITE INIT FAILURE");
    center_sprite_origin(&b.spr);
    set_sprite_scale_scalar(&b.spr, 2.f);

	set_bullet_speed(&b, speed, speed, speed, 0.f);

	return b;
}

void set_bullet_speed(Bullet *b, float speed, float min, float max, float delta) {
	b->speed = speed;
	b->min_speed = min;
	b->max_speed = max;
	b->speed_delta = delta;
}

void update_bullet(Bullet *b) {
    animate_sprite_hframes(&b->spr, modified_delta);
    b->spr.pos = b->pos;

    b->spr.rotation = RAD2DEG*v2_radians(b->dir) + 90;

    if (b->spawning) {
        float s = b->spr.scale.x;
        s -= modified_delta * b->anim_speed;
        set_sprite_scale_scalar(&b->spr, s);
        if (s <= b->spawn_scale) {
            b->spawning = false;
            set_sprite_scale_scalar(&b->spr, b->spawn_scale);
        }
    } 

    if (b->dying) {
        float s = b->spr.scale.x;
        s -= modified_delta * b->anim_speed;
        set_sprite_scale_scalar(&b->spr, s);
        if (s <= b->dead_scale) {
            b->dying = false;
            b->dead = true;
        }
    } else if (!b->dead) {
        b->pos = v2_add(b->pos, v2_scale(b->dir, modified_delta * b->speed));

        b->speed += b->speed_delta * modified_delta;
        if (b->speed < b->min_speed) b->speed = b->min_speed;
        if (b->speed > b->max_speed) b->speed = b->max_speed;
    }
}

void draw_bullet(Bullet *b) {
    b->spr.tint.a = 255;
    if (b->spawning) b->spr.tint.a = mapf(b->spr.scale.x, 2.f, b->spawn_scale, 0, 255);
    if (b->dying)    b->spr.tint.a = mapf(b->spr.scale.x, b->spawn_scale, b->dead_scale, 0, 255);

    draw_sprite(&b->spr);

    if (DEBUG_DRAW) {
        draw_hitbox_offsetted(&b->hitbox, b->pos);
    }
}

Bullet parse_bullet_from_lua(lua_State *L) {
    // log_debug("BEGIN PARSING BULLET FROM LUA: %d", lua_gettop(L));

    lua_getfield(L, -1, "x");
    float x = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "y");
    float y = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "texname");
    const char *texname = lua_tostring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "hframes");
    int hframes = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "vframes");
    int vframes = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "min_speed");
    float min_speed = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "max_speed");
    float max_speed = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "speed");
    float speed = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "speed_dt");
    float speed_dt = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "dir_degrees");
    float dir_degrees = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "hitbox");
    Hitbox hbox = hitbox_from_lua(L);
    lua_pop(L, 1);

    lua_getfield(L, -1, "tex_offset_x");
    float tex_x = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "tex_offset_y");
    float tex_y = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "tex_w");
    float tex_w = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "tex_h");
    float tex_h = lua_tonumber(L, -1);
    lua_pop(L, 1);

    Bullet b = make_bullet(v2(x, y), v2i(tex_x, tex_y), v2i(tex_w, tex_h), hframes, vframes, dir_degrees, speed, hbox);
    set_bullet_speed(&b, speed, min_speed, max_speed, speed_dt);

    // log_debug("END PARSING BULLET FROM LUA: %d", lua_gettop(L));
    return b;
}
