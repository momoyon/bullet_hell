#ifndef ENEMY_DEFINITION_H_
#define ENEMY_DEFINITION_H_

typedef struct Enemy_definition Enemy_definition;

struct Enemy_definition {
	const char *tex_path;
	Hitbox hitbox;
	const char *lua_behaviour_funcname; // This lua function will get called each frame
};

#endif // ENEMY_DEFINITION_H_
