#ifndef ENEMY_H_
#define ENEMY_H_

#include <engine.h>

typedef struct Enemy Enemy;
typedef struct Enemies Enemies;

struct Enemy {
	Vector2 pos;

	Texture2D tex;

	float hitbox_radius;
};

struct Enemies {
	Enemy *items;
	size_t count;
	size_t capacity;
};

Enemy make_enemy(Vector2 pos, const char *tex_path, float hitbox_radius);
void draw_enemy(Enemy *e);


#endif // ENEMY_H_
