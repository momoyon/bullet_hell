#include <level_action.h>

const char *level_action_type_as_str(const Level_action_type t) {
	switch (t) {
		case LVL_ACT_SPAWN_ENEMY: return "Spawn Enemy";
		case LVL_ACT_COUNT:
		default: ASSERT(false, "UNREACHABLE!");
	}
}

Level_action spawn_enemy(Vector2 pos, float time) {
	Level_action res = {
		.activation_time = time,
		.type = LVL_ACT_SPAWN_ENEMY,
		.spawn_pos = pos,
	};

	return res;
}
