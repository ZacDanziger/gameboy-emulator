#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include "types.h"

inline bool is_set(const Byte byte, const Bit bit) { return (byte & static_cast<Byte>(bit)) > 0; }
inline bool is_set(const Word word, const Bit bit) { return (word & static_cast<Word>(bit)) > 0; }
inline void set_bit(Byte& byte, const Bit bit) { byte |= static_cast<Byte>(bit); }
inline void reset_bit(Byte& byte, const Bit bit) { byte &= ~static_cast<Byte>(bit); }

#endif  // BIT_UTILS_H