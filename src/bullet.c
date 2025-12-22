#include "hitbox.h"
#include "raylib.h"
#include <bullet.h>
#include <config.h>
#include <engine.h>

Bullet make_bullet(Vector2 pos, const char *texpath, int hframes, int vframes, float direction_degrees, float speed, Hitbox hbox) {
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

	ASSERT(load_texture(&tm, texpath, &b.tex), "Texture fail");
    ASSERT(init_sprite(&b.spr, b.tex, hframes, vframes), "SPRITE INIT FAILURE");
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
    animate_sprite_hframes(&b->spr, GetFrameTime());
    b->spr.pos = b->pos;

    if (b->spawning) {
        float s = b->spr.scale.x;
        s -= GetFrameTime() * b->anim_speed;
        set_sprite_scale_scalar(&b->spr, s);
        if (s <= b->spawn_scale) {
            b->spawning = false;
        }
    } 

    if (b->dying) {
        float s = b->spr.scale.x;
        s -= GetFrameTime() * b->anim_speed;
        set_sprite_scale_scalar(&b->spr, s);
        if (s <= b->dead_scale) {
            b->dying = false;
            b->dead = true;
        }
    } else if (!b->dead) {
        b->pos = v2_add(b->pos, v2_scale(b->dir, GetFrameTime() * b->speed));

        b->speed += b->speed_delta * GetFrameTime();
        if (b->speed < b->min_speed) b->speed = b->min_speed;
        if (b->speed > b->max_speed) b->speed = b->max_speed;
    }
}

void draw_bullet(Bullet *b) {

    if (b->spawning) b->spr.tint.a = mapf(b->spr.scale.x, 2.f, b->spawn_scale, 0, 255);
    if (b->dying)    b->spr.tint.a = mapf(b->spr.scale.x, b->spawn_scale, b->dead_scale, 0, 255);

    draw_sprite(&b->spr);

    if (DEBUG_DRAW) {
        draw_hitbox_offsetted(&b->hitbox, b->pos);
    }
}
