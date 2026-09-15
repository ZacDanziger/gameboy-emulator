#ifndef APP_EVENT_H
#define APP_EVENT_H

#include <variant>
#include <string>

#include "input.h"

struct QuitRequested {};
struct TogglePause {};
struct RomDialogOpened{};
struct RomSelected {
    std::string rom_file;
};
struct RomDialogCanceled {};

using AppEvent = std::variant<
    QuitRequested,
    TogglePause,
    RomDialogOpened,
    RomSelected,
    RomDialogCanceled,
    InputEvent
>;

#endif  // APP_EVENT_H