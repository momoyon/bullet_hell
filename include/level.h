#ifndef LEVEL_H_
#define LEVEL_H_

#include <engine.h>
#include <level_action.h>
#include <spawner.h>
#include <lualib.h>
#include <lauxlib.h>

typedef struct Level Level;
typedef struct Levels Levels;

struct Level {
    const char *name;
    float time;
    bool paused;
    Spawners spawners;
	Level_actions actions;
};

void define_level_struct_in_lua(lua_State *L);
Level make_level(const char *name);
bool save_level_to_lua(Level *l, const char *name, const char *script_path);
bool load_level_from_lua(Level *l, lua_State *L, const char *level_name);

struct Levels {
    Level *items;
    size_t count;
    size_t capacity;
};

#endif // LEVEL_H_
