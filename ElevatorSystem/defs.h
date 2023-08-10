#ifndef DEFS_H
#define DEFS_H

#define MAX_ARR  256
#define MAX_CAPACITY 500    // 500 lbs5
#define MOVE_DURATION 3000          // 3 seconds
#define DOOR_DURATION 5000          // 5 seconds
#define REOPEN_DOOR_INTERVAL 2000   // 2 seconds *Note: this must be shorter than DOOR_DURATION*
#define CLOSE_DURATION 3000         // 3 seconds

typedef enum {
    AI,
    ELEVATOR_CENTERED,
    DESTINATION_KNOWN
} Strategy;

typedef enum {
    DEFAULT,
    HELP,
    DOOR_OBSTACLE,
    FIRE,
    OVERLOAD,
    POWER_OUT,
    DEAD,
    RESET
} State;

typedef enum { UP, DOWN, BOTH, NONE } Direction;

#endif // DEFS_H
