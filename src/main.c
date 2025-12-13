#include <config.h>
#include <entity.h>
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

#ifdef DEBUG
bool DEBUG_DRAW = true;
#else
bool DEBUG_DRAW = false;
#endif // DEBUG

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

	Control player_controls = {0};
	add_control(&player_controls, KEY_UP, ACTION_MOVE_UP);
	add_control(&player_controls, KEY_DOWN, ACTION_MOVE_DOWN);
	add_control(&player_controls, KEY_LEFT, ACTION_MOVE_LEFT);
	add_control(&player_controls, KEY_RIGHT, ACTION_MOVE_RIGHT);
	add_control(&player_controls, KEY_LEFT_SHIFT, ACTION_FOCUS);
	add_control(&player_controls, KEY_Z, ACTION_FIRE);

	add_control(&player_controls, KEY_Z, ACTION_UI_CONFIRM);
	add_control(&player_controls, KEY_UP, ACTION_UI_MOVE_UP);
	add_control(&player_controls, KEY_DOWN, ACTION_UI_MOVE_DOWN);
	add_control(&player_controls, KEY_LEFT, ACTION_UI_MOVE_LEFT);
	add_control(&player_controls, KEY_RIGHT, ACTION_UI_MOVE_RIGHT);

	arena      = arena_make(0);
	temp_arena = arena_make(0);
	str_arena  = arena_make(4*1024);

	Entity player = make_entity(v2(WIDTH*0.5, HEIGHT*0.5), 150.f);
	player.unfocus_speed = 400.f;
	player.focus_speed = 200.f;

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
		if (IsKeyPressed(KEY_GRAVE)) DEBUG_DRAW = !DEBUG_DRAW;

		// Update
		control_entity(&player, player_controls);
		if (on_action_held(&player_controls, ACTION_FOCUS)) {
			player.speed = player.focus_speed;
		} else {
			player.speed = player.unfocus_speed;
		}

		// State-specific Update

		// Draw
		ClearBackground(BLACK);

		draw_entity(&player);
		DrawCircleV(m, 16.f, GREEN);

        EndTextureMode();
        draw_ren_tex(ren_tex, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
	}

	close_window(ren_tex);
	return 0;
}

