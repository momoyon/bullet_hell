#include "control_config.h"
#include "raylib.h"
#include "shot.h"
#include <entity.h>
#include <config.h>

Entity make_entity(Vector2 pos, float speed, float radius) {
	return (Entity) {
		.pos = pos,
		.speed = speed,
		.radius = radius,
	};
}

Entity make_player(Shots *shots_ptr, Vector2 pos, float fire_rate, float unfocus_speed, float focus_speed, float radius, float hitbox_radius, float shot_speed, float shot_hitbox, const char *shot_texpath) {
    Entity res = {
        .fire_rate = fire_rate,
        .shots_ptr = shots_ptr,
		.is_player = true,
		.pos = pos,
		.speed = unfocus_speed,
		.unfocus_speed = unfocus_speed,
		.focus_speed = focus_speed,
		.radius = radius,
		.hitbox_radius = hitbox_radius,
        .shot_speed = shot_speed,
        .shot_hitbox = shot_hitbox,
        .shot_texpath = shot_texpath,
	};

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
	if (DEBUG_DRAW) {
		DrawCircleV(e->pos, e->radius, RED);
	}
}
