#ifndef BULLET_H_
#define BULLET_H_
#include <raylib.h>
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

typedef struct Bullet Bullet;
typedef struct Bullet_array Bullet_array;

struct Bullet {
	Vector2 pos;
	Vector2 dir;

	float radius;
	float hitbox_radius;

	float speed, min_speed, max_speed, speed_delta;
};

struct Bullet_array {
	Bullet *items;
	size_t count;
	size_t capacity;
};

Bullet make_bullet(Vector2 pos, float direction_degrees, float speed, float hitbox_radius, float radius);
void set_bullet_speed(Bullet *b, float speed, float min, float max, float delta);
void update_bullet(Bullet *b);
void draw_bullet(Bullet *b);

#endif // BULLET_H_
