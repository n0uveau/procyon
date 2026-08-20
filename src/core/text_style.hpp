#pragma once
#include <cstdint>

struct Color {
    std::uint8_t r = 255;
    std::uint8_t g = 255;
    std::uint8_t b = 255;
    std::uint8_t a = 255;

    friend bool operator==(const Color&, const Color&) = default;
};

struct TextStyle {
    bool bold = false;
    bool italic = false;
    Color color{};

    friend bool operator==(const TextStyle&, const TextStyle&) = default;
};