#ifndef COMMON_H_
#define COMMON_H_

#include <raylib.h>

void bind(Vector2 *p, Vector2 size, Rectangle bound);
void draw_info_text(Vector2 *p, const char *text, int font_size, Color color);
void draw_texture_centered(Texture2D tex, Vector2 pos, Vector2 scl, float rot, Color tint);
void load_all_textures(void);
bool is_key_pressed_repeat(int key);
bool is_key_down_ON_key_down_OR_key_pressed_repeat(int key, int on_key);

#endif // COMMON_H_
