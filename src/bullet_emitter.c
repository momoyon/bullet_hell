#include <bullet_emitter.h>

Bullet_emitter make_bullet_emitter(Vector2 pos, Bullet_array *bullets_ptr, float fire_rate, Bullet_pattern_fn pattern_fn, void *userdata) {
	Bullet_emitter em = {
		.pos = pos,
		.bullets_ptr = bullets_ptr,
		.pattern_fn = pattern_fn,
		.userdata = userdata,
	};
	ASSERT(bullets_ptr, "bullets_ptr is NULL");
	em.alarm = (Alarm) { .alarm_time = fire_rate };

	return em;
}

void update_bullet_emitter(Bullet_emitter *em) {
	if (on_alarm(&em->alarm, GetFrameTime())) {
		if (em->pattern_fn) {
			Bullet_array bllts = em->pattern_fn(em->pos, em->userdata);

			for (int i = 0; i < bllts.count; ++i) {
				Bullet b = bllts.items[i];
				darr_append((*em->bullets_ptr), b);
			}

			darr_free(bllts);
		}
	}
}
