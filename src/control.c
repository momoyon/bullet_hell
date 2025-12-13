#include <control_config.h>
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

#include <raylib.h>

void add_control(Control *ctrls, int key, Control_action action) {
	Control_map map = {
		.key = key,
		.action = action,
	};
	darr_append(ctrls->controls, map);
}

#define X(x) for (int i = 0; i < ctrls->controls.count; ++i) {\
		Control_map map = ctrls->controls.items[i];\
\
		if (map.action == action) {\
			if (IsKey##x(map.key)) return true;\
		}\
	}\
	return false
bool on_action_pressed(Control *ctrls, Control_action action) {
	X(Pressed);
}

bool on_action_released(Control *ctrls, Control_action action) {
	X(Released);
}
bool on_action_held(Control *ctrls, Control_action action) {
	X(Down);
}
