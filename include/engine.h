#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

#include <stb_ds.h>

#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>

// #define ENGINE_IMPLEMENTATION

// NOTE: Mouse
static bool __eng_ignore_mouse_input = false;
Vector2 get_mpos_scaled(float scl);
bool mouse_button_pressed(int btn);
bool mouse_button_down(int btn);
bool mouse_button_released(int btn);
bool mouse_button_pressed_unignored(int btn);
bool mouse_button_down_unignored(int btn);
bool mouse_button_released_unignored(int btn);
void ignore_mouse_input(bool ignore);

// Vector helpers
Vector2 v2xx(float v);
Vector2 v2(float x, float y);
#define v2_normalize Vector2Normalize
#define v2_add Vector2Add
#define v2_sub Vector2Subtract
#define v2_mag2 Vector2LengthSqr
#define v2_scale Vector2Scale
#define v2_mul Vector2Multiply
float v2_radians(Vector2 v);
Vector2 v2_from_radians(float r);
Vector2 v2_from_degrees(float d);

// NOTE: Vector2i
typedef struct {
	int x, y;
} Vector2i;

Vector2i v2vi(Vector2 v);
Vector2i v2i(int x, int y);
Vector2i v2ixx(int x);
bool v2i_equal(Vector2i a, Vector2i b);

// NOTE: Sprite
typedef struct Sprite Sprite;

#define SPRITE_DEFAULT_TIME_PER_FRAME 0.1f // seconds

struct Sprite {
	Texture2D texture;
	Rectangle tex_rect;
    Vector2i tex_offset;
	Vector2 pos;
	float width, height;
	float rotation;
	Vector2 origin;
	Vector2 scale;
	bool vflip, hflip;
	size_t vframes, hframes;
	size_t vframe, hframe;
	float time_per_frame; // in seconds
	float accumulated_time;
	Color tint;
};

bool init_sprite(Sprite* spr, Texture tex, size_t hframes, size_t vframes);
bool init_sprite_from_sheet(Sprite *spr, Texture tex, Vector2i offset, Vector2i size, size_t hframes, size_t vframes);
void update_sprite_tex_rect(Sprite *spr);
void set_sprite_hframe(Sprite* spr, size_t hframe);
void set_sprite_vframe(Sprite* spr, size_t vframe);
void center_sprite_origin(Sprite* spr);
void draw_sprite(Sprite* spr);
void animate_sprite_hframes(Sprite* spr, float delta);
void free_sprite(Sprite* spr);
void set_sprite_scale_scalar(Sprite *spr, float scl);

// NOTE: Timer and Alarm
typedef struct Timer Timer;
typedef struct Alarm Alarm;

struct Timer {
		float time;
};

void update_timer(Timer *t, float dt);

struct Alarm {
		Timer timer;
		float alarm_time;
		bool once;
		bool done;
};

bool on_alarm(Alarm *a, float dt);

// NOTE: UI
typedef struct UI UI;
typedef struct UI_Theme UI_Theme;
typedef struct UI_Layout UI_Layout;
typedef struct UI_Draw_element UI_Draw_element;

typedef enum {
	UI_LAYOUT_KIND_HORZ,
	UI_LAYOUT_KIND_VERT,
	UI_LAYOUT_KIND_COUNT
} UI_Layout_kind;

struct UI_Layout {
	UI_Layout_kind kind;
	Vector2 pos;
	Vector2 size;
	Vector2 padding;
};

typedef enum {
	UI_DRAW_ELEMENT_TYPE_RECT,
	UI_DRAW_ELEMENT_TYPE_BOX,
	UI_DRAW_ELEMENT_TYPE_SPRITE,
	UI_DRAW_ELEMENT_TYPE_SPRITE_FRAME,
	UI_DRAW_ELEMENT_TYPE_TEXT,
	UI_DRAW_ELEMENT_TYPE_LINE,
	UI_DRAW_ELEMENT_TYPE_COUNT,
} UI_Draw_element_type;

const char *UI_Draw_element_type_as_str(const UI_Draw_element_type t);

struct UI_Draw_element {
	UI_Draw_element_type type;
	Vector2 pos;
	Vector2 size;
	Color fill_color;
	Color out_color;
	Sprite* spr;
	int hframe, vframe;
	Font* font;
	cstr text;
	int font_size;
    float thick;
};

typedef struct {
	Arena arena;
	UI_Draw_element* buff;
	size_t count;
} UI_Draw_element_stack;

UI_Draw_element_stack UI_Draw_element_stack_make(void);
void UI_Draw_element_stack_push(UI_Draw_element_stack* stack, UI_Draw_element val);
bool UI_Draw_element_stack_pop(UI_Draw_element_stack* stack, UI_Draw_element* popped);
void UI_Draw_element_stack_free(UI_Draw_element_stack* stack);

Vector2 UI_Layout_available_pos(UI_Layout* this);
void UI_Layout_push_widget(UI_Layout* this, Vector2 size);

struct UI_Theme {
	Color bg_color;
	Color titlebar_color;
	int titlebar_padding;
	float titlebar_height;
	int titlebar_font_size;
	Vector2 bg_padding;
    float titlebar_pad_bottom;
};

UI_Theme get_default_ui_theme(void);
void set_ui_theme_titlebar_font_size(UI_Theme* theme, int font_size);

struct UI {
	int active_id;
	int last_used_id;
#define LAYOUTS_CAP 256
	UI_Layout layouts[LAYOUTS_CAP];
	size_t layouts_count;
	Vector2 btn_padding;
	int text_input_width; // in characters, so depends on the font_size
	Font* font;
	Rectangle bg_rect;
    Rectangle ui_rect; // @NOTE Rect covering the ui components (without bg_padding, etc)
	Vector2 pos;
	Vector2 pos_offset;
	bool is_moving;
	Alarm text_input_cursor_blink_alarm;
	bool show_text_input_cursor;
	UI_Draw_element_stack draw_element_stack;
	cstr title;
	bool show;
    UI_Theme theme;
    Vector2* mpos_ptr;
    Vector2 scroll_offset;
};

UI UI_make(UI_Theme theme, Font* font, Vector2 pos, cstr title, Vector2* mpos_ptr);
void UI_push_layout(UI* this, UI_Layout layout);
UI_Layout UI_pop_layout(UI* this);
UI_Layout* UI_top_layout(UI* this);
void UI_begin_layout(UI* this, UI_Layout_kind kind);
void UI_end_layout(UI* this);
void UI_free(UI* this);

void UI_begin(UI* this, UI_Layout_kind kind);
bool UI_button(UI* this, cstr text, int font_size, Color color);
void UI_text(UI* this, cstr text, int font_size, Color color);
void UI_line(UI* this, float thick, Color color);
void UI_spacing(UI* this, float spacing);
void UI_sprite(UI* this, Sprite* spr);
bool UI_sprite_button(UI* this, Sprite* spr);
bool UI_sprite_button_frame(UI* this, Sprite* spr, int hframe, int vframe);
void UI_text_input(UI* this, char* text_buff, uint32 text_buff_size, uint32* cursor, int font_size, Color color);
void UI_background(UI* this);
// @NOTE: We are defering drawing because we need to call UI funcs before any input handling for the frame is happened,
// if we want input ignoring. We just push draw info to a stack when the UI funcs are called and draw all of them at once on UI_draw().
// @NOTE: @IMPORTANT: IT IS A MUST TO CALL THIS BEFORE UI_end()!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void UI_draw(UI* this);
// @NOTE: Must be in called input handling for the frame. ***
void UI_end(UI* this);

// NOTE: TextBox
typedef struct Textbox Textbox;

struct Textbox {
		char *buff;
		size_t buff_size;
		int cursor;
		const char *name;
		Font font;
		Vector2 pos;
		int font_size;
		Color active_color;
		Color inactive_color;
		bool active;
		Vector2 size;
		bool ignoring_input;

		int activate_key;
		int deactivate_key;
		char ignore_char;
};

Textbox make_textbox(Font font, int fs, Color active_color, Color inactive_color, Vector2 pos, Vector2 size, size_t buff_size, const char *name, char ignore_char);
void free_textbox(Textbox *tbox);
bool update_textbox(Textbox *tbox);
bool input_to_textbox(Textbox *tbox);
void set_textbox_keys(Textbox *tbox, int activate, int deactivate);
void draw_textbox(Textbox *tbox);

// NOTE: Rectangle
bool rect_contains_point(Rectangle r1, Vector2 p);
bool rect_contains_rect(Rectangle r1, Rectangle r2);
bool rect_intersects_rect(Rectangle r1, Rectangle r2);
// bool rect_resolve_rect_collision(Rectangle* rect1, const Rectangle rect2);
// void rect_get_3d_points(Rectangle rect, Vector3f* p0, Vector3f* p1, Vector3f* p2, Vector3f* p3);
// void rect_get_points(Rectangle rect, Vector2* p0, Vector2* p1, Vector2* p2, Vector2* p3);

// NOTE: Window
RenderTexture2D init_window(int screen_width, int screen_height, float scl, const char *title, int *width_out, int *height_out);
void close_window(RenderTexture2D ren_tex);

typedef enum {
	TEXT_ALIGN_H_LEFT = 0,
	TEXT_ALIGN_H_CENTER,
	TEXT_ALIGN_H_RIGHT,
	TEXT_ALIGN_H_COUNT,
} Text_align_h;

// NOTE: Start enum at 10 to not conflict with Text_align_h
typedef enum {
	TEXT_ALIGN_V_TOP = 10,
	TEXT_ALIGN_V_CENTER,
	TEXT_ALIGN_V_BOTTOM,
	TEXT_ALIGN_V_COUNT,
} Text_align_v;

// NOTE: Draw
void draw_ren_tex(RenderTexture2D ren_tex, int screen_width, int screen_height);
void draw_text_aligned(Font font, const char *text, Vector2 pos, int font_size, const Text_align_v align_v, const Text_align_h align_h, Color color);
void draw_text_aligned_ex(Font font, const char *text, Vector2 pos, int font_size, const Text_align_v align_v, const Text_align_h align_h, float rotation, Color color);
void draw_text(Font font, const char *text, Vector2 pos, int font_size, Color color);


// NOTE: Input
bool input_to_buff(char *buff, size_t buff_cap, int *cursor);
bool input_to_buff_ignored(char *buff, size_t buff_cap, int *cursor, char ignore, bool *ignoring);

// NOTE: Assets Manager
typedef struct {
	char *key;
	Texture2D value;
} Texture_KV;

typedef struct {
	Texture_KV *texture_map;
} Texture_manager;

bool load_texture(Texture_manager *tm, const char *filepath, Texture2D *tex_out);
bool load_texture_(Texture_manager *tm, const char *filepath, Texture2D *tex_out, bool verbose);

// NOTE: Console
#define CONSOLE_LINE_BUFF_CAP (1024*1)

typedef struct Console Console;
typedef struct Console_line Console_line;
typedef struct Console_lines Console_lines;

struct Console_line {
	char buff[CONSOLE_LINE_BUFF_CAP];
		size_t count;
		Color color;
};

struct Console_lines {
	Console_line *items;
	size_t count;
	size_t capacity;
}; // @darr


typedef enum Console_flag Console_flag;

enum Console_flag {
		CONSOLE_FLAG_NONE,
		CONSOLE_FLAG_READLINE_USES_UNPREFIXED_LINES,
		CONSOLE_FLAG_COUNT,
};

struct Console {
	Console_lines history;
	Console_lines lines;
		Console_lines unprefixed_lines;
	int cursor; // offset in the line
	int line;	 // line number
		Font font;
		int hist_lookup_idx; // idx for Ctrl+P and Ctrl+N
		const char *prefix;
		const char *prefix2;
		char prefix_symbol;
		int flags; // int so can have 32 flags
		bool prompting;
		bool expecting_values;
		void (*prompt_done_func)(Console *, void *);
		void *prompt_userdata;
		String_array expected_prompt_values;
		size_t selected_prompt_value_id;
		size_t prompt_line_id;
};

Console make_console(int flags, Font font);
void add_line_to_console_simple(Console *console, char *line, Color color, bool hist);
void add_line_to_console(Console *console, char *buff, size_t buff_size, Color color, bool histt);
void add_line_to_console_prefixed(Console *console, Arena *tmp_arena, char *buff, Color color, bool histt);
void add_character_to_console_line(Console *console, char ch, size_t line);
Console_line *get_console_line(Console *console, size_t line);
Console_line *get_console_history(Console *console, size_t line);
Console_line *get_or_create_console_line(Console *console, size_t line);
void clear_console_line(Console_line *cl);
void clear_current_console_line(Console *console);
char *get_current_console_line_buff(Console *console);
String_array get_current_console_args(Console *console);
bool input_to_console(Console *console, char *ignore_characters, size_t ignore_characters_count);
float get_cursor_offset(Console *console, int font_size);
void draw_console(Console *console, Rectangle rect, Vector2 pad, int font_size, Color fill_color, Color border_color, float alpha);
void console_prompt(Console *console, const char *prompt, String_array *expected_prompt_values);

// NOTE: Macros
#define log_info_console(console, fmt, ...) do {\
				Console_line l = {\
						.color = WHITE,\
				};\
				snprintf(l.buff, CONSOLE_LINE_BUFF_CAP, "[INFO] "fmt, __VA_ARGS__);\
				darr_append(console.lines, l);\
		} while (0)

#define log_warning_console(console, fmt, ...) do {\
				Console_line l = {\
						.color = YELLOW,\
				};\
				snprintf(l.buff, CONSOLE_LINE_BUFF_CAP, "[WARNING] "fmt, __VA_ARGS__);\
				darr_append(console.lines, l);\
		} while (0)

#define log_error_console(console, fmt, ...) do {\
				Console_line l = {\
						.color = RED,\
				};\
				snprintf(l.buff, CONSOLE_LINE_BUFF_CAP, "[ERROR] "fmt, ##__VA_ARGS__);\
				darr_append(console.lines, l);\
		} while (0)

#ifdef DEBUG
#define log_debug_console(console, fmt, ...) do {\
				Console_line l = {\
						.color = YELLOW,\
				};\
				snprintf(l.buff, CONSOLE_LINE_BUFF_CAP, "[DEBUG] "fmt, __VA_ARGS__);\
				darr_append(console.lines, l);\
		} while (0)
#else
#define log_debug_console(...)
#endif

#endif // _ENGINE_H_

// IMPLEMENTATION ////////////////////////////////
#ifdef ENGINE_IMPLEMENTATION
// NOTE: Mouse
Vector2 get_mpos_scaled(float scl) {
	Vector2 m = GetMousePosition();
	m.x *= scl;
	m.y *= scl;
	return m;
}

bool mouse_button_pressed(int btn) {
    if (__eng_ignore_mouse_input) return false;
    return IsMouseButtonPressed(btn);
}

bool mouse_button_down(int btn) {
    if (__eng_ignore_mouse_input) return false;
    return IsMouseButtonDown(btn);
}

bool mouse_button_released(int btn) {
    if (__eng_ignore_mouse_input) return false;
    return IsMouseButtonReleased(btn);
}

bool mouse_button_pressed_unignored(int btn) {
    return IsMouseButtonPressed(btn);
}

bool mouse_button_down_unignored(int btn) {
    return IsMouseButtonDown(btn);
}

bool mouse_button_released_unignored(int btn) {
    return IsMouseButtonReleased(btn);
}

void ignore_mouse_input(bool ignore) {
    __eng_ignore_mouse_input = ignore;
}

// Vector helpers
Vector2 v2xx(float v) { return CLITERAL(Vector2) { v, v }; }
Vector2 v2(float x, float y) { return CLITERAL(Vector2) { x, y }; }
Vector2 v2_from_radians(float r) {
		Vector2 v = {0};

		v.x = cosf(r);
		v.y = sinf(r);

		return v;
}

Vector2 v2_from_degrees(float d) {
	return v2_from_radians(DEG2RAD*d);
}

float v2_radians(Vector2 v) {
		float angle = atan2f(v.y, v.x);
		return angle < 0 ? angle + (2*PI) : angle;	// Ensure the angle is positive
}

// Vector2i
Vector2i v2vi(Vector2 v) { return CLITERAL(Vector2i) { (int)v.x, (int)v.y }; }

Vector2i v2i(int x, int y) { return (Vector2i) { .x = x, .y = y }; }
Vector2i v2ixx(int x)      { return v2i(x, x); }

bool v2i_equal(Vector2i a, Vector2i b) {
	return a.x == b.x && a.y == b.y;
}

// @TODO: make an UI array struct and do begin/end and draws on them, make the active UI item the last in the array
// so that way the active UI item has the input.

extern bool should_ignore_mouse_input;

static Vector2 get_ui_bg_rect_pos(UI* this) {
    return (Vector2) {
        .x = this->pos.x - this->theme.bg_padding.x,
        .y = this->pos.y + (this->theme.titlebar_height * (1.f+this->theme.titlebar_pad_bottom)) - this->theme.bg_padding.y,
    };
}

Vector2 UI_Layout_available_pos(UI_Layout* this) {
	switch (this->kind) {
	case UI_LAYOUT_KIND_HORZ: {
		return (Vector2) {
			.x = this->pos.x + this->size.x + this->padding.x,
			.y = this->pos.y,
		};
	} break;
	case UI_LAYOUT_KIND_VERT: {
		return (Vector2) {
			.x = this->pos.x,
			.y = this->pos.y + this->size.y + this->padding.y,
		};
	} break;
	case UI_LAYOUT_KIND_COUNT:
	default: ASSERT(0, "Unreachable");
	}
	ASSERT(0, "Unreachable");

	return (Vector2) {0.f, 0.f};
}

void UI_Layout_push_widget(UI_Layout* this, Vector2 size) {
	switch (this->kind) {
	case UI_LAYOUT_KIND_HORZ: {
		this->size.x += size.x + this->padding.x;
		this->size.y = fmaxf(this->size.y, size.y);
	} break;
	case UI_LAYOUT_KIND_VERT: {
		this->size.x = fmaxf(this->size.x, size.x);
		this->size.y += size.y + this->padding.y;
	} break;
	case UI_LAYOUT_KIND_COUNT:
	default: ASSERT(0, "Unreachable");
	}
}

static void push_ui_widget(UI* this, UI_Layout* layout, Vector2 size) {
	switch (layout->kind) {
	case UI_LAYOUT_KIND_HORZ: {
		this->ui_rect.width += size.x;
		this->ui_rect.height = fmaxf(this->ui_rect.height, size.y);
	} break;
	case UI_LAYOUT_KIND_VERT: {
		this->ui_rect.width = fmaxf(this->ui_rect.width, size.x);
		this->ui_rect.height += size.y;
	} break;
	case UI_LAYOUT_KIND_COUNT:
	default: ASSERT(0, "Unreachable");
	}
    UI_Layout_push_widget(layout, size);
}

UI_Theme get_default_ui_theme(void) {
    UI_Theme res = {0};
    res.bg_color = GetColor(0x585B70FF),
    res.titlebar_color = GetColor(0x45475AFF),
	res.bg_padding = (Vector2) {10.f, 10.f};
    res.titlebar_pad_bottom = 0.5f;

    res.titlebar_padding = 4.f;
    set_ui_theme_titlebar_font_size(&res, 12);
    return res;
}

void set_ui_theme_titlebar_font_size(UI_Theme* theme, int font_size) {
    theme->titlebar_font_size = font_size;
	theme->titlebar_height = (float)(theme->titlebar_font_size + (theme->titlebar_padding*2.f));
}

UI UI_make(UI_Theme theme, Font* font, Vector2 pos, cstr title, Vector2* mpos_ptr) {
	UI res = {0};
	res.active_id = -1;
	res.layouts_count = 0;
	res.pos = pos;
	res.font = font;
	res.btn_padding = (Vector2) {4.f, 4.f};
	res.text_input_width = 12;
	res.text_input_cursor_blink_alarm.alarm_time = 0.5f;
	res.show_text_input_cursor = true;
	res.draw_element_stack = UI_Draw_element_stack_make();
	res.title = title;
    res.show = true;
    res.theme = theme;
    res.mpos_ptr = mpos_ptr;
	return res;
}

void UI_push_layout(UI* this, UI_Layout layout) {
	ASSERT(this->layouts_count < LAYOUTS_CAP, "Layouts exceeded");
	this->layouts[this->layouts_count++] = layout;
}

UI_Layout UI_pop_layout(UI* this) {
	ASSERT(this->layouts_count > 0, "Layouts exceeded");
	return this->layouts[--this->layouts_count];
}

UI_Layout* UI_top_layout(UI* this) {
	if (this->layouts_count > 0)
		return &this->layouts[this->layouts_count - 1];
	return NULL;
}

void UI_begin_layout(UI* this, UI_Layout_kind kind) {
	UI_Layout* prev = UI_top_layout(this);
	if (prev == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return;
	}

	UI_Layout next = {0};
	next.kind = kind;
	next.pos = UI_Layout_available_pos(prev);
	next.size = (Vector2) {0.f, 0.f};
	UI_push_layout(this, next);
}

void UI_end_layout(UI* this) {
	UI_Layout child = UI_pop_layout(this);
	UI_Layout* parent = UI_top_layout(this);
	if (parent == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return;
	}
    push_ui_widget(this, parent, child.size);
	/* UI_Layout_push_widget(parent, child.size); */
}

const char *UI_Draw_element_type_as_str(const UI_Draw_element_type t) {
	switch (t) {
        case UI_DRAW_ELEMENT_TYPE_RECT: return "Rect";
        case UI_DRAW_ELEMENT_TYPE_BOX: return "Box";
        case UI_DRAW_ELEMENT_TYPE_SPRITE: return "Sprite";
        case UI_DRAW_ELEMENT_TYPE_SPRITE_FRAME: return "Frame";
        case UI_DRAW_ELEMENT_TYPE_TEXT: return "Text";
        case UI_DRAW_ELEMENT_TYPE_LINE: return "Line";
        case UI_DRAW_ELEMENT_TYPE_COUNT:
        default: ASSERT(false, "UNREACHABLE!");
    }
}

UI_Draw_element_stack UI_Draw_element_stack_make(void) {
	UI_Draw_element_stack res = {0};

	const size_t draw_element_count = 100;
	res.arena = arena_make(sizeof(UI_Draw_element)*(draw_element_count+1));
	res.buff = (UI_Draw_element*)arena_alloc(&res.arena, sizeof(UI_Draw_element)*draw_element_count);

	return res;
}

void UI_Draw_element_stack_push(UI_Draw_element_stack* stack, UI_Draw_element val) {
	stack->buff[stack->count++] = val;
}

bool UI_Draw_element_stack_pop(UI_Draw_element_stack* stack, UI_Draw_element* popped) {
	if (stack->count == 0) {
		return false;
	} else {
		*popped = stack->buff[--stack->count];
	}
	return true;
}

void UI_Draw_element_stack_free(UI_Draw_element_stack* stack) {
	arena_free(&stack->arena);
}

void UI_begin(UI* this, UI_Layout_kind kind) {
	UI_Layout layout = {0};
	layout.pos = Vector2Add(this->pos, (Vector2) {0.f, this->theme.titlebar_height*(1+this->theme.titlebar_pad_bottom)});
    layout.pos = Vector2Add(layout.pos, this->scroll_offset);
	layout.kind = kind;
	UI_push_layout(this, layout);

    this->ui_rect.width = 0.f;
    this->ui_rect.height = 0.f;
}

bool UI_button(UI* this, cstr text, int font_size, Color color) {
	int id = this->last_used_id++;
	UI_Layout* top = UI_top_layout(this);
	if (top == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return false;
	}

	const Vector2 pos = UI_Layout_available_pos(top);
	const Vector2 size = Vector2Add(MeasureTextEx(*this->font, text, font_size, 1.f), Vector2Scale(this->btn_padding, 2.f));
	const Rectangle rect = {
			.x = pos.x,
			.y = pos.y,
			.width = size.x,
			.height = size.y,
	};
	bool click = false;
	Vector2 mpos = *this->mpos_ptr;
	bool hovering = CheckCollisionPointRec(mpos, rect);
	if (this->active_id == id) {
		if (mouse_button_released_unignored(MOUSE_BUTTON_LEFT)) {
			this->active_id = -1;
			if (hovering) {
                click = true;
			}
		}
	} else {
		if (hovering && mouse_button_pressed_unignored(MOUSE_BUTTON_LEFT)) {
			this->active_id = id;
		}
	}

	float alpha = 0.4f;
	if (hovering) {
		alpha = 0.5f;
	}

	bool is_clicked = (hovering && mouse_button_down_unignored(MOUSE_BUTTON_LEFT));
	if (is_clicked) {
		alpha = 1.f;
	}

	UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
			.type = UI_DRAW_ELEMENT_TYPE_BOX,
			.pos =	(Vector2) { rect.x, rect.y },
			.size = (Vector2) { rect.width, rect.height },
			.fill_color = ColorAlpha(color, 0.f),
			.out_color = WHITE,
		});

	Vector2 draw_pos = Vector2Add(pos, this->btn_padding);
	if (is_clicked) {
		draw_pos = Vector2AddValue(draw_pos, 1);
	}

	UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
			.type = UI_DRAW_ELEMENT_TYPE_TEXT,
			.pos = draw_pos,
			.fill_color = WHITE,
			.out_color = WHITE,
			.spr = NULL,
			.font = this->font,
			.text = text,
			.font_size = font_size,
		});

	UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
			.type = UI_DRAW_ELEMENT_TYPE_RECT,
			.pos =	(Vector2) { rect.x, rect.y },
			.size = (Vector2) { rect.width, rect.height },
			.fill_color = ColorAlpha(color, alpha),
		});
    push_ui_widget(this, top, size);
	/* UI_Layout_push_widget(top, size); */

    if (!this->show) click = false;
    if (click) ignore_mouse_input(true);

	return click;
}

void UI_text(UI* this, cstr text, int font_size, Color color) {
	int id = this->last_used_id++;
	(void)id;
	UI_Layout* top = UI_top_layout(this);
	if (top == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return;
	}

	const Vector2 pos = UI_Layout_available_pos(top);
	const Vector2 size = Vector2Add(MeasureTextEx(*this->font, text, font_size, 1.f), Vector2Scale(this->btn_padding, 2.f));
	/* draw_text(ctx, this->font, text, pos, font_size, color); */
	UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
			.type = UI_DRAW_ELEMENT_TYPE_TEXT,
			.pos = pos,
			.fill_color = color,
			.out_color = color,
			.spr = NULL,
			.font = this->font,
			.text = text,
			.font_size = font_size,
		});

    push_ui_widget(this, top, size);
    /* UI_Layout_push_widget(top, size); */
}

void UI_line(UI* this, float thick, Color color) {
	int id = this->last_used_id++;
	(void)id;
	UI_Layout* top = UI_top_layout(this);
	if (top == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return;
	}

	Vector2 pos = UI_Layout_available_pos(top);
    pos.y += thick;
	const Vector2 size = v2(this->ui_rect.width, thick * 4);

	UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
			.type = UI_DRAW_ELEMENT_TYPE_LINE,
			.pos = pos,
			.fill_color = color,
			.out_color = color,
			.spr = NULL,
            .thick = thick,
		});

    push_ui_widget(this, top, size);
}

void UI_sprite(UI* this, Sprite* spr) {
	int id = this->last_used_id++;
	(void)id;
	UI_Layout* top = UI_top_layout(this);
	if (top == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return;
	}

	const Vector2 pos = UI_Layout_available_pos(top);
	const Vector2 size = (Vector2) { spr->tex_rect.width*spr->scale.x, spr->tex_rect.height*spr->scale.y };
	/* draw_sprite_at(ctx, spr, pos); */
	UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
			.type = UI_DRAW_ELEMENT_TYPE_SPRITE,
			.pos = pos,
			.fill_color = WHITE,
			.out_color = WHITE,
			.spr = spr,
			.font = NULL,
			.text = NULL,
			.font_size = 0,
		});

    push_ui_widget(this, top, size);
	/* UI_Layout_push_widget(top, size); */
}

bool UI_sprite_button(UI* this, Sprite* spr) {
	int id = this->last_used_id++;
	UI_Layout* top = UI_top_layout(this);
	if (top == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return false;
	}

	const Vector2 pos = UI_Layout_available_pos(top);
	const Vector2 size = (Vector2) { spr->tex_rect.width, spr->tex_rect.height };
	const Rectangle rect = {
			.x = pos.x,
			.y = pos.y,
			.width = size.x,
			.height = size.y,
	};
	bool click = false;
	Vector2 mpos = *this->mpos_ptr;
	bool hovering = CheckCollisionPointRec(mpos, rect);
	if (this->active_id == id) {
		if (mouse_button_released_unignored(MOUSE_BUTTON_LEFT)) {
			this->active_id = -1;
			if (hovering) {
	click = true;
			}
		}
	} else {
		if (hovering && mouse_button_pressed_unignored(MOUSE_BUTTON_LEFT)) {
			this->active_id = id;
		}
	}

	float alpha = 0.4f;
	if (hovering) {
		alpha = 0.5f;
	}

	bool is_clicked = (hovering && mouse_button_down_unignored(MOUSE_BUTTON_LEFT));
	if (is_clicked) {
		alpha = 1.f;
	}

	Color color = spr->tint;
	color.a = alpha;
	UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
			.type = UI_DRAW_ELEMENT_TYPE_SPRITE,
			.pos = pos,
			.fill_color = color,
			.out_color = color,
			.spr = spr,
			.font = NULL,
			.text = NULL,
			.font_size = 0,
		});


    push_ui_widget(this, top, size);
	/* UI_Layout_push_widget(top, size); */

	// @Cleanup: why are we ignoring mouse input unconditionally here????
	/* ignore_mouse_input(true); */

    if (!this->show) click = false;
    if (click) ignore_mouse_input(true);
	return click;
}

bool UI_sprite_button_frame(UI* this, Sprite* spr, int hframe, int vframe) {
	int id = this->last_used_id++;
	UI_Layout* top = UI_top_layout(this);
	if (top == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return false;
	}

	const Vector2 pos = UI_Layout_available_pos(top);
	const Vector2 size = (Vector2) { spr->tex_rect.width, spr->tex_rect.height };
	const Rectangle rect = {
			.x = pos.x,
			.y = pos.y,
			.width = size.x,
			.height = size.y,
	};
	bool click = false;
	Vector2 mpos = *this->mpos_ptr;
	bool hovering = CheckCollisionPointRec(mpos, rect);
	if (this->active_id == id) {
		if (mouse_button_released_unignored(MOUSE_BUTTON_LEFT)) {
			this->active_id = -1;
			if (hovering) {
	click = true;
			}
		}
	} else {
		if (hovering && mouse_button_pressed_unignored(MOUSE_BUTTON_LEFT)) {
			this->active_id = id;
		}
	}

	float alpha = 0.4f;
	if (hovering) {
		alpha = 0.5f;
	}

	bool is_clicked = (hovering && mouse_button_down_unignored(MOUSE_BUTTON_LEFT));
	if (is_clicked) {
		alpha = 1.f;
	}

	Color color = spr->tint;
	color.a = alpha;
	/* draw_sprite_at(ctx, spr, pos); */
	UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
			.type = UI_DRAW_ELEMENT_TYPE_SPRITE_FRAME,
			.pos = pos,
			.fill_color = color,
			.out_color = color,
			.spr = spr,
			.hframe = hframe,
			.vframe = vframe,
			.font = NULL,
			.text = NULL,
			.font_size = 0,
		});

    push_ui_widget(this, top, size);
	/* UI_Layout_push_widget(top, size); */

	// @Cleanup: why are we ignoring mouse input unconditionally here????
	/* ignore_mouse_input(true); */

    if (!this->show) click = false;
    if (click) ignore_mouse_input(true);
	return click;
}

void UI_spacing(UI* this, float spacing) {
	int id = this->last_used_id++;
	(void)id;
	UI_Layout* top = UI_top_layout(this);
	if (top == NULL) {
		log_error("This function must be used between 'begin' and 'end'!");
		return;
	}

	Vector2 size = {
		.x = spacing,
		.y = 0.f,
	};

	if (top->kind == UI_LAYOUT_KIND_VERT) {
		size.x = 0.f;
		size.y = spacing;
	}

    push_ui_widget(this, top, size);
	/* UI_Layout_push_widget(top, size); */
}

/* void UI_text_input(UI* this, char* text_buff, uint32 text_buff_size, uint32* cursor_ptr, int font_size, Color color) { */
/*	 uint32 cursor = (*cursor_ptr); */
/*	 int id = this->last_used_id++; */
/*	 UI_Layout* top = UI_top_layout(this); */
/*	 if (top == NULL) { */
/*		 log_error("This function must be used between 'begin' and 'end'!"); */
/*		 return; */
/*	 } */

/*	 const Vector2 pos = UI_Layout_available_pos(top); */
/*	 // TODO: maybe have text input padding? */
/*	 const Vector2 size = Vector2Add((Vector2) {this->text_input_width * (float32)font_size, (float32)font_size}, Vector2Scale(this->btn_padding, 2.f)); */
/*	 const Rectangle rect = { */
/*			 .x = pos.x, */
/*			 .y = pos.y, */
/*			 .width = size.x, */
/*			 .height = size.y, */
/*	 }; */
/*   Vector2 mpos = *this->mpos_ptr; */
/*	 bool hovering = CheckCollisionPointRec(mpos, rect); */
/*	 if (this->active_id == id) { */
/*		 bool pressed = false; */

/*		 // Backspace */
/*		 if (clock_key_pressed(ctx, KEY_BACKSPACE) && cursor > 0) { */
/*			 uint32 n = text_buff_size - cursor; */
/*			 if (n == 0) { */
/* 	text_buff[--cursor] = '\0'; */
/*			 } else { */
/* 	memcpy((uint8*)text_buff+(cursor-1), (uint8*)text_buff+cursor, n); */
/* 	cursor--; */
/* 	// Edge case: text buffer is full, cursor is not at the end of text buffer */
/* 	if (text_buff[text_buff_size-1] != '\0') { */
/* 		memset((uint8*)text_buff+cursor+n, 0, text_buff_size - (cursor + n)); */
/* 	} */
/*			 } */
/*			 pressed = true; */
/*		 } */

/*		 // Delete */
/*		 if (clock_key_pressed(ctx, KEY_DELETE) && */
/* 	cursor < (uint32)strlen(text_buff)) { */
/*			 uint32 n = text_buff_size - cursor; */

/*			 if (n == 0) { */
/* 	text_buff[cursor--] = '\0'; */
/*			 } else { */
/* 	memcpy((uint8*)text_buff+(cursor), (uint8*)text_buff+(cursor+1), n-1); */
/* 	// TODO: Check for edge-case */
/*			 } */
/*			 pressed = true; */
/*		 } */

/*		 // enter */
/*		 if (clock_key_pressed(ctx, KEY_ENTER)) { */
/*			 if (clock_key_held(ctx, KEY_LEFT_CONTROL)) { */
/* 	this->active_id = -1; */
/*			 } else { */
/* 	/\* 	text_buff[cursor] = '\n'; *\/ */
/* 	/\* 	cursor++; *\/ */
/*			 } */
/*			 pressed = true; */
/*		 } */

/*		 if (ctx->text_entered) { */
/*			 text_buff[cursor] = (char)ctx->last_entered_character; */
/*			 cursor++; */
/*			 pressed = true; */
/*		 } */

/*		 // Pasting */
/*		 if (clock_key_held(ctx, KEY_LEFT_CONTROL)) { */
/*			 if (clock_key_pressed(ctx, KEY_V)) { */
/* 	cstr pasted_text = get_clipboard(); */
/* 	size_t pasted_text_len = strlen(pasted_text); */
/* 	ASSERT((pasted_text_len + cursor) <= text_buff_size); */
/* 	memcpy((uint8*)text_buff+cursor, pasted_text, pasted_text_len); */
/* 	cursor += (uint32)pasted_text_len; */
/*			 } */
/*			 pressed = true; */
/*		 } */

/*		 // Home/End */
/*		 if (clock_key_pressed(ctx, KEY_HOME)) { */
/*			 cursor = 0; */
/*			 pressed = true; */
/*		 } else if (clock_key_pressed(ctx, KEY_END)) { */
/*			 cursor = (uint32)strlen(text_buff); */
/*			 pressed = true; */
/*		 } */

/*		 // Cursor movement */
/*		 if (clock_key_pressed(ctx, KEY_LEFT)) { */
/*			 if (cursor > 0) cursor--; */
/*			 pressed = true; */
/*		 } */

/*		 if (clock_key_pressed(ctx, KEY_RIGHT)) { */
/*			 size_t text_len = strlen(text_buff); */
/*			 if (cursor < text_len) cursor++; */
/*			 pressed = true; */
/*		 } */

/*		 clock_eat_key_input(ctx); */

/*		 if (!hovering && clock_mouse_released(ctx, MOUSE_BUTTON_LEFT)) { */
/*			 this->active_id = -1; */
/*		 } */

/*		 if (!pressed) { */
/*			 if (Alarm_on_alarm(&this->text_input_cursor_blink_alarm, ctx->delta)) { */
/* 	this->show_text_input_cursor = !this->show_text_input_cursor; */
/*			 } */
/*		 } else { */
/*			 this->text_input_cursor_blink_alarm.time = 0.f; */
/*			 this->show_text_input_cursor = true; */
/*		 } */

/*	 } else { */
/*		 this->show_text_input_cursor = true; */
/*		 if (hovering && mouse_button_pressed(ctx, MOUSE_BUTTON_LEFT)) { */
/*			 this->active_id = id; */
/*			 clock_eat_mouse_input(ctx); */
/*		 } */
/*	 } */

/*	 Color fill_col = color_alpha(WHITE, this->active_id == id ? 0.2f : 0.f); */
/*	 Vector2 text_box_pos = pos; */
/*	 /\* draw_box(ctx, rect, WHITE, fill_col); *\/ */
/*	 UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) { */
/*			 .type = UI_DRAW_ELEMENT_TYPE_BOX, */
/*			 .pos = rect.pos, */
/*			 .size = rect.size, */
/*			 .fill_color = fill_col, */
/*			 .out_color = WHITE, */
/*			 .spr = NULL, */
/*			 .font = NULL, */
/*			 .text = NULL, */
/*			 .font_size = 0, */
/*		 }); */

/*	 // TODO: look previous todo... */
/*	 Vector2 text_pos = Vector2Add(pos, this->btn_padding); */
/*	 float text_width = get_text_size(this->ctx, this->font, text_buff, font_size).x; */
/*	 float text_box_width = ((float32)this->text_input_width * (float32)font_size); */
/*	 if (text_width > text_box_width) { */
/*		 text_pos.x -= text_width - text_box_width; */
/*	 } */

/*	 // offset by cursor */
/*	 ASSERT(cursor <= text_buff_size); */
/*	 float text_width_until_cursor = get_text_sizen(this->ctx, this->font, text_buff, cursor, font_size).x; */
/*	 Rect cursor_rect = { */
/*		 .pos = (Vector2) {text_pos.x + text_width_until_cursor, text_pos.y}, */
/*		 .size = (Vector2) {font_size*0.2f, (float32)font_size} */
/*	 }; */

/*	 if (cursor_rect.pos.x < text_box_pos.x) { */
/*		 text_pos.x += text_box_pos.x - cursor_rect.pos.x; */
/*		 cursor_rect.pos.x = text_box_pos.x; */
/*	 } */

/*	 clock_begin_scissor(ctx, rect); */
/*	 color.a = 0.5f; */
/*	 if (this->active_id == id) { */
/*		 color.a = 1.f; */
/*	 } */

/*	 /\* draw_text(ctx, this->font, text_buff, text_pos, font_size, color); *\/ */
/*	 UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) { */
/*			 .type = UI_DRAW_ELEMENT_TYPE_TEXT, */
/*			 .pos = text_pos, */
/*			 .fill_color = color, */
/*			 .out_color = color, */
/*			 .spr = NULL, */
/*			 .font = this->font, */
/*			 .text = text_buff, */
/*			 .font_size = font_size, */
/*		 }); */

/*	 clock_end_scissor(ctx); */

/*	 // cursor */

/*	 if (this->show_text_input_cursor) { */
/*		 /\* draw_rect(ctx, cursor_rect, color_alpha(WHITE, (this->active_id == id ? 0.85f : 0.45f))); *\/ */
/*		 UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) { */
/*			 .type = UI_DRAW_ELEMENT_TYPE_RECT, */
/*			 .pos = cursor_rect.pos, */
/*			 .size = cursor_rect.size, */
/*			 .fill_color = color_alpha(WHITE, (this->active_id == id ? 0.85f : 0.45f)), */
/*		 }); */
/*	 } */

/*	 UI_Layout_push_widget(top, size); */
/*	 *cursor_ptr = cursor; */
/* } */
static void UI_titlebar(UI* this) {
	Vector2 rect_pos = Vector2Subtract(this->pos, (Vector2) {this->theme.bg_padding.x, 0.f});
	Vector2 rect_size = v2(this->bg_rect.width, 
                        fmaxf(this->theme.titlebar_height, this->theme.titlebar_font_size + (2*this->theme.titlebar_padding)));
	Rectangle titlebar = {
			.x = rect_pos.x,
			.y = rect_pos.y,
			.width = rect_size.x,
			.height = rect_size.y,
	};

	DrawRectangleRec(titlebar, this->theme.titlebar_color);
	/* DrawRectangleLinesEx(titlebar, 1.f, WHITE); */

	Vector2 title_pos = v2(titlebar.x + this->theme.titlebar_padding, titlebar.y + this->theme.titlebar_padding);
    /* Vector2 ui_size = get_ui_size(this); */
	DrawTextEx(*this->font, TextFormat("%s", this->title), title_pos, this->theme.titlebar_font_size, 1.f, WHITE);
	/* DrawTextEx(*this->font, TextFormat("%s %d, %d | %d, %d", this->title, (int)this->scroll_offset.x, (int)this->scroll_offset.y, (int)ui_size.x, (int)ui_size.y), title_pos, this->theme.titlebar_font_size, 1.f, WHITE); */
}

void UI_background(UI* this) {
	DrawRectangleRec(this->bg_rect, this->theme.bg_color);
	/* DrawRectangleLinesEx(this->bg_rect, 1.f, WHITE); */
    /* DrawRectangleLinesEx(this->ui_rect, 1.f, BLUE); */
}

void UI_draw(UI* this) {
    UI_titlebar(this); if (!this->show) {
        // @NOTE: Clean up draw element stack
        this->draw_element_stack.count = 0;
        return;
    }
	UI_background(this);

    BeginScissorMode(this->bg_rect.x, this->bg_rect.y, this->bg_rect.width, this->bg_rect.height);
	UI_Draw_element elm = {0};
	while (UI_Draw_element_stack_pop(&this->draw_element_stack, &elm)) {
		switch (elm.type) {
            case UI_DRAW_ELEMENT_TYPE_RECT: {
                DrawRectangleRec((Rectangle) {elm.pos.x, elm.pos.y, elm.size.x, elm.size.y}, elm.fill_color);
            } break;
            case UI_DRAW_ELEMENT_TYPE_BOX: {
                DrawRectangleRec((Rectangle) {elm.pos.x, elm.pos.y, elm.size.x, elm.size.y}, elm.fill_color);
                DrawRectangleLinesEx((Rectangle) {elm.pos.x, elm.pos.y, elm.size.x, elm.size.y}, 1.f, elm.out_color);
            } break;
            case UI_DRAW_ELEMENT_TYPE_SPRITE: {
                Color previous_tint = elm.spr->tint;
                elm.spr->tint = elm.fill_color;
                Vector2 prev_spr_pos = elm.spr->pos;
                elm.spr->pos = elm.pos;
                draw_sprite(elm.spr);
                elm.spr->pos = prev_spr_pos;
                elm.spr->tint = previous_tint;
            } break;
            case UI_DRAW_ELEMENT_TYPE_SPRITE_FRAME: {
                Color previous_tint = elm.spr->tint;
                int prev_hframe = elm.spr->hframe;
                int prev_vframe = elm.spr->vframe;
                set_sprite_hframe(elm.spr, elm.hframe);
                set_sprite_vframe(elm.spr, elm.vframe);
                elm.spr->tint = elm.fill_color; Vector2 prev_spr_pos = elm.spr->pos;
                elm.spr->pos = elm.pos;
                draw_sprite(elm.spr);
                elm.spr->pos = prev_spr_pos;
                elm.spr->tint = previous_tint;
                set_sprite_hframe(elm.spr, prev_hframe);
                set_sprite_vframe(elm.spr, prev_vframe);
            } break;
            case UI_DRAW_ELEMENT_TYPE_TEXT: {
                DrawTextEx(*elm.font, elm.text, elm.pos, elm.font_size, 1.f, elm.fill_color);
            } break;
            case UI_DRAW_ELEMENT_TYPE_LINE: {
                Vector2 pos2 = v2(this->ui_rect.x + this->ui_rect.width, elm.pos.y);
                DrawLineEx(elm.pos, pos2, elm.thick, elm.out_color);
            } break;
            case UI_DRAW_ELEMENT_TYPE_COUNT:
            default: ASSERT(0, "Unreachable!");
		}
	}
    EndScissorMode();
    // @TEMP
    // DrawCircleV(this->pos, 16.f, RED);
}

void UI_end(UI* this) {
    // Calculate bg_rect
    /* Vector2 ui_size      = get_ui_size(this); */
    Vector2 pos          = get_ui_bg_rect_pos(this);
    this->bg_rect.x      = pos.x;
    this->bg_rect.y      = pos.y;
    this->bg_rect.width  = this->ui_rect.width + this->theme.bg_padding.x;
    this->bg_rect.height  = this->ui_rect.height + this->theme.bg_padding.y;

    this->ui_rect.x      = this->bg_rect.x + this->theme.bg_padding.x;
    this->ui_rect.y      = this->bg_rect.y + this->theme.bg_padding.y;
    this->ui_rect.width   -= this->theme.bg_padding.x;
    this->ui_rect.height  -= this->theme.bg_padding.y;
    /* this->ui_rect.width  = 300.f; */
    /* this->ui_rect.height = 125.f; */

	Vector2 title_pos  = this->pos;
	Vector2 title_size = (Vector2) {this->bg_rect.width, this->theme.titlebar_height};
	Rectangle titlebar = {
			.x = title_pos.x,
			.y = title_pos.y,
			.width = title_size.x,
			.height = title_size.y,
	};
	if (!mouse_button_down_unignored(MOUSE_BUTTON_LEFT)) {
		this->is_moving = false;
	}

	Vector2 mpos = *this->mpos_ptr;
	if (mouse_button_pressed_unignored(MOUSE_BUTTON_LEFT) &&
			CheckCollisionPointRec(mpos, titlebar)) {
		/* this->pos_offset = Vector2Subtract(Vector2Subtract(mpos, title_pos), (Vector2) {this->theme.bg_padding.x, 0.f}); */
		this->pos_offset = Vector2Subtract(mpos, title_pos);
		this->is_moving = true;
	}

	if (this->is_moving) {
        ignore_mouse_input(true);
        this->pos = Vector2Subtract(mpos, this->pos_offset);
	} else {
        if (mouse_button_pressed_unignored(MOUSE_BUTTON_MIDDLE) &&
			CheckCollisionPointRec(mpos, titlebar)) {
            this->show = !this->show;
        }
    }

	// eat mouse input if clicked on ui rect
    Vector2 size = { this->bg_rect.width, this->bg_rect.height };

     Rectangle rect = {
        pos.x,
        pos.y,
        size.x,
        size.y
     };

     if ((mouse_button_down_unignored(MOUSE_BUTTON_LEFT) || mouse_button_down_unignored(MOUSE_BUTTON_MIDDLE) || mouse_button_down_unignored(MOUSE_BUTTON_RIGHT))&&
         (CheckCollisionPointRec(mpos, rect) || CheckCollisionPointRec(mpos, titlebar))) {
         ignore_mouse_input(true);
     }

	this->last_used_id = 0;
	UI_pop_layout(this);
}

void UI_free(UI* this) {
	UI_Draw_element_stack_free(&this->draw_element_stack);
}

// Sprite
bool init_sprite(Sprite* spr, Texture2D texture, size_t hframes, size_t vframes) {
	spr->texture = texture;
	spr->hframes = hframes;
	spr->vframes = vframes;
	spr->pos = (Vector2) {0.f, 0.f};
	spr->width = (float)spr->texture.width;
	spr->height = (float)spr->texture.height;
	spr->scale = (Vector2) {1.f, 1.f};
	spr->tex_rect.width = spr->width / (float)spr->hframes;
	spr->tex_rect.height = spr->height / (float)spr->vframes;
	set_sprite_hframe(spr, hframes);
	set_sprite_vframe(spr, vframes);
	spr->tint = WHITE;

	spr->time_per_frame = SPRITE_DEFAULT_TIME_PER_FRAME;
	spr->accumulated_time = 0.f;
	return true;
}

bool init_sprite_from_sheet(Sprite *spr, Texture tex, Vector2i offset, Vector2i size, size_t hframes, size_t vframes) {
	spr->texture = tex;
	spr->hframes = hframes;
	spr->vframes = vframes;
    spr->tex_offset = offset;
	spr->pos = (Vector2) {0.f, 0.f};
	spr->width  = (float)size.x;
	spr->height = (float)size.y;
	spr->scale = (Vector2) {1.f, 1.f};
	spr->tex_rect.width  = spr->width;
	spr->tex_rect.height = spr->height;
	set_sprite_hframe(spr, hframes);
	set_sprite_vframe(spr, vframes);
	spr->tint = WHITE;

	spr->time_per_frame = SPRITE_DEFAULT_TIME_PER_FRAME;
	spr->accumulated_time = 0.f;
	return true;
}

void update_sprite_tex_rect(Sprite *spr) {
	spr->tex_rect = (Rectangle) {
		.x = spr->tex_offset.x + (spr->tex_rect.width * (float)spr->hframe),
		.y = spr->tex_offset.y + (spr->tex_rect.height * (float)spr->vframe),
		.width  = spr->tex_rect.width,
		.height = spr->tex_rect.height,
	};
}

void set_sprite_hframe(Sprite* spr, size_t hframe) {
	if (hframe > spr->hframes-1) hframe = 0;
	spr->hframe = hframe;
	update_sprite_tex_rect(spr);
}

void set_sprite_vframe(Sprite* spr, size_t vframe) {
	if (vframe > spr->vframes-1) vframe = 0;
	spr->vframe = vframe;
	update_sprite_tex_rect(spr);
}

void center_sprite_origin(Sprite* spr) {
	spr->origin.x = spr->tex_rect.width / 2.f;
	spr->origin.y = spr->tex_rect.height / 2.f;
}

void draw_sprite(Sprite* spr) {
		Rectangle dest = {
				.x = spr->pos.x,
				.y = spr->pos.y,
				.width	= spr->tex_rect.width * spr->scale.x,
				.height = spr->tex_rect.height * spr->scale.y,
		};
		Vector2 origin = CLITERAL(Vector2) {
				.x = spr->origin.x * spr->scale.x,
				.y = spr->origin.y * spr->scale.y,
		};
		DrawTexturePro(spr->texture, spr->tex_rect, dest, origin, spr->rotation, spr->tint);
		/* DrawTexture(spr->texture, spr->pos.x, spr->pos.y, spr->tint); */
}

void animate_sprite_hframes(Sprite* spr, float delta) {
	spr->accumulated_time += delta;
	if (spr->accumulated_time >= spr->time_per_frame) {
		spr->accumulated_time -= spr->time_per_frame;
		set_sprite_hframe(spr, spr->hframe+1);
	}
}

void free_sprite(Sprite* spr) {
		(void)spr;
}

void set_sprite_scale_scalar(Sprite *spr, float scl) {
    spr->scale = v2xx(scl);
}

// TextBox
Textbox make_textbox(Font font, int fs, Color active_color, Color inactive_color, Vector2 pos, Vector2 size, size_t buff_size, const char *name, char ignore_char) {
		Textbox tbox = {
				.buff = calloc(buff_size, sizeof(char)),
				.buff_size = buff_size,
				.name = name,
				.font = font,
				.pos = pos,
				.size = size,
				.font_size = fs,
				.active_color = active_color,
				.inactive_color = inactive_color,
				.ignoring_input = true,
				.ignore_char = ignore_char,
		};

		return tbox;
}

void free_textbox(Textbox *tbox) {
		if (!tbox) return;
		if (tbox->buff) free(tbox->buff);
}

bool update_textbox(Textbox *tbox) {
		if (tbox->active) {
				if (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(tbox->deactivate_key)) {
						tbox->active = false;
						tbox->ignoring_input = true;
				}
		} else {
				if (IsKeyPressed(tbox->activate_key)) {
						tbox->active = true;
				}
		}

		return tbox->active;
}

bool input_to_textbox(Textbox *tbox) {
		if (!tbox->active) return false;
		if (input_to_buff_ignored(tbox->buff, tbox->buff_size, &tbox->cursor, tbox->ignore_char, &tbox->ignoring_input)) {
				tbox->ignoring_input = true;
				return true;
		}
		return false;
}

void set_textbox_keys(Textbox *tbox, int activate, int deactivate) {
		tbox->activate_key = activate;
		tbox->deactivate_key = deactivate;
}

void draw_textbox(Textbox *tbox) {
		float buff_measured = MeasureTextEx(tbox->font, tbox->buff, tbox->font_size, 1.f).x;
		float name_measured = MeasureTextEx(tbox->font, tbox->name, tbox->font_size, 1.f).x;
		float measure_pad = 20.f;
		int buff_x = tbox->pos.x + name_measured;
		Rectangle rect = {
				.x = buff_x + measure_pad,
				.y = tbox->pos.y,
				.width = fmaxf(tbox->size.x, buff_measured*1.1),
				.height = tbox->size.y,
		};
		draw_text(tbox->font, tbox->name, tbox->pos, tbox->font_size, tbox->active ? tbox->active_color : tbox->inactive_color);
		draw_text(tbox->font, tbox->buff, v2(buff_x+20+2, tbox->pos.y), tbox->font_size, tbox->active ? tbox->active_color : tbox->inactive_color);
		DrawRectangleLinesEx(rect, 1, tbox->active ? tbox->active_color : tbox->inactive_color);
}

// Rectangle
bool rect_contains_point(Rectangle r1, Vector2 p) {
	return (p.x >= r1.x && p.x < r1.x + r1.width &&
		p.y >= r1.y && p.y < r1.y + r1.height);
}

bool rect_contains_rect(Rectangle r1, Rectangle r2) {
	return (rect_contains_point(r1, v2(r2.x, r2.y)) &&
		rect_contains_point(r1, (Vector2) {r2.x + r2.width,
							 r2.y + r2.height}));
}

bool rect_intersects_rect(Rectangle r1, Rectangle r2) {
	const float rect1_l = r1.x;
	const float rect1_r = r1.x+r1.width;
	const float rect1_t = r1.y;
	const float rect1_b = r1.y+r1.height;

	const float rect2_l = r2.x;
	const float rect2_r = r2.x+r2.width;
	const float rect2_t = r2.y;
	const float rect2_b = r2.y+r2.height;

	return (rect1_r >= rect2_l &&
		rect1_l <= rect2_r &&
		rect1_t <= rect2_b &&
		rect1_b >= rect2_t);
}

bool rect_resolve_rect_collision(Rectangle* rect1, const Rectangle rect2) {
	const float rect1_l = rect1->x;
	const float rect1_r = rect1->x+rect1->width;
	const float rect1_t = rect1->y;
	const float rect1_b = rect1->y+rect1->height;

	const float rect2_l = rect2.x;
	const float rect2_r = rect2.x+rect2.width;
	const float rect2_t = rect2.y;
	const float rect2_b = rect2.y+rect2.height;

	// resolve collision only if it ever happens
	if (rect_intersects_rect(*rect1, rect2)) {
		Vector2 cb2_bot = {0.f, rect2_b};
		Vector2 cb1_top = {0.f, rect1_t};
		float d2_top = v2_mag2(v2_sub(cb1_top, cb2_bot));
		Vector2 cb2_left = {rect2_l, 0.f};
		Vector2 cb1_right = {rect1_r, 0.f};
		float d2_right = v2_mag2(v2_sub(cb1_right, cb2_left));
		Vector2 cb2_right = {rect2_r, 0.f};
		Vector2 cb1_left = {rect1_l, 0.f};
		float d2_left = v2_mag2(v2_sub(cb1_left, cb2_right));
		Vector2 cb2_top = {0.f, rect2_t};
		Vector2 cb1_bot = {0.f, rect1_b};
		float d2_bot = v2_mag2(v2_sub(cb1_bot, cb2_top));

		float min_d2 = fminf(d2_top, fminf(d2_left, fminf(d2_right, d2_bot)));

		if (min_d2 == d2_top) {
			rect1->y = rect2_b;
		} else if (min_d2 == d2_left) {
			rect1->x = rect2_r;
		} else if (min_d2 == d2_right) {
			rect1->x = rect2_l - rect1->width;
		} else if (min_d2 == d2_bot) {
			rect1->y = rect2_t - rect1->height;
		} else {
			ASSERT(0, "UNREACHABLE");
		}
		return true;
	}
	return false;
}

// void rect_get_3d_points(Rectangle rect, Vector3f* p0, Vector3f* p1, Vector3f* p2, Vector3f* p3) {
//	 Vector2 p0_ = v2_add(rect.pos, (Vector2) {0.f, 0.f});
//	 Vector2 p1_ = v2_add(rect.pos, (Vector2) {rect.size.x, 0.f});
//	 Vector2 p2_ = v2_add(rect.pos, (Vector2) {rect.size.x, rect.size.y});
//	 Vector2 p3_ = v2_add(rect.pos, (Vector2) {0.f, rect.size.y});
//
//	 *p0 = (Vector3f) {p0_.x, p0_.y, 0.f};
//	 *p1 = (Vector3f) {p1_.x, p1_.y, 0.f};
//	 *p2 = (Vector3f) {p2_.x, p2_.y, 0.f};
//	 *p3 = (Vector3f) {p3_.x, p3_.y, 0.f};
// }
//
// void rect_get_points(Rectangle rect, Vector2* p0, Vector2* p1, Vector2* p2, Vector2* p3) {
//	 *p0 = v2_add(rect.pos, (Vector2) {0.f, 0.f});
//	 *p1 = v2_add(rect.pos, (Vector2) {rect.size.x, 0.f});
//	 *p2 = v2_add(rect.pos, (Vector2) {rect.size.x, rect.size.y});
//	 *p3 = v2_add(rect.pos, (Vector2) {0.f, rect.size.y});
// }
//

// Setup
RenderTexture2D init_window(int screen_width, int screen_height, float scl, const char *title, int *width_out, int *height_out) {
	SetTraceLogLevel(LOG_NONE);
	InitWindow(screen_width, screen_height, title);

		int width = screen_width * scl;
		int height = screen_height * scl;

		*width_out = width;
		*height_out = height;

	log_info("Created Window with dimensions %dx%d", screen_width, screen_height);

	RenderTexture2D ren_tex = LoadRenderTexture((int)(width), (int)(height));
	if (!IsRenderTextureReady(ren_tex)) {
		log_error("Failed to create RenderTexture2D!");
		exit(1);
	}
	log_info("Created RenderTexture2D with dimensions %dx%d (Scaled down by %.2f)", ren_tex.texture.width, ren_tex.texture.height, scl);

	return ren_tex;
}

void close_window(RenderTexture2D ren_tex) {
	UnloadRenderTexture(ren_tex);
	CloseWindow();
}

// Draw
void draw_ren_tex(RenderTexture2D ren_tex, int screen_width, int screen_height) {
	const Rectangle src = {
		.x = 0,
		.y = 0,
		.width = ren_tex.texture.width,
		// NOTE: We flip the height because y-axis is flipped internally (in opengl land probably)
		.height = -ren_tex.texture.height,
	};

	const Rectangle dst = {
		.x = 0,
		.y = 0,
		.width	= screen_width,
		.height = screen_height,
	};
	DrawTexturePro(ren_tex.texture, src, dst, CLITERAL(Vector2) { 0.f, 0.f }, 0.f, WHITE);
}

void draw_text_aligned(Font font, const char *text, Vector2 pos, int font_size, const Text_align_v align_v, const Text_align_h align_h, Color color) {
		draw_text_aligned_ex(font, text, pos, font_size, align_v, align_h, 0.0, color);
}

void draw_text_aligned_ex(Font font, const char *text, Vector2 pos, int font_size, const Text_align_v align_v, const Text_align_h align_h, float rotation, Color color) {
	Vector2 origin = {0};
	// RLAPI Vector2 MeasureTextEx(Font font, const char *text, float fontSize, float spacing);		// Measure string size for Font
	float spacing = 2.f;
	Vector2 text_size = MeasureTextEx(font, text, font_size, spacing);

	switch (align_h) {
		case TEXT_ALIGN_H_LEFT: {
		} break;
		case TEXT_ALIGN_H_CENTER: {
			origin.x = text_size.x * 0.5f;
		} break;
		case TEXT_ALIGN_H_RIGHT: {
			origin.x = text_size.x;
		} break;
		case TEXT_ALIGN_H_COUNT: {
		} break;
		default: ASSERT(false, "UNREACHABLE");
	}

	switch (align_v) {
		case TEXT_ALIGN_V_TOP: {
		} break;
		case TEXT_ALIGN_V_CENTER: {
			origin.y = text_size.y * 0.5f;
		} break;
		case TEXT_ALIGN_V_BOTTOM: {
			origin.y = text_size.y;
		} break;
		case TEXT_ALIGN_V_COUNT: {
		} break;
		default: ASSERT(false, "UNREACHABLE");
	}

	DrawTextPro(font, text, pos, origin, rotation, font_size, spacing, color);
}

void draw_text(Font font, const char *text, Vector2 pos, int font_size, Color color) {
	draw_text_aligned(font, text, pos, font_size, TEXT_ALIGN_V_TOP, TEXT_ALIGN_H_LEFT, color);
}

// NOTE: Input
bool input_to_buff(char *buff, size_t buff_cap, int *cursor) {
		return input_to_buff_ignored(buff, buff_cap, cursor, 0, NULL);
}

bool input_to_buff_ignored(char *buff, size_t buff_cap, int *cursor, char ignore, bool *ignoring) {
		int ch = 0;

		if ((*cursor) < 0) (*cursor) = 0;
		if ((*cursor) > buff_cap-1) (*cursor) = buff_cap-1;

		do {
				// Backspace
				if (IsKeyPressed(KEY_BACKSPACE) ||
						IsKeyPressedRepeat(KEY_BACKSPACE)) {
						if (*cursor > 0)
								buff[--(*cursor)] = '\0';
				}

				// Enter
				if (IsKeyPressed(KEY_ENTER)) {
						return true;
				}

				ch = GetCharPressed();

				if (ignoring && *ignoring && ignore > 0 && ch == ignore) {
						*ignoring = false;
						continue;
				}

				if (ch > 0) {
						buff[(*cursor)++] = (char)ch;
				}

		} while (ch > 0);
		return false;

}

// Assets Manager
bool load_texture(Texture_manager *tm, const char *filepath, Texture2D *tex_out) {
		return load_texture_(tm, filepath, tex_out, false);
}


bool load_texture_(Texture_manager *tm, const char *filepath, Texture2D *tex_out, bool verbose) {
	Texture_KV *tex_KV = shgetp_null(tm->texture_map, (char *)filepath);

	if (tex_KV != NULL) {
				if (tex_out)
						*tex_out = tex_KV->value;
				if (verbose)
						log_debug("Found '%s' at texture_map index [%zu]", filepath, shlenu(tm->texture_map));
	} else {
		Texture2D tex = LoadTexture(filepath);
		if (!IsTextureReady(tex)) return false;
				if (tex_out)
						*tex_out = tex;
		shput(tm->texture_map, (char *)filepath, tex);
				if (verbose)
						log_debug("Added '%s' to texture_map index [%zu]", filepath, shlenu(tm->texture_map));
	}

	return true;
}

// Console
Console make_console(int flags, Font font) {
		Console c = {0};

		c.font = font;
		c.flags = flags;

		Console_line l = {0};
		darr_append(c.lines, l);
		darr_append(c.unprefixed_lines, l);

		return c;
}

void add_line_to_console_simple(Console *console, char *line, Color color, bool hist) {
		Console_line cl = {
				.count = strlen(line),
				.color = color,
		};
		memcpy(cl.buff, line, cl.count);
		darr_append(console->lines, cl);
		darr_append(console->unprefixed_lines, cl);

	if (hist) {
		darr_append(console->history, cl);
	}
		console->hist_lookup_idx = console->history.count-1;
}

void add_line_to_console(Console *console, char *buff, size_t buff_size, Color color, bool hist) {
		Console_line cl = { .count = buff_size, };
		memcpy(cl.buff, buff, buff_size);
		cl.color = color;
		darr_append(console->lines, cl);
		darr_append(console->unprefixed_lines, cl);

	if (hist) {
		darr_append(console->history, cl);
	}
		console->hist_lookup_idx = console->history.count-1;
}

void add_line_to_console_prefixed(Console *console, Arena *tmp_arena, char *buff, Color color, bool hist) {
		const char *prefixed = arena_alloc_str(*tmp_arena, "%s%s%c%s", console->prefix, console->prefix2, console->prefix_symbol, buff);
		size_t prefixed_len = strlen(prefixed);

		Console_line ucl = { .count = strlen(buff) };
		memcpy(ucl.buff, buff, ucl.count);
		darr_append(console->unprefixed_lines, ucl);

		Console_line cl = { .count = prefixed_len, };
		memcpy(cl.buff, prefixed, prefixed_len);
		cl.color = color;
		darr_append(console->lines, cl);

	if (hist) {
		darr_append(console->history, cl);
	}
		console->hist_lookup_idx = console->history.count-1;
}

void add_character_to_console_line(Console *console, char ch, size_t line) {
		Console_line *l = get_console_line(console, line);
		Console_line *ul = &console->unprefixed_lines.items[line];
		if (l == NULL) {
				return;
		}

		l->buff[l->count++] = ch;
		ul->buff[ul->count++] = ch;
}

Console_line *get_console_line(Console *console, size_t line) {
		if (line >= console->lines.count) {
				log_error("Outofbounds: %zu is out of bounds of lines.count (%zu)", line, console->lines.count);
				return NULL;
		}

		return &console->lines.items[line];
}

Console_line *get_console_history(Console *console, size_t line) {
		if (line >= console->history.count) {
				log_error("Outofbounds: %zu is out of bounds of history.count (%zu)", line, console->history.count);
				return NULL;
		}

		return &console->history.items[line];
}

Console_line *get_or_create_console_line(Console *console, size_t line) {
		if (console->lines.count < line+1) {
				Console_line new_console_line = {0};
				darr_append(console->lines, new_console_line);
		}
		return get_console_line(console, line);
}

void clear_console_line(Console_line *cl) {
		if (cl == NULL) {
				log_warning("Console line is NULL!!");
				return;
		}
		memset(cl->buff, 0, CONSOLE_LINE_BUFF_CAP);
}

void clear_current_console_line(Console *console) {
		Console_line *cl = get_or_create_console_line(console, console->line);
		clear_console_line(cl);
		console->cursor = 0;
}

char *get_current_console_line_buff(Console *console) {
		if (console == NULL) return NULL;

		if (console->line >= console->lines.count) {
				log_error("Outofbounds: %d is out of bounds of lines.count (%zu)", console->line, console->lines.count);
				return NULL;
		}

		return console->lines.items[console->line].buff;
}

String_array get_current_console_args(Console *console) {
		String_array res = {0};

		const char *buff = get_current_console_line_buff(console);
		String_view sv = SV(buff);

		sv_trim(&sv);
		while (sv.count > 0) {
				sv_trim(&sv);
				String_view arg = {0};
				if (!sv_lpop_arg(&sv, &arg)) break;
				char *str = sv_to_cstr(arg);
				darr_append(res, str);

				// skip spaces between args
				sv_trim(&sv);
		}

		return res;
}

void readline_update(Console *console, Console_line *line) {
	Console_line *last_line = get_console_history(console, console->hist_lookup_idx);
	bool should_get_unprefixed_lines = GET_FLAG(console->flags, CONSOLE_FLAG_READLINE_USES_UNPREFIXED_LINES);
	if (should_get_unprefixed_lines) {
		if (console->hist_lookup_idx >= console->unprefixed_lines.count) {
			log_error("Outofbounds: %d is out of bounds of unprefixed_lines.count (%zu)", console->hist_lookup_idx, console->unprefixed_lines.count);
			return;
		}
		last_line = &console->unprefixed_lines.items[console->hist_lookup_idx];
	}
	if (last_line != NULL)	{
		memcpy(line->buff, last_line->buff, last_line->count);
		line->count = last_line->count;
		line->buff[line->count] = '\0';
		console->cursor = strlen(line->buff);
	}
}

bool input_to_console(Console *console, char *ignore_characters, size_t ignore_characters_count) {
	int ch = 0;
		Console_line *line = get_or_create_console_line(console, console->line);

		if (console->cursor < 0) console->cursor = 0;
		if (console->cursor > CONSOLE_LINE_BUFF_CAP-1) console->cursor = CONSOLE_LINE_BUFF_CAP-1;

	int chars_inputted = 0;

	do {
		ch = GetCharPressed();

		if (ch > 0) chars_inputted++;

				bool ignore = false;

				for (size_t i = 0; i < ignore_characters_count; ++i) {
						if (ch == ignore_characters[i]) {
								ignore = true;
								break;
						}
				}

				if (ignore) continue;

				if (IsKeyPressed(KEY_ENTER)) {
						if (console->prompting && console->expecting_values) {
								bool found = false;
								for (size_t i = 0; i < console->expected_prompt_values.count; ++i) {
										const char *expecting = console->expected_prompt_values.items[i]; 
										if (strcmp(line->buff, expecting) == 0) {
												console->selected_prompt_value_id = i;
												found = true;
												break;
										}
								}
								if (!found) {
										log_error_console((*console), "Prompt expects: ");
										for (int i = 0; i < console->expected_prompt_values.count; ++i) {
												log_error_console((*console), "		- %s", console->expected_prompt_values.items[i]);
										}
								}

								console->prompting = false;
								if (console->prompt_done_func)
										console->prompt_done_func(console, console->prompt_userdata);

								console->prompt_done_func = NULL;
								console->prompt_userdata = NULL;
								clear_current_console_line(console);

								darr_delete(console->lines, Console_line, console->prompt_line_id);

								return false;
						}

			// if (chars_inputted <= 0) {
			// 	darr_delete(console->lines, Console_line, console->line);
			// }

						return true;
				}

				// readline functionality
				if (IsKeyDown(KEY_LEFT_CONTROL)) {
						// Prev_line
						if (IsKeyPressed(KEY_P)) {
								if (console->lines.count > 0) {
										if (console->hist_lookup_idx > 1) {
						console->hist_lookup_idx--;
					}
					// log_info("%d", console->hist_lookup_idx);
					readline_update(console, line);
								}
						}

						if (IsKeyPressed(KEY_N)) {
								if (console->lines.count > 0) {
										if (console->hist_lookup_idx < (int)(console->history.count)-1) {
						console->hist_lookup_idx++;
					}

					// log_info("%d (%s)", console->hist_lookup_idx, console->hist_lookup_idx < console->history.count-2 ? "true" : "false");
					readline_update(console, line);
								}
						}
				}

				if (IsKeyPressed(KEY_BACKSPACE) ||
						IsKeyPressedRepeat(KEY_BACKSPACE)) {
						if (console->cursor > 0) {
								line->buff[--console->cursor] = '\0';
						}
				}

				if (line->count > CONSOLE_LINE_BUFF_CAP) {
						log_error("Exhausted line buff!");
						exit(1);
				}

				if (ch > 0) {
						// log_debug("TYPED %c AT %d:%d", (char)ch, console->line, console->cursor);
						// log_debug("CODEPOINT %c: %fx%f", ch, codepoint_rec.width, codepoint_rec.height);
						line->buff[console->cursor++] = (char)ch;
				}

				
	} while (ch > 0);

	// if (chars_inputted <= 0) {
	// 	darr_delete(console->lines, Console_line, console->line);
	// }

		return false;
}

// float get_cursor_offset(Console *console, int font_size) {
//		 Font font = console->font;
//
//		 char buf[1024];
//
//		 char *text = get_current_console_line_buff(console);
//		 memcpy(buf, text, console->cursor);
//		 buf[console->cursor] = '\0';
//
//		 Vector2 size = MeasureTextEx(font, buf, font_size, 1.f);
//
//		 return size.x;
// }

float get_cursor_offset(Console *console, int font_size) {
		Font font = console->font;
		const char *text = get_current_console_line_buff(console);
		float scale = (float)font_size / (float)font.baseSize;
		float x = 0.0f;

		for (int i = 0; i < console->cursor && text[i] != '\0'; ) {
				int codepoint = text[i];
				// GlyphInfo glyph = GetGlyphInfo(font, codepoint); // pseudo: access font.glyphs[...] or use raylib helpers
				i += 1;
				Rectangle glyph_atlas_rec = GetGlyphAtlasRec(font, codepoint);
				x += glyph_atlas_rec.width * scale;

				// log_info("%c: %f, %f, %fx%f", codepoint, glyph_atlas_rec.x, glyph_atlas_rec.y, glyph_atlas_rec.width, glyph_atlas_rec.height);
		}


		return x;
}

void draw_console(Console *console, Rectangle rect, Vector2 pad, int font_size, Color fill_color, Color border_color, float alpha) {
		Vector2 pos = {rect.x, rect.y + (rect.height - font_size)};
		pos = Vector2Add(pos, pad);
		DrawRectangleRec(rect, fill_color);
		DrawRectangleLinesEx(rect, 1.f, ColorAlpha(border_color, alpha));
		BeginScissorMode(rect.x, rect.y, rect.width, rect.height);

		for (size_t i = 0; i < console->lines.count; ++i) {
				Console_line *line = &console->lines.items[console->lines.count - i - 1];
				draw_text(GetFontDefault(), line->buff, pos, font_size, ColorAlpha(line->color, alpha));

				pos.y -= (pad.y + 2.f*font_size);
		}

		EndScissorMode();

		// @SPEED
		char actual_prefix[1024] = {0};
		
		snprintf(actual_prefix, 1024, "%s%s%c", console->prefix ? console->prefix : "", console->prefix2 ? console->prefix2 : "", console->prefix_symbol);

		draw_text(console->font, actual_prefix, v2(rect.x + 4.f, rect.y + rect.height), font_size, ColorAlpha(WHITE, alpha));
		float prefix_offset = MeasureTextEx(console->font, actual_prefix, font_size, 2.5f).x + 10.f;
		draw_text(console->font, get_current_console_line_buff(console), v2(rect.x + prefix_offset, rect.y + rect.height), font_size, ColorAlpha(WHITE, alpha));

		// Rectangle cursor_rec = {
		//		 .x = rect.x + get_cursor_offset(console, font_size),
		//		 .y = rect.y + rect.height,
		//		 .width = font_size,
		//		 .height = font_size,
		// };
		// DrawRectangleRec(cursor_rec, WHITE);

		// log_debug("console->cursor: %d", console->cursor);
}

void console_prompt(Console *console, const char *prompt, String_array *expected_prompt_values) {
		console->prompting = true;
		add_line_to_console_simple(console, (char *)prompt, GOLD, false);
		console->prompt_line_id = console->lines.count-1;
		console->expecting_values = expected_prompt_values != NULL;
		if (expected_prompt_values != NULL) {
				console->expected_prompt_values.count = 0;
				for (int i = 0; i < expected_prompt_values->count; ++i) {
						darr_append(console->expected_prompt_values, expected_prompt_values->items[i]);
				}
		}
}

// Timer and Alarm
void update_timer(Timer *t, float dt) {
		t->time += dt;
}

bool on_alarm(Alarm *a, float dt) {
		update_timer(&a->timer, dt);

		if (a->timer.time >= a->alarm_time) {
            a->timer.time = 0;
            if (a->once) {
                if (!a->done) {
                    a->done = true;
                    return true;
                }
            } else {
                return true;
            }
		}
		return false;
}

#endif // ENGINE_IMPLEMENTATION
