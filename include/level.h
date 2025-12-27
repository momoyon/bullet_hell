#ifndef LEVEL_H_
#define LEVEL_H_

#include <engine.h>
#include <spawner.h>

typedef struct Level Level;
typedef struct Levels Levels;

struct Level {
    const char *name;
    float time;
    bool paused;
    Spawners spawners;
};

struct Levels {
    Level *items;
    size_t count;
    size_t capacity;
};

#endif // LEVEL_H_
