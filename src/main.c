#include "raylib.h"
#include <config.h>
#include <common.h>
#include <entity.h>
#include <bullet.h>
#include <hitbox.h>
#include <bullet_emitter.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

// #include <common.h>

#define LUA_FUNCALL(nargs, nresults) \
    if (lua_pcall(L, (nargs), (nresults), 0) != LUA_OK) {\
        log_error("LUA ERROR: %s", lua_tostring(L, -1));\
        lua_pop(L, 1);\
        exit(1);\
    }

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

// Externs
RenderTexture2D ren_tex;
Texture_manager tm;
Font font;
float delta = 0.f;
float modified_delta = 0.f;
float delta_modification = 1.f;

Arena arena;
Arena temp_arena;
Arena str_arena;

Bullets bullets = {0};
Bullets shots = {0};
Entities enemies = {0};
Spawners spawners = {0};
Levels levels = {0};
int current_level = -1;

lua_State *L = NULL;

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

typedef enum State State;
typedef enum Edit_state Edit_state;

enum State {
    STATE_NORMAL,
    STATE_EDIT,
    STATE_LUA,
    STATE_COUNT,
};

enum Edit_state {
    EDSTATE_SPAWNERS,
    EDSTATE_HITBOX,
    EDSTATE_SFX,
    EDSTATE_COUNT,
};

const char *edstate_as_str(const Edit_state state) {
    switch (state) {
        case EDSTATE_SPAWNERS: return "Spawners";
        case EDSTATE_HITBOX:   return "Hitbox";
        case EDSTATE_SFX:      return "Sfx";
        case EDSTATE_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }
}

const char *state_as_str(const State state) {
    switch (state) {
        case STATE_NORMAL: return "Normal";
        case STATE_EDIT: return "Edit";
        case STATE_LUA: return "Lua";
        case STATE_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }
}

#define CHANGE_STATE(to) do {\
        prev_state = current_state;\
        current_state = to;\
    } while (0)


// #define TEXTBOX_NAME 0
typedef enum {
    EDITING_TBOX_FILEPATH,
    EDITING_TBOX_TEXPATH,
    EDITING_TBOX_TEX_OFFSET,
    EDITING_TBOX_TEX_SIZE,
    EDITING_TBOX_COUNT,
} Editing_tbox_kind;

void confirm_filepath_tbox(Hitbox *editing_hitbox, Texture2D *editing_hitbox_texture, Textbox *editing_textboxes, Textbox *tbox) {
    (void)editing_hitbox_texture;
    (void)editing_textboxes;
    (void)editing_hitbox;
    (void)tbox;
    // const char *actual_editing_hitbox_filepath = arena_alloc_str(str_arena, lua_getstring(L, "HITBOX_PATH")"%s", tbox->buff);
    // if (load_hitbox_from_file(editing_hitbox, actual_editing_hitbox_filepath)) {
    //     log_debug("Successfully loaded hitbox from %s", actual_editing_hitbox_filepath);
    // } else {
    //     log_error("Failed to load hitbox from %s", actual_editing_hitbox_filepath);
    // }
}

void confirm_texoffset_tbox(Vector2i *editing_hitbox_offset, Textbox *tbox) {
    String_view sv = SV(tbox->buff);

    if (!sv_contains_char(sv, ' ')) {
        log_error("%s is not a valid offset!: No space found", tbox->buff);
        return;
    }

    String_view x_sv = sv_lpop_until_char(&sv, ' ');
    sv_lremove(&sv, 1); // Remove SPACE

    String_view y_sv = sv_lpop_until_char(&sv, ' ');
    sv_lremove(&sv, 1); // Remove SPACE

    int x_count = -1;
    float x = sv_to_float(x_sv, &x_count);
    if (x_count == -1) {
        log_error("%s is not a valid offset!: Couldn't convert x to float", tbox->buff);
        return;
    }

    int y_count = -1;
    float y = sv_to_float(y_sv, &y_count);
    if (y_count == -1) {
        log_error("%s is not a valid offset!: Couldn't convert y to float", tbox->buff);
        return;
    }

    editing_hitbox_offset->x = x;
    editing_hitbox_offset->y = y;

    log_debug("Successfully set the texture offset to %f, %f", x, y);
}

void confirm_texsize_tbox(Vector2i *size, Textbox *tbox) {
    String_view sv = SV(tbox->buff);

    if (!sv_contains_char(sv, ' ')) {
        log_error("%s is not a valid size!: No space found", tbox->buff);
        return;
    }

    String_view x_sv = sv_lpop_until_char(&sv, ' ');
    sv_lremove(&sv, 1); // Remove SPACE

    String_view y_sv = sv_lpop_until_char(&sv, ' ');
    sv_lremove(&sv, 1); // Remove SPACE


    int x_count = -1;
    float x = sv_to_float(x_sv, &x_count);
    if (x_count == -1) {
        log_error("%s is not a valid size!: Couldn't convert x to float", tbox->buff);
        return;
    }

    int y_count = -1;
    float y = sv_to_float(y_sv, &y_count);
    if (y_count == -1) {
        log_error("%s is not a valid size!: Couldn't convert y to float", tbox->buff);
        return;
    }

    size->x = x;
    size->y = y;

    log_debug("Successfully set the texture size to %f, %f", x, y);
}

void confirm_texpath_tbox(Texture2D *editing_hitbox_texture, Vector2i *tex_size, Textbox *tbox) {
    memset(editing_hitbox_texture, 0, sizeof(Texture2D));
    const char *actual_editing_hitbox_texpath = arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "TEXTURE_PATH"), tbox->buff);
    if (!load_texture_(&tm, actual_editing_hitbox_texpath, editing_hitbox_texture, true)) {
        log_error("Failed to load texture %s", actual_editing_hitbox_texpath);
    } else {
        log_debug("Successfully loaded texture %s", actual_editing_hitbox_texpath);
    }

    tex_size->x = editing_hitbox_texture->width;
    tex_size->y = editing_hitbox_texture->height;
}

void confirm_name_tbox(Hitbox *editing_hitbox, Texture2D *editing_hitbox_texture, Textbox *editing_textboxes, Textbox *tbox, Vector2i *tex_size) {
    int written = snprintf(editing_textboxes[EDITING_TBOX_FILEPATH].buff, editing_textboxes[EDITING_TBOX_FILEPATH].buff_size, "%s.hitbox", tbox->buff);
    editing_textboxes[EDITING_TBOX_FILEPATH].cursor = written;
    written = snprintf(editing_textboxes[EDITING_TBOX_TEXPATH].buff, editing_textboxes[EDITING_TBOX_TEXPATH].buff_size, "%s.png", tbox->buff);
    editing_textboxes[EDITING_TBOX_TEXPATH].cursor = written;

    confirm_filepath_tbox(editing_hitbox, editing_hitbox_texture, editing_textboxes, &editing_textboxes[EDITING_TBOX_FILEPATH]);
    confirm_texpath_tbox(editing_hitbox_texture, tex_size, &editing_textboxes[EDITING_TBOX_TEXPATH]);
}

int main(void) {
	arena      = arena_make(0);
	temp_arena = arena_make(0);
	str_arena  = arena_make(4*1024);

    L = luaL_newstate();
    if (L == NULL) {
        log_error("Failed to init Lua State!");
        return 1;
    } else {
        log_debug("Successfully initialized Lua State!");
        luaL_openlibs(L);
    }

    // Define structs in LUA
    define_hitbox_struct_in_lua(L);
    define_bullet_struct_in_lua(L);

    load_config(L);

    refresh_hitboxes_script(L);

    luaL_dofile(L, arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "SCRIPT_PATH"), "preload.lua"));

	ren_tex = init_window(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE, "Bullet Hell", &WIDTH, &HEIGHT);
    InitAudioDevice();
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

    Texture2D tex = {0};
    load_texture(&tm, arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "TEXTURE_PATH"), "rumia_player.png"), &tex);
    Sprite spr = {0};
    init_sprite(&spr, tex, 3, 1);
    center_sprite_origin(&spr);

    Texture2D title_screen_tex = {0};
    const char *title_screen_tex_path = "resources/gfx/title_screen.png";
    load_texture(&tm, title_screen_tex_path, &title_screen_tex);
    if (!IsTextureReady(title_screen_tex)) {
        log_debug("Failed to load %s", title_screen_tex_path);
        return 1;
    }

    Hitbox player_hitbox = {0};
    if (!load_hitbox_from_lua(&player_hitbox, arena_alloc_str(str_arena, "%s_hitbox", "rumia"), L)) return 1;
    Hitbox player_hitbox_bounding = {0};
    if (!load_hitbox_from_lua(&player_hitbox_bounding, arena_alloc_str(str_arena, "%s_hitbox", "rumia_bbox"), L)) return 1;
    player_hitbox.color = YELLOW;
    player_hitbox_bounding.color = GREEN;
    Hitbox shot_hitbox = {0};
    if (!load_hitbox_from_file(&shot_hitbox, arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "HITBOX_PATH"), "rumia_shot.hitbox"))) return 1;
    shot_hitbox.color = BLUE;
	Entity player = make_player(&shots, v2(WIDTH*0.5, HEIGHT*0.5), 0.05f, 400.f, 200.f, "resources/gfx/rumia_player.png", 3, 1, player_hitbox, player_hitbox_bounding, shot_hitbox, lua_getstring(L, "RUMIA_SHOT_TEXPATH"));

	Sprite player_hitbox_spr = {0};
	Texture2D player_hitbox_tex = {0};
	if (!load_texture(&tm, arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "TEXTURE_PATH"), "rumia_hitbox.png"), &player_hitbox_tex)) {
		log_error("Failed to load Player Hitbox Texture!");
		return 1;
	}
	if (!init_sprite(&player_hitbox_spr, player_hitbox_tex, 1, 1)) {
		log_error("Failed to init Player Hitbox Sprite!");
		return 1;
	}
	center_sprite_origin(&player_hitbox_spr);

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
    int GLOBAL_UI_FS=GLOBAL_FS;

    State current_state = STATE_NORMAL;
    State prev_state    = STATE_NORMAL;

    CHANGE_STATE(STATE_NORMAL);

    int P=10;

    /// Lua Vars
    int H=GLOBAL_FS+2;
    Textbox lua_script_tbox = make_textbox(font, GLOBAL_FS, YELLOW, GRAY, v2(P, HEIGHT-GLOBAL_FS-P-(0*H)), v2(200, H), 1024, "Lua Script", ' ');
    Textbox lua_func_tbox   = make_textbox(font, GLOBAL_FS, YELLOW, GRAY, v2(P, HEIGHT-GLOBAL_FS-P-(1*H)), v2(200, H), 1024, "Lua Func", 'f');

    /// Edit Vars
    Edit_state edstate = EDSTATE_HITBOX;
    
    /// Edit Hitbox Vars
    Hitbox editing_hitbox = { .pos = {0, 0}, .size = {16,16}};
    Vector2 editing_hitbox_screen_pos = {WIDTH*0.5-(editing_hitbox.size.x*0.5), HEIGHT*0.5-(editing_hitbox.size.y*0.5)};
    Vector2 moving_offset = {0};

    Textbox editing_textboxes[EDITING_TBOX_COUNT] = {
        // [EDITING_TBOX_NAME]       = make_textbox(font, GLOBAL_FS, YELLOW, GRAY, v2(P, HEIGHT-20-P), v2(200, GLOBAL_FS), 1024, "Hitbox Name", '1'),
        [EDITING_TBOX_FILEPATH]   = make_textbox(font, GLOBAL_FS, YELLOW, GRAY, v2(P, HEIGHT-20-P-2*GLOBAL_FS), v2(200, GLOBAL_FS), 1024, "Hitbox Name", '2'),
        [EDITING_TBOX_TEXPATH]    = make_textbox(font, GLOBAL_FS, YELLOW, GRAY, v2(P, HEIGHT-20-P-4*GLOBAL_FS), v2(200, GLOBAL_FS), 1024, "Hitbox Texpath", '3'),
        [EDITING_TBOX_TEX_OFFSET] = make_textbox(font, GLOBAL_FS, YELLOW, GRAY, v2(P, HEIGHT-20-P-6*GLOBAL_FS), v2(200, GLOBAL_FS), 1024, "Hitbox TexOffset", '4'),
        [EDITING_TBOX_TEX_SIZE]   = make_textbox(font, GLOBAL_FS, YELLOW, GRAY, v2(P, HEIGHT-20-P-8*GLOBAL_FS), v2(200, GLOBAL_FS), 1024, "Hitbox TexSize", '5'),
    };

    Texture2D editing_hitbox_texture = {0};
    Vector2i  editing_hitbox_texture_offset = {0};
    Vector2i  editing_hitbox_texture_size = {0};
    float editing_hitbox_scale = 1;

    // Edit SFX Vars
    Textbox edit_sfx_filepath_tbox = make_textbox(font, GLOBAL_FS, YELLOW, GRAY, v2(0, 0), v2(200, GLOBAL_FS), 1024, "SFX Path", 0);
    Music edit_sfx_music = {0};

    // Mouse
    Vector2 m = {0};

	/// @DEBUG
	float angle = 0;
	Bullet_emitter em = make_bullet_emitter(L, v2(WIDTH*0.5, HEIGHT*0.5), &bullets, 0.05, "Pattern", (void*)&angle);

    UI_Theme ui_theme = get_default_ui_theme();
    ui_theme.titlebar_font_size = GLOBAL_UI_FS;
    ui_theme.titlebar_height = GLOBAL_UI_FS;
    UI ui = UI_make(ui_theme, &font, v2xx(P), "DEBUG", &m);

	while (!WindowShouldClose()) {
        ignore_mouse_input(false);
        delta = GetFrameTime();
        modified_delta = delta * delta_modification;
		arena_reset(&temp_arena);
		arena_reset(&str_arena);

        BeginDrawing();
		ClearBackground(BLACK);
        m = get_mpos_scaled(SCREEN_SCALE);

        // NOTE: UI
        UI_begin(&ui, UI_LAYOUT_KIND_VERT);
        if (DEBUG_DRAW) {
            int font_size = 24;
            UI_text(&ui, arena_alloc_str(str_arena, "State: %s", state_as_str(current_state)), font_size, WHITE);
            UI_text(&ui, arena_alloc_str(str_arena, "Delta [mod * dt: mod_dt]: %f * %f: %f", delta_modification, delta, modified_delta), font_size, WHITE);
            UI_line(&ui, 2, WHITE);

            DrawFPS(0, 0);

            switch (current_state) {
                case STATE_NORMAL: {
                    if (current_level < 0) {
                        UI_text(&ui, "No Level Selected", font_size, YELLOW);
                    } else {
                        UI_text(&ui, arena_alloc_str(str_arena, "Level: %s, time: %.2f, %s", levels.items[current_level].name, levels.items[current_level].time, levels.items[current_level].paused ? "PAUSED" : "RUNNING"), font_size, YELLOW);
                    }

                    UI_line(&ui, 2, WHITE);
                    UI_text(&ui, arena_alloc_str(str_arena, "Bullets count: %zu", bullets.count), font_size, RED);
                    UI_text(&ui, arena_alloc_str(str_arena, "Shots count: %zu", shots.count), font_size, RED);
                    UI_text(&ui, arena_alloc_str(str_arena, "Enemies count: %zu", enemies.count), font_size, RED);
                } break;
                case STATE_EDIT: {
                    UI_text(&ui, arena_alloc_str(str_arena, "Edit State: %s", edstate_as_str(edstate)), font_size, YELLOW);

                    switch (edstate) {
                        case EDSTATE_SPAWNERS: {
                            UI_spacing(&ui, 10);
                            UI_text(&ui, "SPAWNERS", GLOBAL_UI_FS, RED);
                        } break;
                        case EDSTATE_HITBOX: {
                            UI_spacing(&ui, 10);
                            UI_text(&ui, "HITBOX", GLOBAL_UI_FS, RED);
                            for (int i = 0; i < ARRAY_LEN(editing_textboxes); ++i) {
                                Textbox *tbox = &editing_textboxes[i];
                                if (UI_textbox(&ui, tbox)) {
                                    switch (i) {
                                        // case EDITING_TBOX_NAME: {
                                        //     confirm_name_tbox(&editing_hitbox, &editing_hitbox_texture, editing_textboxes, tbox, &editing_hitbox_texture_size);
                                        // } break;
                                        case EDITING_TBOX_FILEPATH: {
                                            confirm_filepath_tbox(&editing_hitbox, &editing_hitbox_texture, editing_textboxes, tbox);
                                        } break;
                                        case EDITING_TBOX_TEXPATH: {
                                            confirm_texpath_tbox(&editing_hitbox_texture, &editing_hitbox_texture_size, tbox);
                                        } break;
                                        case EDITING_TBOX_TEX_OFFSET: {
                                            confirm_texoffset_tbox(&editing_hitbox_texture_offset, tbox);
                                        } break;
                                        case EDITING_TBOX_TEX_SIZE: {
                                            confirm_texsize_tbox(&editing_hitbox_texture_size, tbox);
                                        } break;
                                        default: ASSERT(false, "UNREACHABLE!");
                                    }

                                    tbox->active = false;
                                }
                            }
                        } break;
                        case EDSTATE_SFX: {
                            UI_spacing(&ui, 10);
                            UI_text(&ui, "SFX", GLOBAL_UI_FS, RED);

                            if (UI_textbox(&ui, &edit_sfx_filepath_tbox)) {
                                edit_sfx_filepath_tbox.active = false;
                                const char *fullpath = arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "SFX_PATH"), edit_sfx_filepath_tbox.buff);
                                log_debug("SFX PATH: %s", fullpath);
                                if (IsMusicReady(edit_sfx_music)) {
                                    UnloadMusicStream(edit_sfx_music);
                                }
                                edit_sfx_music = LoadMusicStream(fullpath);
                                if (IsMusicReady(edit_sfx_music)) {
                                    log_debug("Successfully loaded Music %s", fullpath);
                                } else {
                                    log_error("Failed to load Music %s", fullpath);
                                }
                            }

                            if (UI_button(&ui, IsMusicStreamPlaying(edit_sfx_music) ? "PAUSE" : "PLAY", GLOBAL_UI_FS, WHITE)) {

                                if (IsMusicStreamPlaying(edit_sfx_music)) {
                                    log_debug("PAUSE");
                                    PauseMusicStream(edit_sfx_music);
                                } else {
                                    log_debug("PLAY");
                                    PlayMusicStream(edit_sfx_music);
                                }
                            }

                            if (IsMusicReady(edit_sfx_music)) {
                                float played_secs = GetMusicTimePlayed(edit_sfx_music);
                                float played_mins = played_secs / 60.f;
                                float length_secs = GetMusicTimeLength(edit_sfx_music);
                                float length_mins = length_secs / 60.f;
                                UI_text(&ui, arena_alloc_str(str_arena, "%.0f:%.2f/%.0f:%.2f", played_mins, played_secs, length_mins, length_secs), GLOBAL_UI_FS, WHITE);
                            }
                        } break;
                        case EDSTATE_COUNT:
                        default: ASSERT(false, "UNREACHABLE!");
                    }
                } break;
                case STATE_LUA: {
                    UI_text(&ui, "LUA", font_size, YELLOW);
                    UI_line(&ui, 2, WHITE);

                    if (UI_textbox(&ui, &lua_func_tbox)) {
                        lua_func_tbox.active = false;

                        log_debug("LUA FUNC: %s", lua_func_tbox.buff);
                    } 

                    if (UI_textbox(&ui, &lua_script_tbox)) {
                        lua_script_tbox.active = false;

                        const char *actual_lua_scriptpath = arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "SCRIPT_PATH"), lua_script_tbox.buff);
                        if (!lua_check(L, luaL_dofile(L, actual_lua_scriptpath))) {
                            log_error("Failed to do file %s", actual_lua_scriptpath);
                        } else {
                            log_debug("Done file %s", actual_lua_scriptpath);
                        }
                    }

                } break;
                case STATE_COUNT:
                default: ASSERT(false, "UNREACHABLE!");
            }
        }
        UI_end(&ui);


		// NOTE: Input
        if (IsKeyPressed(KEY_F1)) CHANGE_STATE(STATE_NORMAL);
		if (IsKeyPressed(KEY_GRAVE)) DEBUG_DRAW = !DEBUG_DRAW;
        if (IsKeyPressed(KEY_TAB)) {
            int next = current_state + 1;
            if (next >= STATE_COUNT) next = 0;
            CHANGE_STATE(next);
        }

        if (IsKeyDown(KEY_F7)) {
            delta_modification -= 0.01f;
        }
        if (IsKeyDown(KEY_F8)) {
            delta_modification += 0.01f;
        }
        if (IsKeyDown(KEY_F9)) {
            delta_modification = 1.f;
        }


        // State-specific Input
        switch (current_state) {
            case STATE_NORMAL: {
                /// @DEBUG
                if (mouse_button_down(MOUSE_BUTTON_LEFT)) {
                    em.pos = m;
                    update_bullet_emitter(&em);
                }
;
                /// @DEBUG
                if (IsKeyPressed(KEY_E)) {
                    Hitbox hbox = {0};
                    load_hitbox_from_file(&hbox, arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "HITBOX_PATH"), "spawnite.hitbox"));
                    hbox.color = RED;
                    Entity e = make_entity(m, arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "TEXTURE_PATH"), "spawnite.png"), 1, 1, 200.f, hbox);

                    darr_append(enemies, e);
                }
                
                // Reload config
                if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
                    load_config(L);
                }

                // Reload lua script
                if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_R)) {
                    if (!lua_check(L, luaL_dofile(L, arena_alloc_str(str_arena, "%s%s", lua_getstring(L, "SCRIPT_PATH"), "reload.lua")))) {
                        log_error("%s", "Failed to reload!");
                    } else {
                        log_debug("%s", "Reloaded succesfully!");
                    }
                }

            } break;
            case STATE_EDIT: {

                // Edit State switching
                if (IsKeyPressed(KEY_F9)) {
                    int next = edstate + 1;
                    if (next >= EDSTATE_COUNT) next = 0;
                    edstate = next;
                }
                switch (edstate) {
                    case EDSTATE_HITBOX: {
                        int active_count = 0;
                        for (int i = 0; i < EDITING_TBOX_COUNT; ++i) {
                            active_count += editing_textboxes[i].active;
                        }
                        if (active_count > 0) break;
                       
                        // Set Scale
                        if (IsKeyPressed(KEY_ZERO)) {
                            editing_hitbox_scale = 1;
                        }
                        // Size
                        if (!IsKeyDown(KEY_LEFT_CONTROL)) {
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
                        }

                        if (IsKeyDown(KEY_LEFT_CONTROL)) {
                            // Save
                            if (IsKeyPressed(KEY_S)) {
                                if (save_hitbox_to_lua_script(&editing_hitbox, editing_textboxes[EDITING_TBOX_FILEPATH].buff, lua_getstring(L, "HITBOXES_SCRIPT_PATH"))) {
                                    refresh_hitboxes_script(L);
                                    log_debug("Successfully saved hitbox %s to %s", editing_textboxes[EDITING_TBOX_FILEPATH].buff, lua_getstring(L, "HITBOXES_SCRIPT_PATH"));
                                } else {
                                    log_debug("Failed to save hitbox %s to %s", editing_textboxes[EDITING_TBOX_FILEPATH].buff, lua_getstring(L, "HITBOXES_SCRIPT_PATH"));
                                }
                            }
                            // Load
                            if (IsKeyPressed(KEY_L)) {
                                if (load_hitbox_from_lua(&editing_hitbox, arena_alloc_str(str_arena, "%s_hitbox", editing_textboxes[EDITING_TBOX_FILEPATH].buff), L)) {
                                    log_debug("Successfully loaded hitbox %s", editing_textboxes[EDITING_TBOX_FILEPATH].buff);
                                } else {
                                    log_error("Failed to load hitbox %s", editing_textboxes[EDITING_TBOX_FILEPATH].buff);
                                }
                            }
                        }

                        // Center on texture
                        if (IsKeyPressed(KEY_C) && IsTextureReady(editing_hitbox_texture)) {
                            editing_hitbox.pos.x = -editing_hitbox.size.x*0.5;
                            editing_hitbox.pos.y = -editing_hitbox.size.y*0.5;;
                        }

                        // Scaling
                        if (editing_hitbox_scale <= 1) editing_hitbox_scale = 1;
                        editing_hitbox_scale += GetMouseWheelMoveV().y * delta * 100.f * (IsKeyDown(KEY_LEFT_SHIFT) ? 0.5f : 1.f);

                    } break;
                    case EDSTATE_SPAWNERS: {
                    } break;
                    case EDSTATE_SFX: {
                    } break;
                    case EDSTATE_COUNT:
                    default: ASSERT(false, "UNREACHABLE!");
                }
            } break;
            case STATE_LUA: {
            } break;
            case STATE_COUNT:
            default: ASSERT(false, "UNREACHABLE!");
        }

		/// NOTE: Update

		// State-specific Update
        switch (current_state) {
            case STATE_NORMAL: {
                control_entity(&player, player_controls);
                bind(&player.pos, player.bounding_hitbox, bounds);
                update_entity(&player);
				player_hitbox_spr.pos = player.pos;
                // Update Bullets
                for (int i = bullets.count-1; i >= 0; --i) {
                    Bullet *b = &bullets.items[i];
                    update_bullet(b);

                    // Delete when outofbounds
                    if (!CheckCollisionPointRec(b->pos, bounds)) {
                        darr_delete(bullets, Bullet, i);
                    }
                }

                // Update Shots
                for (int i = 0; i < shots.count; ++i) {
                    Bullet *sh = &shots.items[i];
                    update_bullet(sh);

                    // Collision with enemies
                    for (int j = 0; j < enemies.count; ++j) {
                        Entity *enm = &enemies.items[j];

                        if (check_hitbox_on_hitbox_collision(sh->pos, sh->hitbox, enm->pos, enm->hitbox)) {
                            sh->dying = true;
                        }
                    }

                    // Delete when dead
                    if (sh->dead) {
                        darr_delete(shots, Bullet, i);
                    }

                    // Delete when outofbounds
                    if (!CheckCollisionPointRec(sh->pos, bounds)) {
                        darr_delete(shots, Bullet, i);
                    }
                }

                // Update Enemies
                for (int i = enemies.count-1; i >= 0; --i) {
                    Entity *e = &enemies.items[i];
                    update_entity(e);

                    if (e->dead) {
                        darr_delete(enemies, Entity, i);
                    }
                }

            } break;
            case STATE_EDIT: {
                switch (edstate) {
                    case EDSTATE_HITBOX: {
                        if (mouse_button_pressed(MOUSE_BUTTON_LEFT))   { moving_offset = v2_sub(m, editing_hitbox_screen_pos); }
                        if (mouse_button_down(MOUSE_BUTTON_LEFT)) {
                            editing_hitbox_screen_pos = v2_sub(m, moving_offset);
                        }
                    } break;
                    case EDSTATE_SPAWNERS: {
                    } break;
                    case EDSTATE_SFX: {
                        if (IsMusicReady(edit_sfx_music)) {
                            UpdateMusicStream(edit_sfx_music);
                        }
                    } break;
                    case EDSTATE_COUNT:
                    default: ASSERT(false, "UNREACHABLE!");
                }
            } break;
            case STATE_LUA: {
            } break;
            case STATE_COUNT:
            default: ASSERT(false, "UNREACHABLE!");
        }

		/// NOTE: Draw
        /// @DEBUG
        DrawTextureEx(title_screen_tex, v2(0,0), 0, 1, WHITE);

		draw_entity(&player);
		draw_sprite(&player_hitbox_spr);

        // Draw Enemies
        for (int i = enemies.count-1; i >= 0; --i) {
            Entity *e = &enemies.items[i];
            draw_entity(e);
        }

        // Draw Bullets
		for (int i = 0; i < bullets.count; ++i) {
			Bullet *b = &bullets.items[i];
			draw_bullet(b);
		}

        // Draw Shots
		for (int i = 0; i < shots.count; ++i) {
			Bullet *sh = &shots.items[i];
			draw_bullet(sh);
		}


        // Mode-specific Draw
        switch (current_state) {
            case STATE_NORMAL: {
                DrawRectangleLinesEx(bounds, 1.f, WHITE);

            } break;
            case STATE_EDIT: {
                switch (edstate) {
                    case EDSTATE_HITBOX: {
                        DrawRectangleV(v2xx(0), v2(WIDTH, HEIGHT), ColorAlpha(BLACK, 1));

                        if (IsTextureReady(editing_hitbox_texture)) {
                            // draw_texture_centered(editing_hitbox_texture, editing_hitbox_screen_pos, v2xx(editing_hitbox_scale), 0, WHITE);
                            Rectangle dst = {
                                .x = editing_hitbox_screen_pos.x - (editing_hitbox_texture_size.x*0.5) * editing_hitbox_scale,
                                .y = editing_hitbox_screen_pos.y - (editing_hitbox_texture_size.y*0.5) * editing_hitbox_scale,
                                .width	= editing_hitbox_texture_size.x * editing_hitbox_scale,
                                .height = editing_hitbox_texture_size.y * editing_hitbox_scale,
                            };
                            Rectangle src = {
                                .x = editing_hitbox_texture_offset.x,
                                .y = editing_hitbox_texture_offset.y,
                                .width	= editing_hitbox_texture_size.x,
                                .height = editing_hitbox_texture_size.y,
                            };
                            Vector2 origin = CLITERAL(Vector2) {
                                    .x = 0,
                                    .y = 0,
                            };
                            DrawTexturePro(editing_hitbox_texture, src, dst, origin, 0, WHITE);
                            DrawRectangleLinesEx(dst, 1, WHITE);
                            DrawCircleV(editing_hitbox_screen_pos, 4, GOLD);
                        }

                        draw_hitbox_offsetted_scaled(&editing_hitbox, editing_hitbox_screen_pos, v2xx(editing_hitbox_scale));

                        int fs = GLOBAL_FS;
                        draw_text_aligned(font, 
                                          arena_alloc_str(str_arena, "HBOX: %dx%d [%dx%d]", 
                                                          (int)editing_hitbox.pos.x, 
                                                          (int)editing_hitbox.pos.y, 
                                                          (int)editing_hitbox.size.x, 
                                                          (int)editing_hitbox.size.y), 
                                          v2_add(editing_hitbox_screen_pos, v2(0, -fs)), fs, TEXT_ALIGN_V_BOTTOM, TEXT_ALIGN_H_LEFT, WHITE);
                        draw_text_aligned(font, 
                                          arena_alloc_str(str_arena, "TEX: [%dx%d]", 
                                                          editing_hitbox_texture.width, 
                                                          editing_hitbox_texture.height), 
                                          v2_add(editing_hitbox_screen_pos, v2(0, -2*fs)), fs, TEXT_ALIGN_V_BOTTOM, TEXT_ALIGN_H_LEFT, YELLOW);

                        // for (int i = 0; i < ARRAY_LEN(editing_textboxes); ++i) {
                        //     Textbox *tbox = &editing_textboxes[i];
                        //     draw_textbox(tbox);
                        // }
                    } break;
                    case EDSTATE_SPAWNERS: {
                    } break;
                    case EDSTATE_SFX: {
                    } break;
                    case EDSTATE_COUNT:
                    default: ASSERT(false, "UNREACHABLE!");
                }
            } break;
            case STATE_LUA: {
            } break;
            case STATE_COUNT:
            default: ASSERT(false, "UNREACHABLE!");
        }

        UI_draw(&ui);

        EndTextureMode();
        draw_ren_tex(ren_tex, SCREEN_WIDTH, SCREEN_HEIGHT);
        EndDrawing();
	}

    // NOTE: CLEANUP
    UI_free(&ui);

    for (int i = 0; i < ARRAY_LEN(editing_textboxes); ++i) {
        Textbox *tbox = &editing_textboxes[i];
        free_textbox(tbox);
    }
    free_textbox(&edit_sfx_filepath_tbox);

    // TODO: Free darrs
    lua_close(L);
	close_window(ren_tex);
	return 0;
}

