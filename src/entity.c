#include <entity.h>
#include <config.h>

Entity make_entity(Vector2 pos, float speed) {
	return (Entity) {
		.pos = pos,
		.speed = speed,
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
}

void draw_entity(Entity *e) {
	if (DEBUG_DRAW) {
		DrawCircleV(e->pos, 16.f, RED);
	}
}
