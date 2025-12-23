#ifndef ENTITY_H_
#define ENTITY_H_

#include <raylib.h>
#include <engine.h>
#include <hitbox.h>
#include <bullet.h>
#include <control_config.h>

typedef struct Entity Entity;

struct Entity {
	bool is_player;
	Vector2 pos;
	float speed;
	float unfocus_speed; // Only for player
	float focus_speed; // Only for player
    Hitbox hitbox; // NOTE: pos is the offset from the texture
    Hitbox bounding_hitbox; // NOTE: This is the hitbox for the bounding box of the player `hitbox` is used for collision with bullets.
	Bullets *shots_ptr; // Only for player
    float fire_rate;
    Alarm fire_alarm;
    Hitbox shot_hitbox; // Only for player
    const char *shot_texpath; // Only for player
    Texture2D tex;
    Sprite spr;
    bool dead;
    const char *texpath;
};

typedef struct {
    Entity *items;
    size_t count;
    size_t capacity;
} Entities;

Entity make_entity(Vector2 pos, const char *texpath, int hframes, int vframes, float speed, Hitbox hitbox);
Entity make_player(Bullets *shots_ptr, Vector2 pos, float fire_rate, float unfocus_speed, float focus_speed, const char *texpath, int hframes, int vframes, Hitbox hitbox, Hitbox bounding_hbox, Hitbox shot_hitbox, const char *shot_texpath);
void control_entity(Entity *e, Control controls);
void update_entity(Entity *e);
void draw_entity(Entity *e);

#endif // ENTITY_H_
