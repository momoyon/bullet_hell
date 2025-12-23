#include <common.h>
#include <engine.h>
#include <config.h>
#include <lauxlib.h>

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

void refresh_hitboxes_script(lua_State *L) {
    lua_check(L, luaL_dofile(L, HITBOXES_SCRIPT_PATH));
}
