#include <common.h>
#include <engine.h>
#include <config.h>
#include <lauxlib.h>
#include <lua.h>

void bind(Vector2 *p, Hitbox hbox, Rectangle bound) {
    if (hbox.pos.x + p->x < bound.x) p->x = bound.x - hbox.pos.x;
    if (hbox.pos.x + p->x + hbox.size.x > bound.x + bound.width) p->x = bound.x + bound.width - hbox.pos.x - hbox.size.x;

    if (hbox.pos.y + p->y < bound.y) p->y = bound.y - hbox.pos.y;
    if (hbox.pos.y + p->y + hbox.size.y > bound.y + bound.height) p->y = bound.y + bound.height - hbox.pos.y - hbox.size.y;
}

void draw_info_text(Vector2 *p, const char *text, int font_size, Color color) {
    draw_text(GetFontDefault(), text, *p, font_size, color);
    p->y += font_size + 2;
}

// NOTE: You preload all textures here (used in loading)
void load_all_textures(void) {
    load_texture(&tm, "resources/gfx/title_screen.png", NULL);
    load_texture(&tm, "resources/gfx/rumia_player.png", NULL);
}

void draw_texture_centered(Texture2D tex, Vector2 pos, Vector2 scl, float rot, Color tint) {
    Rectangle src = {
        .x = 0, .y = 0,
        .width = tex.width, .height = tex.height,
    };
    Rectangle dst = {
        .x = pos.x, .y = pos.y,
        .width = tex.width * scl.x, .height = tex.height * scl.y,
    };
    Vector2 origin = v2(dst.width*0.5f, dst.height*0.5f);
    DrawTexturePro(tex, src, dst, origin, rot, tint);
}

bool is_key_pressed_repeat(int key) {
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

bool is_key_down_ON_key_down_OR_key_pressed_repeat(int key, int on_key) {
    return IsKeyDown(on_key) ? IsKeyDown(key) : is_key_pressed_repeat(key);
}

// Lua Helpers
bool lua_check(lua_State *L, int ret) {
    if (ret != LUA_OK) {
        log_error("LUA ERROR: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
    return true;
}

const char *lua_getstring(lua_State *L, const char *name) {
    int type = lua_getglobal(L, name);
    const char *res = NULL;
    if (type != LUA_TSTRING) {
        log_error("Expected string but got %s", lua_typename(L, type));
        return NULL;
    }
    res = lua_tostring(L, -1);
    lua_pop(L, 1);

    return res;
}

float lua_getfloat(lua_State *L, const char *name) {
    int type = lua_getglobal(L, name);
    float res = 0;
    if (type != LUA_TNUMBER) {
        log_error("Expected float but got %s", lua_typename(L, type));
        return 0;
    }
    res = lua_tonumber(L, -1);
    lua_pop(L, 1);

    return res;
}

void load_config(lua_State *L) {
    // config.lua path has to be hard-coded
    if (!lua_check(L, luaL_dofile(L, "resources/scripts/config.lua"))) {
        log_error("Failed to load config!");
    } else {
        // TEXTURE_PATH         = lua_getstring(L, "TEXTURE_PATH");
        // HITBOX_PATH          = lua_getstring(L, "HITBOX_PATH");
        // SCRIPT_PATH          = lua_getstring(L, "SCRIPT_PATH");
        // HITBOXES_SCRIPT_PATH = lua_getstring(L, "HITBOXES_SCRIPT_PATH");
        //
        // RUMIA_SHOT_TEXPATH   = lua_getstring(L, "RUMIA_SHOT_TEXPATH");
        // RUMIA_SHOT_SPEED     = lua_getfloat(L, "RUMIA_SHOT_SPEED");
        //
        log_debug("Loaded config!");
    }
}

void refresh_hitboxes_script(lua_State *L) {
    lua_check(L, luaL_dofile(L, lua_getstring(L, "HITBOXES_SCRIPT_PATH")));
}
