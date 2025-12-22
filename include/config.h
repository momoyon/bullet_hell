#ifndef _CONFIG_H_
#define _CONFIG_H_

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

#define CAM_SPEED 100.f
#define CAMERA_DEFAULT_ZOOM 1.5f
#define SPRITE_SCALE 2.f
#define TEXTURE_PATH "resources/gfx/"
#define HITBOX_PATH "resources/hbox/"
#define SCRIPT_PATH "resources/scripts/"

/// NOTE: RUMIA
#define RUMIA_SHOT_TEXPATH "resources/gfx/rumia_shot.png"
#define RUMIA_SHOT_SPEED 1500

#endif // _CONFIG_H_
