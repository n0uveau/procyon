#pragma once
#include <vector>
#include "core/text_span.hpp"
#include "core/text_measurer.hpp"

using WrappedLine = std::vector<TextSpan>;

[[nodiscard]] std::vector<WrappedLine> wordWrap(const std::vector<TextSpan>& spans,
                                                const TextMeasurer& measurer,
                                                float maxWidth);