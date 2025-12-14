#include <bullet.h>
#include <engine.h>

Bullet make_bullet(Vector2 pos, float direction_degrees, float speed, float hitbox_radius, float radius) {
	Bullet b = {
		.pos = pos,
		.dir = v2_from_degrees(direction_degrees),
		.hitbox_radius = hitbox_radius,
		.radius = radius,
	};

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
	b->pos = v2_add(b->pos, v2_scale(b->dir, GetFrameTime() * b->speed));

	b->speed += b->speed_delta * GetFrameTime();
	if (b->speed < b->min_speed) b->speed = b->min_speed;
	if (b->speed > b->max_speed) b->speed = b->max_speed;
}

void draw_bullet(Bullet *b) {
	DrawCircleV(b->pos, b->radius, BLUE);
}
