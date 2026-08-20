#include "core/word_wrap.hpp"

#include <algorithm>
#include <utility>

namespace {

enum class TokenKind { Word, Space, Newline };

struct Fragment {
    std::string text;
    TextStyle style;
};

struct Token {
    TokenKind kind = TokenKind::Word;
    std::vector<Fragment> fragments;
    float width = 0.f;
};

[[nodiscard]] std::vector<Token> tokenize(const std::vector<TextSpan>& spans,
                                          const TextMeasurer& measurer) {
    std::vector<Token> tokens;

    Token pending;
    bool hasPending = false;
    std::string fragmentText;
    TextStyle fragmentStyle;

    auto flushFragment = [&] {
        if (!fragmentText.empty()) {
            pending.fragments.push_back({std::move(fragmentText), fragmentStyle});
            fragmentText.clear();
        }
    };

    auto flushToken = [&] {
        flushFragment();

        if (hasPending) {
            for (const Fragment& fragment : pending.fragments)
                pending.width += measurer.measure(fragment.text, fragment.style);

            tokens.push_back(std::move(pending));
            pending = Token{};
            hasPending = false;
        }
    };

    for (const TextSpan& span : spans) {
        for (const char c : span.text) {
            if (c == '\r')
                continue;

            if (c == '\n') {
                flushToken();
                tokens.push_back(Token{TokenKind::Newline, {}, 0.f});
                continue;
            }

            const TokenKind kind =
                (c == ' ' || c == '\t') ? TokenKind::Space : TokenKind::Word;

            if (!hasPending || pending.kind != kind) {
                flushToken();
                pending.kind = kind;
                hasPending = true;
            }

            if (!fragmentText.empty() && fragmentStyle != span.style)
                flushFragment();
            if (fragmentText.empty())
                fragmentStyle = span.style;

            fragmentText.push_back(c);
        }
    }

    flushToken();

    return tokens;
}

[[nodiscard]] WrappedLine buildLine(const std::vector<const Token*>& lineTokens) {
    WrappedLine line;

    for (const Token* token : lineTokens)
        for (const Fragment& fragment : token->fragments) {
            if (!line.empty() && line.back().style == fragment.style)
                line.back().text += fragment.text;
            else
                line.push_back({fragment.text, fragment.style});
        }

    return line;
}

} // namespace

std::vector<WrappedLine> wordWrap(const std::vector<TextSpan>& spans,
                                  const TextMeasurer& measurer, float maxWidth) {
    const float limit = std::max(maxWidth, 0.f);
    const std::vector<Token> tokens = tokenize(spans, measurer);

    std::vector<WrappedLine> lines;
    std::vector<const Token*> lineTokens;
    float lineWidth = 0.f;
    const Token* pendingGap = nullptr;

    auto emitLine = [&] {
        lines.push_back(buildLine(lineTokens));
        lineTokens.clear();
        lineWidth = 0.f;
        pendingGap = nullptr;
    };

    for (const Token& token : tokens) {
        switch (token.kind) {
            case TokenKind::Newline:
                emitLine();
                break;

            case TokenKind::Space:
                pendingGap = &token;
                break;

            case TokenKind::Word: {
                const float gap = pendingGap ? pendingGap->width : 0.f;
                const bool lineEmpty = lineTokens.empty();

                if (!lineEmpty && lineWidth + gap + token.width > limit) {
                    emitLine();
                    lineTokens.push_back(&token);
                    lineWidth = token.width;
                } else {
                    if (!lineEmpty && pendingGap) {
                        lineTokens.push_back(pendingGap);
                        lineWidth += gap;
                    }

                    lineTokens.push_back(&token);
                    lineWidth += token.width;
                }

                pendingGap = nullptr;
                break;
            }
        }
    }

    if (!lineTokens.empty())
        emitLine();

    return lines;
}