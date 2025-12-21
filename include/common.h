#ifndef COMMON_H_
#define COMMON_H_

#include <raylib.h>

void bind(Vector2 *p, float radius, Rectangle bound);
void draw_info_text(Vector2 *p, const char *text, int font_size, Color color);
void load_all_textures(void);

#endif // COMMON_H_
