#include <common.h>
#include <engine.h>
#include <config.h>

void bind(Vector2 *p, Vector2 size, Rectangle bound) {
	if (p->x < bound.x) p->x = bound.x;
	if (p->x > bound.x + bound.width - size.x) p->x = bound.x + bound.width - size.x;
	if (p->y < bound.y) p->y = bound.y;
	if (p->y > bound.y + bound.height - size.y) p->y = bound.y + bound.height - size.y;
}

void draw_info_text(Vector2 *p, const char *text, int font_size, Color color) {
    draw_text(GetFontDefault(), text, *p, font_size, color);
    p->y += font_size + 2;
}

// NOTE: You preload all textures here (used in loading)
void load_all_textures(void) {
    load_texture(&tm, "resources/gfx/title_screen.png", NULL);
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
