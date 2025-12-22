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


typedef enum State State;

enum State {
    STATE_NORMAL,
    STATE_EDIT_HITBOX,
    STATE_COUNT,
};

const char *state_as_str(const State state) {
    switch (state) {
        case STATE_NORMAL: return "Normal";
        case STATE_EDIT_HITBOX: return "Edit Hitbox";
        case STATE_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }
}

#define CHANGE_STATE(to) do {\
        prev_state = current_state;\
        current_state = to;\
    } while (0)

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

    int GLOBAL_FS=24;

    State current_state = STATE_NORMAL;
    State prev_state    = STATE_NORMAL;

    CHANGE_STATE(STATE_NORMAL);

    /// Edit Hitbox Vars
    Hitbox editing_hitbox = { .pos = {100, 100}, .size = {16,16}};
#define EDITING_HITBOX_FILEPATH_MAX (1024)
    char editing_hitbox_filepath[EDITING_HITBOX_FILEPATH_MAX] = {0};
    int editing_hitbox_filepath_cursor = 0;
    Vector2 moving_offset = {0};
    bool editing_hitbox_filepath_input_focused = false;
    int P=10;
    Rectangle editing_hitbox_filepath_rect = {
        .x = P,
        .y = (HEIGHT*0.5)+P,
        .width = WIDTH-2*P,
        .height = GLOBAL_FS,
    };

    bool editing_hitbox_texpath_input_focused = false;
    Texture2D editing_hitbox_texture = {0};
    Vector2 editing_hitbox_texture_pos = {0};
#define EDITING_HITBOX_TEXPATH_MAX (1024)
    char editing_hitbox_texpath[EDITING_HITBOX_FILEPATH_MAX] = {0};
    int editing_hitbox_texpath_cursor = 0;
    Rectangle editing_hitbox_texpath_rect = {
        .x = P,
        .y = editing_hitbox_filepath_rect.y+editing_hitbox_filepath_rect.height+P,
        .width = WIDTH-2*P,
        .height = GLOBAL_FS,
    };

	/// @DEBUG
	float angle = 0;
	Bullet_emitter em = make_bullet_emitter(v2(WIDTH*0.5, HEIGHT*0.5), &bullets, 0.05, pattern1, (void*)&angle);

	while (!WindowShouldClose()) {
		arena_reset(&temp_arena);
		arena_reset(&str_arena);

        BeginDrawing();
        Vector2 m = get_mpos_scaled(SCREEN_SCALE);

		// Input
		if (IsKeyPressed(KEY_GRAVE)) DEBUG_DRAW = !DEBUG_DRAW;
        if (IsKeyPressed(KEY_TAB)) {
            int next = current_state + 1;
            if (next >= STATE_COUNT) next = 0;
            CHANGE_STATE(next);
        }

        // State-specific Input
        switch (current_state) {
            case STATE_NORMAL: {
                /// @DEBUG
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    em.pos = m;
                    update_bullet_emitter(&em);
                }
            } break;
            case STATE_EDIT_HITBOX: {
                if (!editing_hitbox_texpath_input_focused && IsKeyPressed(KEY_Z)) {
                    editing_hitbox_filepath_input_focused = true;
                    editing_hitbox_texpath_input_focused = false;
                }
                if (!editing_hitbox_filepath_input_focused && IsKeyPressed(KEY_X)) {
                    editing_hitbox_filepath_input_focused = false;
                    editing_hitbox_texpath_input_focused = true;
                }

                if (editing_hitbox_filepath_input_focused) {
                    if (input_to_buff(editing_hitbox_filepath, EDITING_HITBOX_FILEPATH_MAX, &editing_hitbox_filepath_cursor)) {
                        editing_hitbox_filepath_input_focused = false;
                    }
                } else if (editing_hitbox_texpath_input_focused ) {
                    if (input_to_buff(editing_hitbox_texpath, EDITING_HITBOX_TEXPATH_MAX, &editing_hitbox_texpath_cursor)) {
                        editing_hitbox_texpath_input_focused = false;
                        memset(&editing_hitbox_texture, 0, sizeof(Texture2D));
                        if (!load_texture_(&tm, editing_hitbox_texpath, &editing_hitbox_texture, true)) {
                            log_error("Failed to load texture %s", editing_hitbox_texpath);
                        }
                    }
                } else {
                    // Size
                    if (is_key_down_ON_key_down_OR_key_pressed_repeat(KEY_D, KEY_LEFT_ALT)) {
                        editing_hitbox.size.x++;
                    }
                    if (is_key_down_ON_key_down_OR_key_pressed_repeat(KEY_A, KEY_LEFT_ALT)) {
                        editing_hitbox.size.x--;
                    }
                    if (is_key_down_ON_key_down_OR_key_pressed_repeat(KEY_S, KEY_LEFT_ALT)) {
                        editing_hitbox.size.y++;
                    }
                    if (is_key_down_ON_key_down_OR_key_pressed_repeat(KEY_W, KEY_LEFT_ALT)) {
                        editing_hitbox.size.y--;
                    }

                    // Move
                    if (is_key_down_ON_key_down_OR_key_pressed_repeat(KEY_LEFT, KEY_LEFT_ALT)) {
                        editing_hitbox.pos.x--;
                    }
                    if (is_key_down_ON_key_down_OR_key_pressed_repeat(KEY_RIGHT, KEY_LEFT_ALT)) {
                        editing_hitbox.pos.x++;
                    }
                    if (is_key_down_ON_key_down_OR_key_pressed_repeat(KEY_UP, KEY_LEFT_ALT)) {
                        editing_hitbox.pos.y--;
                    }
                    if (is_key_down_ON_key_down_OR_key_pressed_repeat(KEY_DOWN, KEY_LEFT_ALT)) {
                        editing_hitbox.pos.y++;
                    }


                    // Save
                    if (IsKeyDown(KEY_LEFT_CONTROL)) {
                        if (IsKeyPressed(KEY_S)) {
                            if (save_hitbox_to_file(&editing_hitbox, editing_hitbox_filepath)) {
                                log_debug("Successfully saved hitbox to %s", editing_hitbox_filepath);
                            } else {
                                log_error("Failed to save hitbox to %s", editing_hitbox_filepath);
                            }
                        }
                        if (IsKeyPressed(KEY_L)) {
                            if (load_hitbox_from_file(&editing_hitbox, editing_hitbox_filepath)) {
                                log_debug("Successfully loaded hitbox from %s", editing_hitbox_filepath);
                            } else {
                                log_error("Failed to load hitbox from %s", editing_hitbox_filepath);
                            }
                        }
                    }
                }


            } break;
            case STATE_COUNT:
            default: ASSERT(false, "UNREACHABLE!");
        }

		// Update

		// State-specific Update
        switch (current_state) {
            case STATE_NORMAL: {
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
            } break;
            case STATE_EDIT_HITBOX: {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))   { moving_offset = v2_sub(m, editing_hitbox.pos); }

                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    editing_hitbox.pos = v2_sub(m, moving_offset);
                }

                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))  { 
                    editing_hitbox_texture_pos = m;
                }
            } break;
            case STATE_COUNT:
            default: ASSERT(false, "UNREACHABLE!");
        }

		// Draw
		ClearBackground(BLACK);
        /// @DEBUG
        DrawTextureEx(title_screen_tex, v2(0,0), 0, SPRITE_SCALE, WHITE);


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

        // Mode-specific Draw
        switch (current_state) {
            case STATE_NORMAL: {
                DrawRectangleLinesEx(bounds, 1.f, WHITE);

            } break;
            case STATE_EDIT_HITBOX: {
                DrawRectangleV(v2xx(0), v2(WIDTH, HEIGHT), ColorAlpha(BLACK, 1));

                draw_hitbox(&editing_hitbox);

                int fs = 24;
                draw_text_aligned(font, 
                                  arena_alloc_str(str_arena, "%dx%d [%dx%d]", 
                                                  (int)editing_hitbox.pos.x, 
                                                  (int)editing_hitbox.pos.y, 
                                                  (int)editing_hitbox.size.x, 
                                                  (int)editing_hitbox.size.y), 
                                  v2_add(editing_hitbox.pos, v2(0, -fs)), fs, TEXT_ALIGN_V_BOTTOM, TEXT_ALIGN_H_LEFT, WHITE);

                draw_text(font, editing_hitbox_filepath, v2(editing_hitbox_filepath_rect.x, editing_hitbox_filepath_rect.y), fs, editing_hitbox_filepath_input_focused ? YELLOW : GRAY);
                DrawRectangleLinesEx(editing_hitbox_filepath_rect, 1, editing_hitbox_filepath_input_focused ? YELLOW : GRAY);

                draw_text(font, editing_hitbox_texpath, v2(editing_hitbox_texpath_rect.x, editing_hitbox_texpath_rect.y), fs, editing_hitbox_texpath_input_focused ? YELLOW : GRAY);
                DrawRectangleLinesEx(editing_hitbox_texpath_rect, 1, editing_hitbox_texpath_input_focused ? YELLOW : GRAY);

                if (IsTextureReady(editing_hitbox_texture)) {
                    DrawTextureEx(editing_hitbox_texture, editing_hitbox_texture_pos, 0, SPRITE_SCALE, WHITE);
                }
            } break;
            case STATE_COUNT:
            default: ASSERT(false, "UNREACHABLE!");
        }

        if (DEBUG_DRAW) {
            Vector2 p = {10, 10};
            int font_size = 24;
            draw_info_text(&p, arena_alloc_str(str_arena, "State: %s", state_as_str(current_state)), font_size, WHITE);

            DrawFPS(0, 0);
        }

        EndTextureMode();
        draw_ren_tex(ren_tex, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
	}

	close_window(ren_tex);
	return 0;
}

