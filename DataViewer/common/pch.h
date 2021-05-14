#ifndef PCH_H
#define PCH_H

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

enum class MOVE_DIRECTION {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    LEFT_WORD,
    RIGHT_WORD,
    LEFT_LINE,
    RIGHT_LINE,
    UP_PAGE,
    DOWN_PAGE,
    DOC_BEGIN,
    DOC_END,
};

#define uptr std::unique_ptr
#define sptr std::shared_ptr
#define mk_u std::make_unique
#define mk_s std::make_shared

#endif  // PCH_H
