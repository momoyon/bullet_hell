#ifndef BULLET_H_
#define BULLET_H_
#include <raylib.h>
#include <hitbox.h>
#include <engine.h>
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

typedef struct Bullet Bullet;
typedef struct Bullets Bullets;

struct Bullet {
	Vector2 pos;
	Vector2 dir;

    Hitbox hitbox;

	float speed, min_speed, max_speed, speed_delta;

	Texture2D tex;
    Sprite spr;
    bool dead;
};

struct Bullets {
	Bullet *items;
	size_t count;
	size_t capacity;
};

Bullet make_bullet(Vector2 pos, const char *texpath, float direction_degrees, float speed, Hitbox hbox);
void set_bullet_speed(Bullet *b, float speed, float min, float max, float delta);
void update_bullet(Bullet *b);
void draw_bullet(Bullet *b);

#endif // BULLET_H_
