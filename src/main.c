#include <config.h>
// #include <common.h>

#define COMMONLIB_REMOVE_PREFIX
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>

#define ENGINE_IMPLEMENTATION
#include <engine.h>

int SCREEN_HEIGHT  = 720;
int SCREEN_WIDTH   = 1280;
float SCREEN_SCALE = 1;

int HEIGHT;
int WIDTH;

bool DEBUG_DRAW;

RenderTexture2D ren_tex;
Texture_manager tm;
Font font;
Camera2D cam;
float cam_zoom = 1.0;
Arena arena;
Arena temp_arena;
Arena str_arena;

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

int main(void) {
	ren_tex = init_window(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE, "Bullet Hell", &WIDTH, &HEIGHT);
	SetExitKey(0);

	arena      = arena_make(0);
	temp_arena = arena_make(0);
	str_arena  = arena_make(4*1024);

	font = GetFontDefault();
	if (!IsFontReady(font)) {
		log_error("Failed to get default font?");
		exit(1);
	}


	while (!WindowShouldClose()) {
		cam.zoom = cam_zoom;
		arena_reset(&temp_arena);

        BeginDrawing();
        Vector2 m = get_mpos_scaled(SCREEN_SCALE);
		Vector2 m_world = GetScreenToWorld2D(m, cam);

		// Input

		// Update

		// State-specific Update

		// Draw

        EndTextureMode();
        draw_ren_tex(ren_tex, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
	}

	close_window(ren_tex);
	return 0;
}

