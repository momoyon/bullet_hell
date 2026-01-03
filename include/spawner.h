#ifndef SPAWNER_H_
#define SPAWNER_H_

#include <engine.h>
#include <lualib.h>

typedef struct Spawner Spawner;
typedef struct Spawners Spawners;

struct Spawner {
	float start_time;
    Vector2 pos;
    Alarm alarm;
	int spawn_count;
};

struct Spawners {
    Spawner *items;
    size_t count;
    size_t capacity;
};

Spawner make_spawner(Vector2 pos, float start_time, float spawn_rate, int count);
void update_spawner(Spawner *s);
void draw_spawner(Spawner *s, float a);
bool save_spawners_to_lua(Spawners *spawners, const char *filepath);
bool load_spawners_from_lua(Spawners *spawners, const char *filepath);
void define_spawner_struct_in_lua(lua_State *L);
Spawner parse_spawner_from_lua(lua_State *L);

#endif // SPAWNER_H_
