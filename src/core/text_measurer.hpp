#pragma once
#include <string_view>
#include "core/text_style.hpp"

class TextMeasurer {
public:
    virtual ~TextMeasurer() = default;

    [[nodiscard]] virtual float measure(std::string_view text,
                                        const TextStyle& style) const = 0;
};