#include <config.h>
#include <common.h>
#include <entity.h>
#include <bullet.h>
#include <enemy.h>
#include <bullet_emitter.h>

// #include <common.h>

#define COMMONLIB_REMOVE_PREFIX
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>

#define ENGINE_IMPLEMENTATION
#include <engine.h>

int SCREEN_HEIGHT  = 900;
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
Bullet_array bullets = {0};
Enemies enemies = {0};

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

Bullet_array pattern1(Vector2 pos, void *userdata) {
	Bullet_array _bullets = {0};
	if (userdata == NULL) {
		log_error("userdata is NULL; expected angle (float *)!");
		return _bullets;
	}
	float *angle = (float*)userdata;
	Bullet b = make_bullet(pos, *angle, 100.f, 8.f);
	set_bullet_speed(&b, 500.f, 100.f, 500.f, -200.f);
	*angle += GetFrameTime() * 400.f;

	darr_append(_bullets, b);

	return _bullets;
}

int main(void) {
	ren_tex = init_window(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE, "Bullet Hell", &WIDTH, &HEIGHT);
	SetTargetFPS(60);
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

	Entity player = make_player(v2(WIDTH*0.5, HEIGHT*0.5), 400.f, 200.f, 16.f, 4.f);

	font = GetFontDefault();
	if (!IsFontReady(font)) {
		log_error("Failed to get default font?");
		exit(1);
	}

	const float pad = 100.f;
	Rectangle bounds = {
		.x = pad,
		.y = pad,
		.width = WIDTH - (pad*2),
		.height = HEIGHT - (pad*2),
	};

	/// @DEBUG
	float angle = 0;
	Bullet_emitter em = make_bullet_emitter(v2(WIDTH*0.5, HEIGHT*0.5), &bullets, 0.05, pattern1, (void*)&angle);

	while (!WindowShouldClose()) {
		arena_reset(&temp_arena);

        BeginDrawing();
        Vector2 m = get_mpos_scaled(SCREEN_SCALE);

		// Input
		if (IsKeyPressed(KEY_GRAVE)) DEBUG_DRAW = !DEBUG_DRAW;
		/// @DEBUG
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			em.pos = m;
			update_bullet_emitter(&em);
		}
		if (IsKeyPressed(KEY_E)) {
			Enemy e = make_enemy(m, "resources/gfx/enemy.png", 16.f);
			darr_append(enemies, e);
		}

		// Update
		control_entity(&player, player_controls);
		bind(&player.pos, player.radius, bounds);
		for (int i = 0; i < bullets.count; ++i) {
			Bullet *b = &bullets.items[i];
			update_bullet(b);

			// Delete when outofbounds
			if (!CheckCollisionPointRec(b->pos, bounds)) {
				darr_delete(bullets, Bullet, i);
			}
		}
		for (int i = 0; i < enemies.count; ++i) {
			Enemy *e = &enemies.items[i];
			// update_enemy(e);

			// Delete when outofbounds
			if (!CheckCollisionPointRec(e->pos, bounds)) {
				darr_delete(enemies, Enemy, i);
			}
		}

		// State-specific Update

		// Draw
		ClearBackground(BLACK);

		draw_entity(&player);
		DrawCircleV(m, 16.f, GREEN);

		for (int i = 0; i < enemies.count; ++i) {
			Enemy *e = &enemies.items[i];
			draw_enemy(e);
		}
		for (int i = 0; i < bullets.count; ++i) {
			Bullet *b = &bullets.items[i];
			draw_bullet(b);
		}

		DrawRectangleLinesEx(bounds, 1.f, WHITE);

		DrawFPS(0, 0);

        EndTextureMode();
        draw_ren_tex(ren_tex, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
	}

	close_window(ren_tex);
	return 0;
}

