#ifndef HITBOX_H_
#define HITBOX_H_

#include <raylib.h>

typedef struct Hitbox Hitbox;

struct Hitbox {
    Vector2 pos;
    Vector2 size;
};


bool load_hitbox_from_file(Hitbox *hbox, const char *filepath);
bool save_hitbox_to_file(Hitbox *hbox, const char *filepath);
void draw_hitbox(Hitbox *hbox);
void draw_hitbox_offsetted(Hitbox *hbox, Vector2 offset);

#endif // HITBOX_H_
