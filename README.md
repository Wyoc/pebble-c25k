# C25K for Pebble 2

A Couch to 5K running app for the Pebble 2 smartwatch. Guides you through the complete 9-week C25K program with timed walk/run intervals and vibration alerts.

## Features

- Full 9-week C25K program (3 days per week, 27 workouts)
- Countdown timer with walk/run interval tracking
- Vibration alerts on interval transitions (double pulse for run, short pulse for walk)
- Pause/resume during workouts
- Persistent progress tracking with completion checkmarks
- Minimal RAM footprint (~5KB)

## Workout Screen

- **Top**: Current activity (WARMUP / RUN / WALK / COOLDOWN)
- **Center**: Countdown timer for current interval
- **Below**: Interval progress (e.g., 3 / 8)
- **Bottom**: Total elapsed time

## Controls

| Button | Menu | Workout |
|--------|------|---------|
| UP/DOWN | Navigate | - |
| SELECT | Open | Pause/Resume |
| BACK | Go back | Abort workout |

## Building

Requires the [Rebble SDK](https://developer.rebble.io/sdk/) (Python 3.10-3.13).

```bash
pebble build
```

## Running

```bash
# Emulator
pebble install --emulator diorite

# Real watch (via Rebble phone app)
pebble install --phone <phone-ip>
```

## C25K Program

All workouts include a 5-minute warmup walk and 5-minute cooldown walk.

| Week | Workout |
|------|---------|
| 1 | [60s run, 90s walk] x8 |
| 2 | [90s run, 2min walk] x6 |
| 3 | [90s run, 90s walk, 3min run, 3min walk] x2 |
| 4 | 3min run, 90s walk, 5min run, 2.5min walk, 3min run, 90s walk, 5min run |
| 5 | Varies by day: intervals to 20min continuous run |
| 6 | Varies by day: intervals to 25min continuous run |
| 7 | 25min run |
| 8 | 28min run |
| 9 | 30min run |

## License

MIT
