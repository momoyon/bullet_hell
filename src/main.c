#include "raylib.h"
#include <config.h>
#include <common.h>
#include <entity.h>
#include <bullet.h>
#include <hitbox.h>
#include <bullet_emitter.h>

// #include <common.h>

#define COMMONLIB_REMOVE_PREFIX
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>

#define ENGINE_IMPLEMENTATION
#include <engine.h>

int SCREEN_HEIGHT  = 960;
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
Bullets bullets = {0};
Shots shots = {0};

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

Bullets pattern1(Vector2 pos, void *userdata) {
Bullets _bullets = {0};
	if (userdata == NULL) {
		log_error("userdata is NULL; expected angle (float *)!");
		return _bullets;
	}
	float *angle = (float*)userdata;
	Bullet b = make_bullet(pos, *angle, 100.f, 4.f, 8.f);
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

    load_all_textures();

    Texture2D title_screen_tex = {0};
    const char *title_screen_tex_path = "resources/gfx/title_screen.png";
    load_texture(&tm, title_screen_tex_path, &title_screen_tex);
    if (!IsTextureReady(title_screen_tex)) {
        log_debug("Failed to load %s", title_screen_tex_path);
        return 1;
    }

    log_debug("Title Screen Texture Size: %dx%d (%fx%f)", title_screen_tex.width, title_screen_tex.height,
             title_screen_tex.width * SPRITE_SCALE, title_screen_tex.height * SPRITE_SCALE);

	arena      = arena_make(0);
	temp_arena = arena_make(0);
	str_arena  = arena_make(4*1024);

    Hitbox player_hitbox = {
        .pos = {0},
        .size = {16, 16},
    };
	Entity player = make_player(&shots, v2(WIDTH*0.5, HEIGHT*0.5), 0.05f, 400.f, 200.f, "resources/gfx/bullet.png", player_hitbox, RUMIA_SHOT_SPEED, 8.f, RUMIA_SHOT_TEXPATH);

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
    Hitbox hbox = { .pos = v2xx(0), .size = { 50, 50 }};

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

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            const char *path = "test.hitbox";
            if (IsKeyPressed(KEY_S)) {
                if (save_hitbox_to_file(&hbox, path)    ) {
                    log_debug("Saved hbox to %s: %fx%f at %fx%f", path, hbox.size.x, hbox.size.y, hbox.pos.x, hbox.pos.y);
                }
            }
            if (IsKeyPressed(KEY_L)) {
                if (load_hitbox_from_file(&hbox, path)) {
                    log_debug("Loaded hbox from %s: %fx%f at %fx%f", path, hbox.size.x, hbox.size.y, hbox.pos.x, hbox.pos.y);
                }
            }
        }

		// Update
        /// @DEBUG
        if (IsKeyDown(KEY_P)) hbox.pos = m;
        if (IsKeyDown(KEY_S)) {

            hbox.size.x = m.x - hbox.pos.x;
            hbox.size.y = m.y - hbox.pos.y;
        }
		control_entity(&player, player_controls);
		bind(&player.pos, player.hitbox.size, bounds);
        // Update Bullets
		for (int i = 0; i < bullets.count; ++i) {
			Bullet *b = &bullets.items[i];
			update_bullet(b);

			// Delete when outofbounds
			if (!CheckCollisionPointRec(b->pos, bounds)) {
				darr_delete(bullets, Bullet, i);
			}
		}

        // Update Shots
		for (int i = 0; i < shots.count; ++i) {
			Shot *sh = &shots.items[i];
			update_shot(sh);

			// Delete when outofbounds
			if (!CheckCollisionPointRec(sh->pos, bounds)) {
				darr_delete(shots, Shot, i);
			}
		}

		// State-specific Update

		// Draw
		ClearBackground(BLACK);
        /// @DEBUG
        DrawTextureEx(title_screen_tex, v2(0,0), 0, SPRITE_SCALE, WHITE);
        draw_hitbox(&hbox);

		draw_entity(&player);
		DrawCircleV(m, 16.f, GREEN);

		for (int i = 0; i < bullets.count; ++i) {
			Bullet *b = &bullets.items[i];
			draw_bullet(b);
		}
		for (int i = 0; i < shots.count; ++i) {
			Shot *sh = &shots.items[i];
			draw_shot(sh);
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

