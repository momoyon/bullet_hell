#include <entity.h>
#include <config.h>

Entity make_entity(Vector2 pos, float speed, float radius) {
	return (Entity) {
		.pos = pos,
		.speed = speed,
		.radius = radius,
	};
}

Entity make_player(Vector2 pos, float unfocus_speed, float focus_speed, float radius, float hitbox_radius) {
	return (Entity) {
		.is_player = true,
		.pos = pos,
		.speed = unfocus_speed,
		.unfocus_speed = unfocus_speed,
		.focus_speed = focus_speed,
		.radius = radius,
		.hitbox_radius = hitbox_radius,
	};
	
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
		if (on_action_held(&controls, ACTION_FOCUS)) {
			e->speed = e->focus_speed;
		} else {
			e->speed = e->unfocus_speed;
		}
	}
}

void draw_entity(Entity *e) {
	if (DEBUG_DRAW) {
		DrawCircleV(e->pos, e->radius, RED);
	}
}
