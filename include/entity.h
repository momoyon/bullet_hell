#ifndef ENTITY_H_
#define ENTITY_H_

#include <raylib.h>
#include <control_config.h>

typedef struct Entity Entity;

struct Entity {
	Vector2 pos;
	float speed;
	float unfocus_speed; // Only for player
	float focus_speed; // Only for player
};

Entity make_entity(Vector2 pos, float speed);
void control_entity(Entity *e, Control controls);
void draw_entity(Entity *e);

#endif // ENTITY_H_
