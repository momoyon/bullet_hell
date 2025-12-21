#include "common.h"
#include "control_config.h"
#include "hitbox.h"
#include "raylib.h"
#include "shot.h"
#include <entity.h>
#include <config.h>

Entity make_entity(Vector2 pos, float speed, Hitbox hitbox) {
	return (Entity) {
		.pos = pos,
		.speed = speed,
        .hitbox = hitbox,
	};
}

Entity make_player(Shots *shots_ptr, Vector2 pos, float fire_rate, float unfocus_speed, float focus_speed, const char *texpath, Hitbox hitbox, float shot_speed, float shot_hitbox, const char *shot_texpath) {
    Entity res = {
        .fire_rate = fire_rate,
        .shots_ptr = shots_ptr,
		.is_player = true,
		.pos = pos,
		.speed = unfocus_speed,
		.unfocus_speed = unfocus_speed,
		.focus_speed = focus_speed,
        .hitbox = hitbox,
        .shot_speed = shot_speed,
        .shot_hitbox = shot_hitbox,
        .shot_texpath = shot_texpath,
	};

    load_texture(&tm, texpath, &res.tex);

    res.fire_alarm.alarm_time = fire_rate;

    return res;
}

void control_entity(Entity *e, Control controls) {
	if (on_action_held(&controls, ACTION_MOVE_UP)) {
		e->pos.y -= e->speed * GetFrameTime();
	}
	if (on_action_held(&controls, ACTION_MOVE_DOWN)) {
		e->pos.y += e->speed * GetFrameTime();
	}
	if (on_action_held(&controls, ACTION_MOVE_LEFT)) {
		e->pos.x -= e->speed * GetFrameTime();
	}
	if (on_action_held(&controls, ACTION_MOVE_RIGHT)) {
		e->pos.x += e->speed * GetFrameTime();
	}

	if (e->is_player) {
        ASSERT(e->shots_ptr, "SHOTS PTR IS NOT SET!");
		if (on_action_held(&controls, ACTION_FOCUS)) {
			e->speed = e->focus_speed;
		} else {
			e->speed = e->unfocus_speed;
		}

        if (on_action_held(&controls, ACTION_FIRE) && on_alarm(&e->fire_alarm, GetFrameTime())) {
            Shot shot = make_shot(e->pos, 270, e->shot_speed, e->shot_hitbox, e->shot_texpath);
            darr_append((*e->shots_ptr), shot);
            log_debug("FIRE");
        }
	}
}

void draw_entity(Entity *e) {
    if (IsTextureReady(e->tex)) {
        draw_texture_centered(e->tex, e->pos, v2xx(SPRITE_SCALE), 0, WHITE);
    }

	if (DEBUG_DRAW) {
        draw_hitbox_offsetted(&e->hitbox, e->pos);
        DrawCircleV(e->pos, 2.f, RED);
	}
}
