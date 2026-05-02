#pragma once

#include <pebble.h>

typedef enum {
    ACTIVITY_WARMUP,
    ACTIVITY_RUN,
    ACTIVITY_WALK,
    ACTIVITY_COOLDOWN
} ActivityType;

typedef struct {
    ActivityType activity;
    uint16_t duration_sec;
} Interval;

typedef struct {
    const Interval *intervals;
    uint8_t num_intervals;
} Workout;

// Helper macro
#define WORKOUT(arr) { (arr), ARRAY_LENGTH(arr) }

// Week 1: [60s run, 90s walk] x8
static const Interval s_week1[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN,  60}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN,  60}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN,  60}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN,  60}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN,  60}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN,  60}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN,  60}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN,  60}, {ACTIVITY_WALK,  90},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 2: [90s run, 120s walk] x6
static const Interval s_week2[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN,  90}, {ACTIVITY_WALK, 120},
    {ACTIVITY_RUN,  90}, {ACTIVITY_WALK, 120},
    {ACTIVITY_RUN,  90}, {ACTIVITY_WALK, 120},
    {ACTIVITY_RUN,  90}, {ACTIVITY_WALK, 120},
    {ACTIVITY_RUN,  90}, {ACTIVITY_WALK, 120},
    {ACTIVITY_RUN,  90}, {ACTIVITY_WALK, 120},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 3: [90s run, 90s walk, 3min run, 3min walk] x2
static const Interval s_week3[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN,  90}, {ACTIVITY_WALK,  90}, {ACTIVITY_RUN, 180}, {ACTIVITY_WALK, 180},
    {ACTIVITY_RUN,  90}, {ACTIVITY_WALK,  90}, {ACTIVITY_RUN, 180}, {ACTIVITY_WALK, 180},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 4: 3min run, 90s walk, 5min run, 2.5min walk, 3min run, 90s walk, 5min run
static const Interval s_week4[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 180}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN, 300}, {ACTIVITY_WALK, 150},
    {ACTIVITY_RUN, 180}, {ACTIVITY_WALK,  90},
    {ACTIVITY_RUN, 300},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 5 Day 1: 5min run, 3min walk, 5min run, 3min walk, 5min run
static const Interval s_week5_day1[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 300}, {ACTIVITY_WALK, 180},
    {ACTIVITY_RUN, 300}, {ACTIVITY_WALK, 180},
    {ACTIVITY_RUN, 300},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 5 Day 2: 8min run, 5min walk, 8min run
static const Interval s_week5_day2[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 480}, {ACTIVITY_WALK, 300},
    {ACTIVITY_RUN, 480},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 5 Day 3: 20min run
static const Interval s_week5_day3[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 1200},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 6 Day 1: 5min run, 3min walk, 8min run, 3min walk, 5min run
static const Interval s_week6_day1[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 300}, {ACTIVITY_WALK, 180},
    {ACTIVITY_RUN, 480}, {ACTIVITY_WALK, 180},
    {ACTIVITY_RUN, 300},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 6 Day 2: 10min run, 3min walk, 10min run
static const Interval s_week6_day2[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 600}, {ACTIVITY_WALK, 180},
    {ACTIVITY_RUN, 600},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 6 Day 3: 25min run
static const Interval s_week6_day3[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 1500},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 7: 25min run
static const Interval s_week7[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 1500},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 8: 28min run
static const Interval s_week8[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 1680},
    {ACTIVITY_COOLDOWN, 300}
};

// Week 9: 30min run
static const Interval s_week9[] = {
    {ACTIVITY_WARMUP, 300},
    {ACTIVITY_RUN, 1800},
    {ACTIVITY_COOLDOWN, 300}
};

// Master lookup table: all_workouts[week][day], 0-indexed
static const Workout all_workouts[9][3] = {
    // Week 1
    { WORKOUT(s_week1), WORKOUT(s_week1), WORKOUT(s_week1) },
    // Week 2
    { WORKOUT(s_week2), WORKOUT(s_week2), WORKOUT(s_week2) },
    // Week 3
    { WORKOUT(s_week3), WORKOUT(s_week3), WORKOUT(s_week3) },
    // Week 4
    { WORKOUT(s_week4), WORKOUT(s_week4), WORKOUT(s_week4) },
    // Week 5
    { WORKOUT(s_week5_day1), WORKOUT(s_week5_day2), WORKOUT(s_week5_day3) },
    // Week 6
    { WORKOUT(s_week6_day1), WORKOUT(s_week6_day2), WORKOUT(s_week6_day3) },
    // Week 7
    { WORKOUT(s_week7), WORKOUT(s_week7), WORKOUT(s_week7) },
    // Week 8
    { WORKOUT(s_week8), WORKOUT(s_week8), WORKOUT(s_week8) },
    // Week 9
    { WORKOUT(s_week9), WORKOUT(s_week9), WORKOUT(s_week9) },
};
