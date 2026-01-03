#ifndef COMMON_H_
#define COMMON_H_

#include <hitbox.h>
#include <raylib.h>
#include <lua.h>

void bind(Vector2 *p, Hitbox hbox, Rectangle bound);
void draw_info_text(Vector2 *p, const char *text, int font_size, Color color);
void draw_texture_centered(Texture2D tex, Vector2 pos, Vector2 scl, float rot, Color tint);
void load_all_textures(void);
bool is_key_pressed_repeat(int key);
bool is_key_down_ON_key_down_OR_key_pressed_repeat(int key, int on_key);

// Lua Helpers
bool lua_check(lua_State *L, int ret);
const char *lua_getstring(lua_State *L, const char *name);
float lua_getfloat(lua_State *L, const char *name);
int lua_getint(lua_State *L, const char *name);
void load_config(lua_State *L);
void refresh_hitboxes_script(lua_State *L);

#endif // COMMON_H_
