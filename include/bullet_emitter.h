#ifndef BULLET_EMITTER_H_
#define BULLET_EMITTER_H_
#include <bullet.h>
#include <engine.h>

#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

typedef struct Bullet_emitter Bullet_emitter;
typedef Bullet_array(*Bullet_pattern_fn)(Vector2, void *);

struct Bullet_emitter {
	Bullet_array *bullets_ptr;
	Alarm alarm;
	Vector2 pos;
	Bullet_pattern_fn pattern_fn;
	void *userdata;
};

Bullet_emitter make_bullet_emitter(Vector2 pos, Bullet_array *bullets_ptr, float fire_rate, Bullet_pattern_fn pattern_fn, void *userdata);
void update_bullet_emitter(Bullet_emitter *em);

#endif // BULLET_EMITTER_H_
