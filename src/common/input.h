#ifndef INPUT_H
#define INPUT_H

enum class Button {
    A, B, Select, Start,    // button keys
    Right, Left, Up, Down   // direction keys
};

struct InputEvent {
    Button button;
    bool pressed;
};

#endif  // INPUT_H