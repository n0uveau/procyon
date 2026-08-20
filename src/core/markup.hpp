#pragma once
#include <cstddef>
#include <expected>
#include <string>
#include <string_view>
#include <vector>
#include "core/text_span.hpp"

enum class MarkupErrorKind {
    BadHex,
    UnknownTag,
    UnmatchedColorClose,
    UnclosedBold,
    UnclosedItalic,
    UnclosedColor,
};

struct MarkupError {
    MarkupErrorKind kind;
    std::string message;
    std::size_t position = 0;
};

[[nodiscard]] std::expected<std::vector<TextSpan>, MarkupError>
parseMarkup(std::string_view markup);