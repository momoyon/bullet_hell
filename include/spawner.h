#ifndef SPAWNER_H_
#define SPAWNER_H_

#include <engine.h>

typedef struct Spawner Spawner;
typedef struct Spawners Spawners;

struct Spawner {
    Vector2 pos;
    Alarm alarm;
};

struct Spawners {
    Spawner *items;
    size_t count;
    size_t capacity;
};

Spawner make_spawner(Vector2 pos, float spawn_rate);
void update_spawner(Spawner *s);
void draw_spawner(Spawner *s);

#endif // SPAWNER_H_
