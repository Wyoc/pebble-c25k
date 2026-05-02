#include "progress.h"

#define PERSIST_KEY_WEEK_BASE 0x100

static uint32_t prv_key(uint8_t week) {
    return PERSIST_KEY_WEEK_BASE + week;
}

bool progress_is_complete(uint8_t week, uint8_t day) {
    if (!persist_exists(prv_key(week))) return false;
    int32_t val = persist_read_int(prv_key(week));
    return (val >> day) & 1;
}

void progress_mark_complete(uint8_t week, uint8_t day) {
    int32_t val = 0;
    if (persist_exists(prv_key(week))) {
        val = persist_read_int(prv_key(week));
    }
    val |= (1 << day);
    persist_write_int(prv_key(week), val);
}

int progress_days_complete(uint8_t week) {
    if (!persist_exists(prv_key(week))) return 0;
    int32_t val = persist_read_int(prv_key(week));
    int count = 0;
    for (int i = 0; i < 3; i++) {
        if ((val >> i) & 1) count++;
    }
    return count;
}

void progress_reset_all(void) {
    for (uint8_t w = 0; w < 9; w++) {
        persist_delete(prv_key(w));
    }
}
