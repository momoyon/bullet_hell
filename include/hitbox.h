#ifndef HITBOX_H_
#define HITBOX_H_

#include <raylib.h>

typedef struct Hitbox Hitbox;

struct Hitbox {
    Vector2 pos;
    Vector2 size;
    Color color;
};

bool load_hitbox_from_file(Hitbox *hbox, const char *filepath);
bool load_hitbox_from_file_scaled(Hitbox *hbox, const char *filepath, float scl);
bool save_hitbox_to_file(const Hitbox *hbox, const char *filepath);
bool save_hitbox_to_file_scaled(const Hitbox *hbox, const char *filepath, float scl);
void draw_hitbox(Hitbox *hbox);
void draw_hitbox_offsetted(Hitbox *hbox, Vector2 offset);
void draw_hitbox_offsetted_scaled(Hitbox *hbox, Vector2 offset, Vector2 scl);

#endif // HITBOX_H_
