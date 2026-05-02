#include <pebble.h>
#include "workout_data.h"
#include "workout.h"
#include "progress.h"

// Week menu
static Window *s_week_window;
static MenuLayer *s_week_menu;

// Day menu
static Window *s_day_window;
static MenuLayer *s_day_menu;
static uint8_t s_selected_week;

// --- Day Menu ---

static uint16_t day_menu_get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return 3;
}

static void day_menu_draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    uint8_t day = cell_index->row;
    bool done = progress_is_complete(s_selected_week, day);

    char title[24];
    snprintf(title, sizeof(title), "%s Day %d", done ? "\xe2\x9c\x93" : " ", day + 1);

    // Count run/walk intervals (exclude warmup and cooldown)
    const Workout *w = &all_workouts[s_selected_week][day];
    int run_intervals = 0;
    for (int i = 0; i < w->num_intervals; i++) {
        if (w->intervals[i].activity == ACTIVITY_RUN) run_intervals++;
    }

    char subtitle[32];
    if (run_intervals == 1) {
        int mins = 0;
        for (int i = 0; i < w->num_intervals; i++) {
            if (w->intervals[i].activity == ACTIVITY_RUN) {
                mins = w->intervals[i].duration_sec / 60;
            }
        }
        snprintf(subtitle, sizeof(subtitle), "%d min run", mins);
    } else {
        snprintf(subtitle, sizeof(subtitle), "%d run/walk intervals", run_intervals);
    }

    menu_cell_basic_draw(ctx, cell_layer, title, subtitle, NULL);
}

static void day_menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    workout_window_push(s_selected_week, cell_index->row);
}

static void day_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_day_menu = menu_layer_create(bounds);
    menu_layer_set_callbacks(s_day_menu, NULL, (MenuLayerCallbacks){
        .get_num_rows = day_menu_get_num_rows,
        .draw_row = day_menu_draw_row,
        .select_click = day_menu_select_click,
    });
    menu_layer_set_click_config_onto_window(s_day_menu, window);
    layer_add_child(window_layer, menu_layer_get_layer(s_day_menu));
}

static void day_window_unload(Window *window) {
    menu_layer_destroy(s_day_menu);
}

static void day_window_appear(Window *window) {
    // Refresh checkmarks when returning from workout
    menu_layer_reload_data(s_day_menu);
}

// --- Week Menu ---

static uint16_t week_menu_get_num_rows(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    return 9;
}

static void week_menu_draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    uint8_t week = cell_index->row;
    int done = progress_days_complete(week);

    char title[16];
    snprintf(title, sizeof(title), "Week %d", week + 1);

    char subtitle[16];
    snprintf(subtitle, sizeof(subtitle), "%d/3 done", done);

    menu_cell_basic_draw(ctx, cell_layer, title, subtitle, NULL);
}

static void week_menu_select_click(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    s_selected_week = cell_index->row;
    window_stack_push(s_day_window, true);
}

static void week_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_week_menu = menu_layer_create(bounds);
    menu_layer_set_callbacks(s_week_menu, NULL, (MenuLayerCallbacks){
        .get_num_rows = week_menu_get_num_rows,
        .draw_row = week_menu_draw_row,
        .select_click = week_menu_select_click,
    });
    menu_layer_set_click_config_onto_window(s_week_menu, window);
    layer_add_child(window_layer, menu_layer_get_layer(s_week_menu));
}

static void week_window_unload(Window *window) {
    menu_layer_destroy(s_week_menu);
}

static void week_window_appear(Window *window) {
    // Refresh progress counts when returning from day menu
    menu_layer_reload_data(s_week_menu);
}

// --- App lifecycle ---

static void init(void) {
    s_week_window = window_create();
    window_set_window_handlers(s_week_window, (WindowHandlers){
        .load = week_window_load,
        .unload = week_window_unload,
        .appear = week_window_appear,
    });

    s_day_window = window_create();
    window_set_window_handlers(s_day_window, (WindowHandlers){
        .load = day_window_load,
        .unload = day_window_unload,
        .appear = day_window_appear,
    });

    window_stack_push(s_week_window, true);
}

static void deinit(void) {
    window_destroy(s_day_window);
    window_destroy(s_week_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
