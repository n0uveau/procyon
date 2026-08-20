#include "core/markup.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <format>
#include <optional>
#include <utility>

namespace {

struct OpenColor {
    Color color;
    std::size_t position;
};

[[nodiscard]] std::optional<std::uint8_t> hexDigit(char c) {
    if (c >= '0' && c <= '9')
        return static_cast<std::uint8_t>(c - '0');
    if (c >= 'a' && c <= 'f')
        return static_cast<std::uint8_t>(c - 'a' + 10);
    if (c >= 'A' && c <= 'F')
        return static_cast<std::uint8_t>(c - 'A' + 10);

    return std::nullopt;
}

[[nodiscard]] std::optional<Color> parseHex6(std::string_view s) {
    if (s.size() != 6)
        return std::nullopt;

    std::array<std::uint8_t, 6> n{};
    for (std::size_t i = 0; i < s.size(); ++i) {
        const std::optional<std::uint8_t> d = hexDigit(s[i]);

        if (!d)
            return std::nullopt;

        n[i] = *d;
    }

    return Color{static_cast<std::uint8_t>(n[0] << 4 | n[1]),
                 static_cast<std::uint8_t>(n[2] << 4 | n[3]),
                 static_cast<std::uint8_t>(n[4] << 4 | n[5])};
}

} // namespace

std::expected<std::vector<TextSpan>, MarkupError> parseMarkup(std::string_view markup) {
    std::vector<TextSpan> spans;
    std::string run;
    bool bold = false;
    bool italic = false;
    std::size_t boldOpen = 0;
    std::size_t italicOpen = 0;
    std::vector<OpenColor> colors;

    auto style = [&] {
        return TextStyle{bold, italic, colors.empty() ? Color{} : colors.back().color};
    };

    auto flush = [&] {
        if (!run.empty()) {
            spans.push_back({std::move(run), style()});
            run.clear();
        }
    };

    std::size_t pos = 0;
    while (pos < markup.size()) {
        const std::string_view rest = markup.substr(pos);

        if (rest.starts_with("**")) {
            flush();

            bold = !bold;
            if (bold)
                boldOpen = pos;

            pos += 2;
        } else if (rest.starts_with("[color=")) {
            const std::size_t close = markup.find(']', pos);

            if (close == std::string_view::npos)
                return std::unexpected(MarkupError{MarkupErrorKind::UnclosedColor,
                                                   "unclosed [color=...] tag", pos});

            const std::string_view hex = markup.substr(pos + 7, close - (pos + 7));
            const std::optional<Color> color = parseHex6(hex);

            if (!color)
                return std::unexpected(MarkupError{
                    MarkupErrorKind::BadHex,
                    std::format("expected 6 hex digits in color tag, got \"{}\"", hex),
                    pos});

            flush();
            colors.push_back({*color, pos});
            pos = close + 1;
        } else if (rest.starts_with("[/color]")) {
            if (colors.empty())
                return std::unexpected(MarkupError{MarkupErrorKind::UnmatchedColorClose,
                                                   "[/color] with no open color tag",
                                                   pos});

            flush();
            colors.pop_back();
            pos += 8;
        } else if (rest.front() == '*') {
            flush();

            italic = !italic;
            if (italic)
                italicOpen = pos;

            pos += 1;
        } else if (rest.front() == '[') {
            const std::size_t close = markup.find(']', pos);
            const std::string_view tag =
                close == std::string_view::npos
                    ? rest.substr(0, std::min<std::size_t>(rest.size(), 12))
                    : markup.substr(pos, close - pos + 1);

            return std::unexpected(MarkupError{MarkupErrorKind::UnknownTag,
                                               std::format("unknown tag \"{}\"", tag),
                                               pos});
        } else {
            run.push_back(rest.front());
            pos += 1;
        }
    }

    if (bold)
        return std::unexpected(
            MarkupError{MarkupErrorKind::UnclosedBold, "unclosed ** (bold)", boldOpen});
    if (italic)
        return std::unexpected(MarkupError{MarkupErrorKind::UnclosedItalic,
                                           "unclosed * (italic)", italicOpen});
    if (!colors.empty())
        return std::unexpected(MarkupError{MarkupErrorKind::UnclosedColor,
                                           "unclosed [color=...] tag",
                                           colors.back().position});

    flush();

    return spans;
}