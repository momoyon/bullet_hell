#include <enemy.h>
#include <config.h>

Enemy make_enemy(Vector2 pos, const char *tex_path, float hitbox_radius) {
	Enemy e = {
		.pos = pos,
		.hitbox_radius = hitbox_radius,
	};

	ASSERT(load_texture(&tm, tex_path, &e.tex), "Texture fail");

	return e;
}

void draw_enemy(Enemy *e) {
	Rectangle src = {
		.x = 0,
		.y = 0,
		.width =  e->tex.width,
		.height = e->tex.height,
	};
	Rectangle dst = {
		.x = e->pos.x,
		.y = e->pos.y,
		.width =  e->tex.width,
		.height = e->tex.height,
	};
	Vector2 origin = v2(e->tex.width*0.5, e->tex.height*0.5);
	DrawTexturePro(e->tex, src, dst, origin, 0, WHITE);
	if (DEBUG_DRAW) {
		DrawCircleV(e->pos, e->hitbox_radius, RED);
	}
}
