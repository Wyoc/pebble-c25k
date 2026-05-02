#pragma once

#include <pebble.h>

bool progress_is_complete(uint8_t week, uint8_t day);
void progress_mark_complete(uint8_t week, uint8_t day);
int progress_days_complete(uint8_t week);
void progress_reset_all(void);
