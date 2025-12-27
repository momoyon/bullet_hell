#include "raylib.h"
#include <engine.h>
#include <hitbox.h>
#include <stdio.h>
#include <common.h>
#include <config.h>
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>
#include <errno.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void define_hitbox_struct_in_lua(lua_State *L) {
    if (!lua_check(L, luaL_dostring(L,
                "Hitbox = {}\n"
                "Hitbox.__index = Hitbox\n"
                "function Hitbox.new(x, y, w, h)\n"
                "local self = setmetatable({}, Hitbox)\n"
                "self.x = x\n"
                "self.y = y\n"
                "self.w = w\n"
                "self.h = h\n"
                "return self\n"
                "end\n"
                  ))) {
        log_error("Failed to define Hitbox struct in LUA!");
        exit(1);
    }
    log_debug("Defined Hitbox struct in LUA!");
}

bool save_hitbox_to_lua_script(const Hitbox *hbox, const char *name, const char *scriptpath) {
    FILE *f = fopen(scriptpath, "a");
    if (!f) {
        log_error("save_hitbox_to_lua_script(): Failed to open %s", scriptpath);
        return false;
    }

    fprintf(f, "%s_hitbox = Hitbox.new(%f, %f, %f, %f)\n", name, hbox->pos.x, hbox->pos.y, hbox->size.x, hbox->size.y);

    fclose(f);
    return true;
}

bool load_hitbox_from_lua(Hitbox *hbox, const char *name, lua_State *L) {
    int type = lua_getglobal(L, name);

    if (type == LUA_TNIL) {
        log_error("Failed to find hitbox %s", name);
        return false;
    }

    *hbox = hitbox_from_lua(L);

    return true;
}

bool load_hitbox_from_file(Hitbox *hbox, const char *filepath) {
    return load_hitbox_from_file_scaled(hbox, filepath, 1.0f);
}

bool load_hitbox_from_file_scaled(Hitbox *hbox, const char *filepath, float scl) {
    int filesize = -1;
    char *file = (char *)read_file(filepath, &filesize);

    if (filesize == -1) {
        return false;
    }

    String_view sv = SV(file);

    String_view pos_x_sv = sv_lpop_until_char(&sv, ',');
    sv_lremove(&sv, 1); // Remove ,
    String_view pos_y_sv = sv_lpop_until_char(&sv, ' ');
    sv_lremove(&sv, 1); // Remove <SPACE>

    String_view size_x_sv = sv_lpop_until_char(&sv, ',');
    sv_lremove(&sv, 1); // Remove ,
    String_view size_y_sv = sv;


    int pos_x_count = -1;
    float pos_x = sv_to_float(pos_x_sv, &pos_x_count);
    if (pos_x_count == -1) {
        log_error("Failed to convert "SV_FMT" to a float", SV_ARG(pos_x_sv));
        free(file);
        return false;
    }
    int pos_y_count = -1;
    float pos_y = sv_to_float(pos_y_sv, &pos_y_count);
    if (pos_y_count == -1) {
        log_error("Failed to convert "SV_FMT" to a float", SV_ARG(pos_y_sv));
        free(file);
        return false;
    }

    int size_x_count = -1;
    float size_x = sv_to_float(size_x_sv, &size_x_count);
    if (size_x_count == -1) {
        log_error("Failed to convert "SV_FMT" to a float", SV_ARG(size_x_sv));
        free(file);
        return false;
    }
    int size_y_count = -1;
    float size_y = sv_to_float(size_y_sv, &size_y_count);
    if (size_y_count == -1) {
        log_error("Failed to convert "SV_FMT" to a float", SV_ARG(size_y_sv));
        free(file);
        return false;
    }

    hbox->pos.x  = pos_x;
    hbox->pos.y  = pos_y;
    hbox->size.x = size_x * scl;
    hbox->size.y = size_y * scl;

    free(file);
    return true;
}

bool save_hitbox_to_file(const Hitbox *hbox, const char *filepath) {
    return save_hitbox_to_file_scaled(hbox, filepath, 1.f);
}

bool save_hitbox_to_file_scaled(const Hitbox *hbox, const char *filepath, float scl) {
    FILE *f = fopen(filepath, "w");

    if (!f) {
        log_error("Failed to open %s: %s", filepath, strerror(errno));
        return false;
    }

#define BUFF_SIZE 1024
    char buff[BUFF_SIZE] = {0};

    int written = snprintf(buff, BUFF_SIZE, "%f,%f %f,%f", hbox->pos.x, hbox->pos.y, hbox->size.x * scl, hbox->size.y * scl);

    // NOTE: We just assume this works
    fwrite((void *)buff, written, 1, f);

    return true;
}

void draw_hitbox(Hitbox *hbox) {
    DrawRectangleV(hbox->pos, hbox->size, ColorAlpha(hbox->color, 0.75));
}

void draw_hitbox_offsetted(Hitbox *hbox, Vector2 offset) {
    Vector2 p = v2_add(hbox->pos, offset);
    DrawRectangleV(p, hbox->size, ColorAlpha(hbox->color, 0.75));
    DrawRectangleLines(p.x, p.y, hbox->size.x, hbox->size.y, WHITE);
}

void draw_hitbox_offsetted_scaled(Hitbox *hbox, Vector2 offset, Vector2 scl) {
    Vector2 p = v2_add(v2_mul(hbox->pos, scl), offset);
    Vector2 s = v2(hbox->size.x * scl.x, hbox->size.y * scl.y);
    DrawRectangleV(p, s, ColorAlpha(hbox->color, 0.75));
    DrawRectangleLines(p.x, p.y, s.x, s.y, WHITE);
}

bool check_hitbox_on_hitbox_collision(Vector2 a_pos, Hitbox a, Vector2 b_pos, Hitbox b) {
    Rectangle rect_a = {
        .x = a_pos.x + a.pos.x,
        .y = a_pos.y + a.pos.y,
        .width = a.size.x,
        .height = a.size.y,
    };

    Rectangle rect_b = {
        .x = b_pos.x + b.pos.x,
        .y = b_pos.y + b.pos.y,
        .width = b.size.x,
        .height = b.size.y,
    };

    return CheckCollisionRecs(rect_a, rect_b);
}

Hitbox hitbox_from_lua(lua_State *L) {
    // log_debug("BEGIN PARSING HITBOX FROM LUA: %d", lua_gettop(L));

    lua_getfield(L, -1, "x");
    float x = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "y");
    float y = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "w");
    float w = lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "h");
    float h = lua_tonumber(L, -1);
    lua_pop(L, 1);

    // log_debug("END PARSING HITBOX FROM LUA: %d", lua_gettop(L));
    return (Hitbox) { .pos = {x, y}, .size = {w, h} };
}
