#ifndef HITBOX_H_
#define HITBOX_H_

#include <raylib.h>
#include <lua.h>

typedef struct Hitbox Hitbox;

struct Hitbox {
    Vector2 pos;
    Vector2 size;
    Color color;
};

void define_hitbox_struct_in_lua(lua_State *L);
bool save_hitbox_to_lua_script(const Hitbox *hbox, const char *name, const char *scriptpath);
bool load_hitbox_from_lua(Hitbox *hbox, const char *name, lua_State *L);
bool load_hitbox_from_file(Hitbox *hbox, const char *filepath);
bool load_hitbox_from_file_scaled(Hitbox *hbox, const char *filepath, float scl);
bool save_hitbox_to_file(const Hitbox *hbox, const char *filepath);
bool save_hitbox_to_file_scaled(const Hitbox *hbox, const char *filepath, float scl);
void draw_hitbox(Hitbox *hbox);
void draw_hitbox_offsetted(Hitbox *hbox, Vector2 offset);
void draw_hitbox_offsetted_scaled(Hitbox *hbox, Vector2 offset, Vector2 scl);

bool check_hitbox_on_hitbox_collision(Vector2 a_pos, Hitbox a, Vector2 b_pos, Hitbox b);
Hitbox hitbox_from_lua(lua_State *L);

#endif // HITBOX_H_
