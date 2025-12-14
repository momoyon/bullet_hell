#include <common.h>

void bind(Vector2 *p, float radius, Rectangle bound) {
	if (p->x < bound.x + radius) p->x = bound.x + radius;
	if (p->x > bound.x + bound.width - radius) p->x = bound.x + bound.width - radius;
	if (p->y < bound.y + radius) p->y = bound.y + radius;
	if (p->y > bound.y + bound.height - radius) p->y = bound.y + bound.height - radius;
}
