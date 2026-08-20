#pragma once
#include <string>
#include "core/text_style.hpp"

struct TextSpan {
    std::string text;
    TextStyle style{};

    friend bool operator==(const TextSpan&, const TextSpan&) = default;
};