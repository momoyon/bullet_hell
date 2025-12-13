#ifndef _CONTROL_CONFIG_H_
#define _CONTROL_CONFIG_H_

#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

typedef enum   Control_action Control_action;
typedef struct Control_map Control_map;
typedef struct Control_maps Control_maps;
typedef struct Control Control;

enum Control_action {
	ACTION_MOVE_UP = 0,
	ACTION_MOVE_DOWN,
	ACTION_MOVE_LEFT,
	ACTION_MOVE_RIGHT,
	ACTION_FOCUS,
	ACTION_FIRE,

	ACTION_UI_CONFIRM,
	ACTION_UI_MOVE_UP,
	ACTION_UI_MOVE_DOWN,
	ACTION_UI_MOVE_LEFT,
	ACTION_UI_MOVE_RIGHT,
};

struct Control_map {
	int key;
	Control_action action;
};

struct Control_maps {
	Control_map *items;
	size_t count;
	size_t capacity;
};

struct Control {
	Control_maps controls;
};

void add_control(Control *ctrls, int key, Control_action action);

bool on_action_pressed(Control *ctrls, Control_action action);
bool on_action_released(Control *ctrls, Control_action action);
bool on_action_held(Control *ctrls, Control_action action);

#endif // _CONTROL_CONFIG_H_
