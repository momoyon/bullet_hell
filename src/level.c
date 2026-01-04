#include <level.h>
#include <lualib.h>
#include <lauxlib.h>

void define_level_struct_in_lua(lua_State *L) {
	if (!lua_check(L, luaL_dostring(L,
                "Level = {}\n"
                "Level.__index = Level\n"
                "function Level.new(name)\n"
                "local self = setmetatable({}, Level)\n"
                "self.name = name\n"
                "self.spawners = {}\n"
                "self.actions = {}\n"
                "return self\n"
                "end\n"
                  ))) {
        log_error("Failed to define Level struct in LUA!");
        exit(1);
    }
    log_debug("Defined Level struct in LUA!");
}

bool save_level_to_lua(Level *l, const char *name, const char *scriptpath) {
    FILE *f = fopen(scriptpath, "a");
    if (!f) {
        log_error("save_level_to_lua(): Failed to open %s", scriptpath);
        return false;
    }

    fprintf(f, "%s_level = Level.new(%s)\n", name, l->name);

    fclose(f);
    return true;

}

bool load_level_from_lua(Level *l, lua_State *L, const char *level_name) {
    int type = lua_getglobal(L, level_name);

    if (type == LUA_TNIL) {
        log_error("Failed to find level %s", level_name);
        return false;
    }

	lua_getfield(L, -1, "name");
	const char *name = lua_tostring(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "spawners");
	luaL_checktype(L, -1, LUA_TTABLE);
	size_t count = lua_rawlen(L, -1);
	l->spawners.count = 0;

	for (int i = 1; i <= count; ++i) {
		lua_pushnumber(L, i);
		lua_gettable(L, -2);

		Spawner s = parse_spawner_from_lua(L);
		darr_append(l->spawners, s);

		lua_pop(L, 1); // Pop i
	}
	lua_pop(L, 1); // Pop spawners field
	
	// TODO: Parse actions
	return true;
}

Level make_level(const char *name) {
	Level res = {
		.name = name,
	};
	return res;
}
