#ifndef LEVEL_ACTION_H_
#define LEVEL_ACTION_H_

typedef struct Level_action Level_action;
typedef struct Level_actions Level_actions;
typedef enum Level_action_type Level_action_type;

enum Level_action_type {
	LVL_ACT_SPAWN_ENEMY,
	LVL_ACT_COUNT,
};

const char *level_action_type_as_str(const Level_action_type t);

struct Level_action {
	float activation_time;
	Level_action_type type;
	Vector2 spawn_pos; // For Spawn types
};

Level_action spawn_enemy(Vector2 pos);

struct Level_actions {
	Level_action *items;
	size_t count;
	size_t capacity;
};

#endif // LEVEL_ACTION_H_
