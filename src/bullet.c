#include <bullet.h>
#include <config.h>
#include <engine.h>

Bullet make_bullet(Vector2 pos, float direction_degrees, float speed, float hitbox_radius) {
	Bullet b = {
		.pos = pos,
		.dir = v2_from_degrees(direction_degrees),
		.hitbox_radius = hitbox_radius,
	};

	ASSERT(load_texture(&tm, "resources/gfx/bullet.png", &b.tex), "Texture fail");

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
	Rectangle src = {
		.x = 0,
		.y = 0,
		.width = b->tex.width,
		.height = b->tex.height,
	};
	Rectangle dst = {
		.x = b->pos.x,
		.y = b->pos.y,
		.width = b->tex.width,
		.height = b->tex.height,
	};
	Vector2 origin = v2(b->tex.width*0.5, b->tex.height*0.5);
	DrawTexturePro(b->tex, src, dst, origin, 0, WHITE);

	if (DEBUG_DRAW) {
		DrawCircleV(b->pos, b->hitbox_radius, RED);
	}
}
