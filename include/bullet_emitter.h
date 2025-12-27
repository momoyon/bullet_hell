#ifndef BULLET_EMITTER_H_
#define BULLET_EMITTER_H_
#include <bullet.h>
#include <engine.h>

#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

typedef struct Bullet_emitter Bullet_emitter;
typedef Bullets(*Bullet_pattern_fn)(Vector2, void *);

struct Bullet_emitter {
	Bullets *bullets_ptr;
	Alarm alarm;
	Vector2 pos;
    const char *lua_pattern_funcname;
    lua_State *L;
	void *userdata;
};

Bullet_emitter make_bullet_emitter(lua_State *L, Vector2 pos, Bullets *bullets_ptr, float fire_rate, const char *pattern_funcname, void *userdata);
void update_bullet_emitter(Bullet_emitter *em);

#endif // BULLET_EMITTER_H_
