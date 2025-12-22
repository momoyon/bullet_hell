#include "raylib.h"
#include <engine.h>
#include <hitbox.h>
#include <stdio.h>
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>
#include <errno.h>

bool load_hitbox_from_file(Hitbox *hbox, const char *filepath) {
    int filesize = -1;
    char *file = (char *)read_file(filepath, &filesize);

    if (filesize == -1) {
        return false;
    }

    String_view sv = SV(file);

    String_view pos_x_sv = sv_lpop_until_char(&sv, ',');
    sv_lremove(&sv, 1); // Remove ,
    String_view pos_y_sv = sv_lpop_until_char(&sv, ' ');
    sv_lremove(&sv, 1); // Remove <SPACE>

    String_view size_x_sv = sv_lpop_until_char(&sv, ',');
    sv_lremove(&sv, 1); // Remove ,
    String_view size_y_sv = sv;


    int pos_x_count = -1;
    float pos_x = sv_to_float(pos_x_sv, &pos_x_count);
    if (pos_x_count == -1) {
        log_error("Failed to convert "SV_FMT" to a float", SV_ARG(pos_x_sv));
        free(file);
        return false;
    }
    int pos_y_count = -1;
    float pos_y = sv_to_float(pos_y_sv, &pos_y_count);
    if (pos_y_count == -1) {
        log_error("Failed to convert "SV_FMT" to a float", SV_ARG(pos_y_sv));
        free(file);
        return false;
    }

    int size_x_count = -1;
    float size_x = sv_to_float(size_x_sv, &size_x_count);
    if (size_x_count == -1) {
        log_error("Failed to convert "SV_FMT" to a float", SV_ARG(size_x_sv));
        free(file);
        return false;
    }
    int size_y_count = -1;
    float size_y = sv_to_float(size_y_sv, &size_y_count);
    if (size_y_count == -1) {
        log_error("Failed to convert "SV_FMT" to a float", SV_ARG(size_y_sv));
        free(file);
        return false;
    }

    hbox->pos.x  = pos_x;
    hbox->pos.y  = pos_y;
    hbox->size.x = size_x;
    hbox->size.y = size_y;

    free(file);
    return true;
}

bool save_hitbox_to_file(Hitbox *hbox, const char *filepath) {
    FILE *f = fopen(filepath, "w");

    if (!f) {
        log_error("Failed to open %s: %s", filepath, strerror(errno));
        return false;
    }

#define BUFF_SIZE 1024
    char buff[BUFF_SIZE] = {0};

    int written = snprintf(buff, BUFF_SIZE, "%f,%f %f,%f", hbox->pos.x, hbox->pos.y, hbox->size.x, hbox->size.y);

    // NOTE: We just assume this works
    fwrite((void *)buff, written, 1, f);

    return true;
}

void draw_hitbox(Hitbox *hbox) {
    DrawRectangleV(hbox->pos, hbox->size, ColorAlpha(RED, 0.75));
}

void draw_hitbox_offsetted(Hitbox *hbox, Vector2 offset) {
    DrawRectangleV(v2_add(hbox->pos, offset), hbox->size, ColorAlpha(RED, 0.75));
}
