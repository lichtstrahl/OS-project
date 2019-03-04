//
// Created by igor on 04.03.19.
//

#ifndef C_CLIENT_ACTION_H
#define C_CLIENT_ACTION_H

#define ATYPE_LEFT      "LEFT"
#define ATYPE_RIGHT     "RIGHT"
#define ATYPE_MIDDLE    "MIDDLE"
#define ATYPE_UP        "WHEELUP"
#define ATYPE_DOWN      "WHEELDOWN"
#define ATYPE_SIDE_1    "SIDE_1"
#define ATYPE_SIDE_2    "SIDE_2"

enum ActionType {
    CLICK_LEFT,
    CLICK_RIGHT,
    CLICK_MIDDLE,
    SCROLL_UP,
    SCROLL_DOWN,
    SIDE_1,
    SIDE_2,
    NONE
};

struct Action {
    int id;
    ActionType type;
    const char* name;
};
typedef struct Action Action;
#define PRINT_ACTION(a) printf("{%d, \"%s\"}\n", a.id, a.name)

#endif //C_CLIENT_ACTION_H
