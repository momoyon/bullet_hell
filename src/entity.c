#include "common.h"
#include "control_config.h"
#include "hitbox.h"
#include "raylib.h"
#include <entity.h>
#include <config.h>

Entity make_entity(Vector2 pos, const char *texpath, int hframes, int vframes, float speed, Hitbox hitbox) {
	Entity res = {
		.pos = pos,
		.speed = speed,
        .hitbox = hitbox,
        .texpath = texpath,
	};
    load_texture(&tm, texpath, &res.tex);
    ASSERT(init_sprite(&res.spr, res.tex, hframes, vframes), "FAILED SPRITE INIT");
    center_sprite_origin(&res.spr);

    return res;
}

Entity make_player(Bullets *shots_ptr, Vector2 pos, float fire_rate, float unfocus_speed, float focus_speed, const char *texpath, int hframes, int vframes, Hitbox hitbox, Hitbox bounding_hbox, float shot_speed, Hitbox shot_hitbox, const char *shot_texpath) {
    Entity res = {
        .fire_rate = fire_rate,
        .shots_ptr = shots_ptr,
		.is_player = true,
		.pos = pos,
		.speed = unfocus_speed,
		.unfocus_speed = unfocus_speed,
		.focus_speed = focus_speed,
        .hitbox = hitbox,
        .bounding_hitbox = bounding_hbox,
        .shot_speed = shot_speed,
        .shot_hitbox = shot_hitbox,
        .shot_texpath = shot_texpath,
	};

    load_texture(&tm, texpath, &res.tex);

    ASSERT(init_sprite(&res.spr, res.tex, hframes, vframes), "FAILED SPRITE INIT");
    center_sprite_origin(&res.spr);

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
            Vector2 spawn_pos = v2(e->pos.x, e->pos.y - e->spr.height * 0.5);
            Bullet shot = make_bullet(spawn_pos, TEXTURE_PATH"rumia_shot.png", 1, 1, 270, e->shot_speed, e->shot_hitbox);
            darr_append((*e->shots_ptr), shot);
        }
	}
}

void update_entity(Entity *e) {
    e->spr.pos = e->pos;
    animate_sprite_hframes(&e->spr, GetFrameTime());
}

void draw_entity(Entity *e) {
    if (IsTextureReady(e->tex)) {
        draw_sprite(&e->spr);
        // draw_texture_centered(e->tex, e->pos, v2xx(1), 0, WHITE);
    }

	if (DEBUG_DRAW) {
        draw_hitbox_offsetted(&e->bounding_hitbox, e->pos);
        draw_hitbox_offsetted(&e->hitbox, e->pos);
        DrawCircleV(e->pos, 2.f, RED);
	}
}
