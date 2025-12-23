#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <lua.h>
#include <raylib.h>
#include <stdbool.h>
#include <engine.h>
#include <bullet.h>
#include <entity.h>

#include <control_config.h>

extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;
extern float SCREEN_SCALE;

extern int HEIGHT;
extern int WIDTH;

extern bool DEBUG_DRAW;

extern RenderTexture2D ren_tex;
extern Texture_manager tm;
extern Font font;
extern float delta;
extern float modified_delta;
extern float delta_modification;

extern Bullets bullets;
extern Bullets shots;
extern Entities enemies;

extern Arena arena;
extern Arena temp_arena;
extern Arena str_arena;

extern lua_State *L;

#endif // _CONFIG_H_
