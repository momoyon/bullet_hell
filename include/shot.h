#ifndef SHOT_H_
#define SHOT_H_
#include <raylib.h>
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

typedef struct Shot Shot;
typedef struct Shots Shots;

struct Shot {
	Vector2 pos;
	Vector2 dir;

	float hitbox_radius;

	float speed, min_speed, max_speed, speed_delta;

	Texture2D tex;
};

struct Shots {
	Shot *items;
	size_t count;
	size_t capacity;
};

Shot make_shot(Vector2 pos, float direction_degrees, float speed, float hitbox_radius, const char *texpath);
void set_shot_speed(Shot *b, float speed, float min, float max, float delta);
void update_shot(Shot *b);
void draw_shot(Shot *b);

#endif // SHOT_H_
