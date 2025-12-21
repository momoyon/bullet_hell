#include <common.h>
#include <engine.h>
#include <config.h>

void bind(Vector2 *p, float radius, Rectangle bound) {
	if (p->x < bound.x + radius) p->x = bound.x + radius;
	if (p->x > bound.x + bound.width - radius) p->x = bound.x + bound.width - radius;
	if (p->y < bound.y + radius) p->y = bound.y + radius;
	if (p->y > bound.y + bound.height - radius) p->y = bound.y + bound.height - radius;
}

void draw_info_text(Vector2 *p, const char *text, int font_size, Color color) {
    draw_text(GetFontDefault(), text, *p, font_size, color);
    p->y += font_size + 2;
}

// NOTE: You preload all textures here (used in loading)
void load_all_textures(void) {
    load_texture(&tm, "resources/gfx/title_screen.png", NULL);
}
