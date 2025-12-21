#ifndef ENTITY_H_
#define ENTITY_H_

#include <raylib.h>
#include <engine.h>
#include <shot.h>
#include <control_config.h>

typedef struct Entity Entity;

struct Entity {
	bool is_player;
	Vector2 pos;
	float speed;
	float unfocus_speed; // Only for player
	float focus_speed; // Only for player
	float radius;
	float hitbox_radius; // Only for player
	Shots *shots_ptr; // Only for player
    float fire_rate;
    Alarm fire_alarm;
    float shot_speed; // Only for player
    float shot_hitbox; // Only for player
    const char *shot_texpath; // Only for player
};

Entity make_entity(Vector2 pos, float speed, float radius);
Entity make_player(Shots *shots_ptr, Vector2 pos, float fire_rate, float unfocus_speed, float focus_speed, float radius, float hitbox_radius, float shot_speed, float shot_hitbox, const char *shot_texpath);
void control_entity(Entity *e, Control controls);
void draw_entity(Entity *e);

#endif // ENTITY_H_
