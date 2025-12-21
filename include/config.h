#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <raylib.h>
#include <stdbool.h>
#include <engine.h>
#include <bullet.h>
#include <shot.h>

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
extern Camera2D cam;
extern float cam_zoom;

#define CAM_SPEED 100.f
#define CAMERA_DEFAULT_ZOOM 1.5f
#define SPRITE_SCALE 2.f

extern Bullets bullets;
extern Shots shots;

extern Arena arena;
extern Arena temp_arena;
extern Arena str_arena;

#endif // _CONFIG_H_
