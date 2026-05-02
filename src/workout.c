#include <pebble.h>
#include "workout.h"
#include "workout_data.h"
#include "progress.h"

typedef enum {
    WORKOUT_STATE_RUNNING,
    WORKOUT_STATE_PAUSED,
    WORKOUT_STATE_COMPLETE
} WorkoutState;

static Window *s_window;
static TextLayer *s_activity_layer;
static TextLayer *s_timer_layer;
static TextLayer *s_progress_layer;
static TextLayer *s_elapsed_layer;
static TextLayer *s_paused_layer;

static uint8_t s_week;
static uint8_t s_day;
static const Workout *s_workout;
static uint8_t s_current_interval;
static uint16_t s_interval_remaining;
static uint32_t s_total_elapsed;
static WorkoutState s_state;
static AppTimer *s_pop_timer;

// Text buffers
static char s_activity_buf[12];
static char s_timer_buf[8];
static char s_progress_buf[16];
static char s_elapsed_buf[20];

static const char *activity_name(ActivityType type) {
    switch (type) {
        case ACTIVITY_WARMUP:   return "WARMUP";
        case ACTIVITY_RUN:      return "RUN";
        case ACTIVITY_WALK:     return "WALK";
        case ACTIVITY_COOLDOWN: return "COOLDOWN";
        default:                return "";
    }
}

static int count_run_walk_intervals(void) {
    int count = 0;
    for (int i = 0; i < s_workout->num_intervals; i++) {
        ActivityType a = s_workout->intervals[i].activity;
        if (a == ACTIVITY_RUN || a == ACTIVITY_WALK) count++;
    }
    return count;
}

static int current_run_walk_index(void) {
    int index = 0;
    for (int i = 0; i <= s_current_interval; i++) {
        ActivityType a = s_workout->intervals[i].activity;
        if (a == ACTIVITY_RUN || a == ACTIVITY_WALK) index++;
    }
    return index;
}

static void update_display(void) {
    const Interval *interval = &s_workout->intervals[s_current_interval];

    // Activity name
    snprintf(s_activity_buf, sizeof(s_activity_buf), "%s", activity_name(interval->activity));
    text_layer_set_text(s_activity_layer, s_activity_buf);

    // Countdown timer
    int mins = s_interval_remaining / 60;
    int secs = s_interval_remaining % 60;
    snprintf(s_timer_buf, sizeof(s_timer_buf), "%d:%02d", mins, secs);
    text_layer_set_text(s_timer_layer, s_timer_buf);

    // Interval progress (only count run/walk intervals)
    ActivityType a = interval->activity;
    if (a == ACTIVITY_RUN || a == ACTIVITY_WALK) {
        int total = count_run_walk_intervals();
        int current = current_run_walk_index();
        snprintf(s_progress_buf, sizeof(s_progress_buf), "%d / %d", current, total);
    } else {
        snprintf(s_progress_buf, sizeof(s_progress_buf), "--");
    }
    text_layer_set_text(s_progress_layer, s_progress_buf);

    // Total elapsed
    int e_mins = s_total_elapsed / 60;
    int e_secs = s_total_elapsed % 60;
    snprintf(s_elapsed_buf, sizeof(s_elapsed_buf), "Total: %d:%02d", e_mins, e_secs);
    text_layer_set_text(s_elapsed_layer, s_elapsed_buf);

    // Paused indicator
    layer_set_hidden(text_layer_get_layer(s_paused_layer), s_state != WORKOUT_STATE_PAUSED);
}

static void show_completion(void) {
    text_layer_set_text(s_activity_layer, "DONE!");
    text_layer_set_text(s_timer_layer, "");
    text_layer_set_text(s_progress_layer, "");
    text_layer_set_text(s_elapsed_layer, "");
    layer_set_hidden(text_layer_get_layer(s_paused_layer), true);
}

static void pop_timer_callback(void *data) {
    s_pop_timer = NULL;
    window_stack_pop(true);
}

static void advance_interval(void) {
    s_current_interval++;

    if (s_current_interval >= s_workout->num_intervals) {
        s_state = WORKOUT_STATE_COMPLETE;
        tick_timer_service_unsubscribe();
        vibes_long_pulse();
        progress_mark_complete(s_week, s_day);
        show_completion();
        s_pop_timer = app_timer_register(2000, pop_timer_callback, NULL);
        return;
    }

    s_interval_remaining = s_workout->intervals[s_current_interval].duration_sec;

    ActivityType next = s_workout->intervals[s_current_interval].activity;
    if (next == ACTIVITY_RUN) {
        vibes_double_pulse();
    } else {
        vibes_short_pulse();
    }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    if (s_state != WORKOUT_STATE_RUNNING) return;

    s_total_elapsed++;
    s_interval_remaining--;

    if (s_interval_remaining == 0) {
        advance_interval();
        if (s_state == WORKOUT_STATE_COMPLETE) return;
    }

    update_display();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (s_state == WORKOUT_STATE_RUNNING) {
        s_state = WORKOUT_STATE_PAUSED;
        tick_timer_service_unsubscribe();
        update_display();
    } else if (s_state == WORKOUT_STATE_PAUSED) {
        s_state = WORKOUT_STATE_RUNNING;
        tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
        update_display();
    }
}

static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (s_state == WORKOUT_STATE_RUNNING) {
        tick_timer_service_unsubscribe();
    }
    if (s_pop_timer) {
        app_timer_cancel(s_pop_timer);
        s_pop_timer = NULL;
    }
    window_stack_pop(true);
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

static TextLayer *create_text_layer(Layer *parent, GRect frame, GFont font, GTextAlignment align) {
    TextLayer *layer = text_layer_create(frame);
    text_layer_set_background_color(layer, GColorBlack);
    text_layer_set_text_color(layer, GColorWhite);
    text_layer_set_font(layer, font);
    text_layer_set_text_alignment(layer, align);
    layer_add_child(parent, text_layer_get_layer(layer));
    return layer;
}

static void window_load(Window *window) {
    Layer *root = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(root);
    window_set_background_color(window, GColorBlack);

    // Activity label (top)
    s_activity_layer = create_text_layer(root,
        GRect(0, 5, bounds.size.w, 38),
        fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD),
        GTextAlignmentCenter);

    // Countdown timer (center, large)
    s_timer_layer = create_text_layer(root,
        GRect(0, 45, bounds.size.w, 50),
        fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD),
        GTextAlignmentCenter);

    // Interval progress
    s_progress_layer = create_text_layer(root,
        GRect(0, 100, bounds.size.w, 30),
        fonts_get_system_font(FONT_KEY_GOTHIC_24),
        GTextAlignmentCenter);

    // Total elapsed
    s_elapsed_layer = create_text_layer(root,
        GRect(0, 135, bounds.size.w, 28),
        fonts_get_system_font(FONT_KEY_GOTHIC_18),
        GTextAlignmentCenter);

    // Paused indicator (overlays center area)
    s_paused_layer = create_text_layer(root,
        GRect(0, 100, bounds.size.w, 30),
        fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
        GTextAlignmentCenter);
    text_layer_set_text(s_paused_layer, "PAUSED");
    layer_set_hidden(text_layer_get_layer(s_paused_layer), true);

    // Initialize workout state
    s_workout = &all_workouts[s_week][s_day];
    s_current_interval = 0;
    s_interval_remaining = s_workout->intervals[0].duration_sec;
    s_total_elapsed = 0;
    s_state = WORKOUT_STATE_RUNNING;
    s_pop_timer = NULL;

    window_set_click_config_provider(window, click_config_provider);
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    update_display();
}

static void window_unload(Window *window) {
    tick_timer_service_unsubscribe();
    if (s_pop_timer) {
        app_timer_cancel(s_pop_timer);
        s_pop_timer = NULL;
    }
    text_layer_destroy(s_activity_layer);
    text_layer_destroy(s_timer_layer);
    text_layer_destroy(s_progress_layer);
    text_layer_destroy(s_elapsed_layer);
    text_layer_destroy(s_paused_layer);
    window_destroy(s_window);
    s_window = NULL;
}

void workout_window_push(uint8_t week, uint8_t day) {
    s_week = week;
    s_day = day;

    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers){
        .load = window_load,
        .unload = window_unload,
    });
    window_stack_push(s_window, true);
}
