#include <gtest/gtest.h>
#include "../tui/char_styling.h"
#include "../tui/tui_base.h"
#include "../result.h"

using namespace dungeons;
using namespace dungeons::tui;

class CharStyleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Тестовые стили
    }
};

// ============================================================================
// Тесты конструкторов CharStyle
// ============================================================================

TEST_F(CharStyleTest, DefaultConstructor) {
    CharStyle style;
    EXPECT_EQ(style.fg_color, Color::BLACK);
    EXPECT_EQ(style.bg_color, Color::WHITE);
    EXPECT_EQ(style.style, StyleFlags::NONE);
    EXPECT_EQ(style.fg_ansi, 0);
    EXPECT_EQ(style.bg_ansi, 0);
    EXPECT_EQ(style.fg_r, 0);
    EXPECT_EQ(style.fg_g, 0);
    EXPECT_EQ(style.fg_b, 0);
    EXPECT_EQ(style.bg_r, 0);
    EXPECT_EQ(style.bg_g, 0);
    EXPECT_EQ(style.bg_b, 0);
}

TEST_F(CharStyleTest, ConstructorWithColors) {
    CharStyle style(Color::RED, Color::BLUE);
    EXPECT_EQ(style.fg_color, Color::RED);
    EXPECT_EQ(style.bg_color, Color::BLUE);
    EXPECT_EQ(style.style, StyleFlags::NONE);
}

TEST_F(CharStyleTest, ConstructorWithColorsAndStyle) {
    CharStyle style(Color::GREEN, Color::YELLOW, StyleFlags::BOLD);
    EXPECT_EQ(style.fg_color, Color::GREEN);
    EXPECT_EQ(style.bg_color, Color::YELLOW);
    EXPECT_EQ(style.style, StyleFlags::BOLD);
}

TEST_F(CharStyleTest, ConstructorWithANSI8) {
    CharStyle style(123, 234, StyleFlags::ITALIC);
    EXPECT_EQ(style.fg_color, Color::ANSI8);
    EXPECT_EQ(style.bg_color, Color::ANSI8);
    EXPECT_EQ(style.fg_ansi, 123);
    EXPECT_EQ(style.bg_ansi, 234);
    EXPECT_EQ(style.style, StyleFlags::ITALIC);
}

TEST_F(CharStyleTest, ConstructorWithRGB) {
    CharStyle style(255, 128, 0, 0, 64, 128, StyleFlags::UNDERLINED);
    EXPECT_EQ(style.fg_color, Color::RGB);
    EXPECT_EQ(style.bg_color, Color::RGB);
    EXPECT_EQ(style.fg_r, 255);
    EXPECT_EQ(style.fg_g, 128);
    EXPECT_EQ(style.fg_b, 0);
    EXPECT_EQ(style.bg_r, 0);
    EXPECT_EQ(style.bg_g, 64);
    EXPECT_EQ(style.bg_b, 128);
    EXPECT_EQ(style.style, StyleFlags::UNDERLINED);
}

TEST_F(CharStyleTest, FullConstructor) {
    CharStyle style(Color::RED, 100, 200, 150, 50,
        Color::BLUE, 10, 20, 30, 40,
        StyleFlags::BOLD);
    EXPECT_EQ(style.fg_color, Color::RED);
    EXPECT_EQ(style.bg_color, Color::BLUE);
    EXPECT_EQ(style.fg_ansi, 100);
    EXPECT_EQ(style.bg_ansi, 10);
    EXPECT_EQ(style.fg_r, 200);
    EXPECT_EQ(style.fg_g, 150);
    EXPECT_EQ(style.fg_b, 50);
    EXPECT_EQ(style.bg_r, 20);
    EXPECT_EQ(style.bg_g, 30);
    EXPECT_EQ(style.bg_b, 40);
    EXPECT_EQ(style.style, StyleFlags::BOLD);
}

// ============================================================================
// Тесты copy constructor
// ============================================================================

TEST_F(CharStyleTest, CopyConstructor) {
    CharStyle original(Color::RED, Color::BLUE, StyleFlags::ITALIC);
    CharStyle copy(original);

    EXPECT_EQ(copy.fg_color, original.fg_color);
    EXPECT_EQ(copy.bg_color, original.bg_color);
    EXPECT_EQ(copy.style, original.style);
    EXPECT_EQ(copy, original);
}

TEST_F(CharStyleTest, CopyConstructor_RGB) {
    CharStyle original(255, 128, 64, 32, 16, 8, StyleFlags::BOLD);
    CharStyle copy(original);

    EXPECT_EQ(copy.fg_r, 255);
    EXPECT_EQ(copy.fg_g, 128);
    EXPECT_EQ(copy.fg_b, 64);
    EXPECT_EQ(copy.bg_r, 32);
    EXPECT_EQ(copy.bg_g, 16);
    EXPECT_EQ(copy.bg_b, 8);
}

// ============================================================================
// Тесты move constructor
// ============================================================================

TEST_F(CharStyleTest, MoveConstructor) {
    CharStyle original(Color::RED, Color::BLUE, StyleFlags::ITALIC);
    Color orig_fg = original.fg_color;
    Color orig_bg = original.bg_color;
    StyleFlags orig_style = original.style;

    CharStyle moved(std::move(original));

    EXPECT_EQ(moved.fg_color, orig_fg);
    EXPECT_EQ(moved.bg_color, orig_bg);
    EXPECT_EQ(moved.style, orig_style);

    // После move original должен быть сброшен
    EXPECT_EQ(original.fg_color, Color::BLACK);
    EXPECT_EQ(original.bg_color, Color::WHITE);
    EXPECT_EQ(original.style, StyleFlags::NONE);
}

TEST_F(CharStyleTest, MoveConstructor_RGB) {
    CharStyle original(255, 128, 64, 32, 16, 8, StyleFlags::BOLD);
    CharStyle moved(std::move(original));

    EXPECT_EQ(moved.fg_r, 255);
    EXPECT_EQ(moved.fg_g, 128);
    EXPECT_EQ(moved.fg_b, 64);

    // Original должен быть сброшен
    EXPECT_EQ(original.fg_r, 0);
    EXPECT_EQ(original.fg_g, 0);
    EXPECT_EQ(original.fg_b, 0);
}

// ============================================================================
// Тесты операторов сравнения
// ============================================================================

TEST_F(CharStyleTest, Equality_SameStyles) {
    CharStyle style1(Color::RED, Color::BLUE, StyleFlags::BOLD);
    CharStyle style2(Color::RED, Color::BLUE, StyleFlags::BOLD);

    EXPECT_TRUE(style1 == style2);
    EXPECT_FALSE(style1 != style2);
}

TEST_F(CharStyleTest, Equality_DifferentForeground) {
    CharStyle style1(Color::RED, Color::BLUE);
    CharStyle style2(Color::GREEN, Color::BLUE);

    EXPECT_FALSE(style1 == style2);
    EXPECT_TRUE(style1 != style2);
}

TEST_F(CharStyleTest, Equality_DifferentBackground) {
    CharStyle style1(Color::RED, Color::BLUE);
    CharStyle style2(Color::RED, Color::GREEN);

    EXPECT_FALSE(style1 == style2);
    EXPECT_TRUE(style1 != style2);
}

TEST_F(CharStyleTest, Equality_DifferentStyle) {
    CharStyle style1(Color::RED, Color::BLUE, StyleFlags::BOLD);
    CharStyle style2(Color::RED, Color::BLUE, StyleFlags::ITALIC);

    EXPECT_FALSE(style1 == style2);
    EXPECT_TRUE(style1 != style2);
}

TEST_F(CharStyleTest, Equality_ANSI8) {
    CharStyle style1(100, 200, StyleFlags::NONE);
    CharStyle style2(100, 200, StyleFlags::NONE);
    CharStyle style3(100, 201, StyleFlags::NONE);

    EXPECT_TRUE(style1 == style2);
    EXPECT_FALSE(style1 == style3);
}

TEST_F(CharStyleTest, Equality_RGB) {
    CharStyle style1(255, 128, 64, 32, 16, 8);
    CharStyle style2(255, 128, 64, 32, 16, 8);
    CharStyle style3(255, 128, 64, 32, 16, 9);

    EXPECT_TRUE(style1 == style2);
    EXPECT_FALSE(style1 == style3);
}

TEST_F(CharStyleTest, Equality_SelfComparison) {
    CharStyle style(Color::RED, Color::BLUE);
    EXPECT_TRUE(style == style);
    EXPECT_FALSE(style != style);
}

// ============================================================================
// Тесты copy assignment
// ============================================================================

TEST_F(CharStyleTest, CopyAssignment) {
    CharStyle original(Color::RED, Color::BLUE, StyleFlags::BOLD);
    CharStyle copy;

    copy = original;

    EXPECT_EQ(copy.fg_color, Color::RED);
    EXPECT_EQ(copy.bg_color, Color::BLUE);
    EXPECT_EQ(copy.style, StyleFlags::BOLD);
    EXPECT_EQ(copy, original);
}

TEST_F(CharStyleTest, CopyAssignment_RGB) {
    CharStyle original(255, 128, 64, 32, 16, 8, StyleFlags::ITALIC);
    CharStyle copy;

    copy = original;

    EXPECT_EQ(copy.fg_r, 255);
    EXPECT_EQ(copy.fg_g, 128);
    EXPECT_EQ(copy.fg_b, 64);
    EXPECT_EQ(copy.bg_r, 32);
    EXPECT_EQ(copy.bg_g, 16);
    EXPECT_EQ(copy.bg_b, 8);
}

TEST_F(CharStyleTest, CopyAssignment_SelfAssignment) {
    CharStyle style(Color::RED, Color::BLUE, StyleFlags::BOLD);
    CharStyle& result = (style = style);

    EXPECT_EQ(&result, &style);
    EXPECT_EQ(style.fg_color, Color::RED);
    EXPECT_EQ(style.bg_color, Color::BLUE);
    EXPECT_EQ(style.style, StyleFlags::BOLD);
}

TEST_F(CharStyleTest, CopyAssignment_Chain) {
    CharStyle style1(Color::RED, Color::BLUE);
    CharStyle style2;
    CharStyle style3;

    style3 = style2 = style1;

    EXPECT_EQ(style2, style1);
    EXPECT_EQ(style3, style1);
}

// ============================================================================
// Тесты валидации
// ============================================================================

TEST_F(CharStyleTest, Validate_ValidStyle) {
    CharStyle style(Color::RED, Color::BLUE);
    auto result = style.validate();
    EXPECT_TRUE(result.is_ok());
    EXPECT_TRUE(style.is_valid());
}

TEST_F(CharStyleTest, Validate_ValidRGB) {
    CharStyle style(255, 128, 0, 0, 64, 128);
    auto result = style.validate();
    EXPECT_TRUE(result.is_ok());
    EXPECT_TRUE(style.is_valid());
}

TEST_F(CharStyleTest, Validate_ValidANSI8) {
    CharStyle style(0, 255, StyleFlags::NONE);
    auto result = style.validate();
    EXPECT_TRUE(result.is_ok());
    EXPECT_TRUE(style.is_valid());
}

TEST_F(CharStyleTest, Validate_InvalidRGB_ForegroundTooHigh) {
    CharStyle style(256, 0, 0, 0, 0, 0);
    auto result = style.validate();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
    EXPECT_FALSE(style.is_valid());
}

TEST_F(CharStyleTest, Validate_InvalidRGB_ForegroundNegative) {
    CharStyle style(-1, 0, 0, 0, 0, 0);
    auto result = style.validate();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(CharStyleTest, Validate_InvalidRGB_BackgroundTooHigh) {
    CharStyle style(0, 0, 0, 256, 0, 0);
    auto result = style.validate();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(CharStyleTest, Validate_InvalidRGB_BackgroundNegative) {
    CharStyle style(0, 0, 0, 0, -1, 0);
    auto result = style.validate();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(CharStyleTest, Validate_InvalidANSI8_ForegroundTooHigh) {
    CharStyle style(256, 0, StyleFlags::NONE);
    auto result = style.validate();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(CharStyleTest, Validate_InvalidANSI8_ForegroundNegative) {
    CharStyle style(-1, 0, StyleFlags::NONE);
    auto result = style.validate();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(CharStyleTest, Validate_InvalidANSI8_BackgroundTooHigh) {
    CharStyle style(0, 256, StyleFlags::NONE);
    auto result = style.validate();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(CharStyleTest, Validate_InvalidANSI8_BackgroundNegative) {
    CharStyle style(0, -1, StyleFlags::NONE);
    auto result = style.validate();
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(CharStyleTest, Validate_BoundaryValues) {
    CharStyle style1(0, 0, 0, 0, 0, 0);
    EXPECT_TRUE(style1.is_valid());

    CharStyle style2(255, 255, 255, 255, 255, 255);
    EXPECT_TRUE(style2.is_valid());
}

// ============================================================================
// Тесты reset()
// ============================================================================

TEST_F(CharStyleTest, Reset) {
    CharStyle style(Color::RED, Color::BLUE, StyleFlags::BOLD);
    style.reset();

    EXPECT_EQ(style.fg_color, Color::BLACK);
    EXPECT_EQ(style.bg_color, Color::WHITE);
    EXPECT_EQ(style.style, StyleFlags::NONE);
    EXPECT_EQ(style.fg_ansi, 0);
    EXPECT_EQ(style.bg_ansi, 0);
    EXPECT_EQ(style.fg_r, 0);
    EXPECT_EQ(style.fg_g, 0);
    EXPECT_EQ(style.fg_b, 0);
    EXPECT_EQ(style.bg_r, 0);
    EXPECT_EQ(style.bg_g, 0);
    EXPECT_EQ(style.bg_b, 0);
}

TEST_F(CharStyleTest, Reset_FromRGB) {
    CharStyle style(255, 128, 64, 32, 16, 8, StyleFlags::ITALIC);
    style.reset();

    EXPECT_EQ(style.fg_r, 0);
    EXPECT_EQ(style.fg_g, 0);
    EXPECT_EQ(style.fg_b, 0);
    EXPECT_EQ(style.fg_color, Color::BLACK);
}

TEST_F(CharStyleTest, Reset_MultipleTimesIdempotent) {
    CharStyle style(Color::RED, Color::BLUE);
    style.reset();
    CharStyle first_reset = style;

    style.reset();
    EXPECT_EQ(style, first_reset);
}

// ============================================================================
// Тесты CharStyleStringBuilder - конструкторы
// ============================================================================

TEST_F(CharStyleTest, Builder_DefaultConstructor) {
    CharStyleStringBuilder builder;
    const CharStyle& style = builder.get();

    EXPECT_EQ(style.fg_color, Color::BLACK);
    EXPECT_EQ(style.bg_color, Color::WHITE);
    EXPECT_EQ(style.style, StyleFlags::NONE);
}

TEST_F(CharStyleTest, Builder_ConstructorWithStyle) {
    CharStyle original(Color::RED, Color::BLUE, StyleFlags::BOLD);
    CharStyleStringBuilder builder(original);

    EXPECT_EQ(builder.get(), original);
}

// ============================================================================
// Тесты CharStyleStringBuilder - to_string()
// ============================================================================

TEST_F(CharStyleTest, Builder_ToString_BasicColors) {
    CharStyle style(Color::RED, Color::BLUE);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("\033["), std::string::npos);
    EXPECT_NE(result.find("31"), std::string::npos);  // RED foreground
    EXPECT_NE(result.find("44"), std::string::npos);  // BLUE background
    EXPECT_NE(result.find("m"), std::string::npos);
}

TEST_F(CharStyleTest, Builder_ToString_WithBoldStyle) {
    CharStyle style(Color::WHITE, Color::BLACK, StyleFlags::BOLD);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("1"), std::string::npos);  // BOLD
}

TEST_F(CharStyleTest, Builder_ToString_WithItalicStyle) {
    CharStyle style(Color::WHITE, Color::BLACK, StyleFlags::ITALIC);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("3"), std::string::npos);  // ITALIC
}

TEST_F(CharStyleTest, Builder_ToString_WithUnderlinedStyle) {
    CharStyle style(Color::WHITE, Color::BLACK, StyleFlags::UNDERLINED);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("4"), std::string::npos);  // UNDERLINED
}

TEST_F(CharStyleTest, Builder_ToString_MultipleStyles) {
    CharStyle style(Color::WHITE, Color::BLACK,
        StyleFlags::BOLD | StyleFlags::ITALIC | StyleFlags::UNDERLINED);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("1"), std::string::npos);  // BOLD
    EXPECT_NE(result.find("3"), std::string::npos);  // ITALIC
    EXPECT_NE(result.find("4"), std::string::npos);  // UNDERLINED
}

TEST_F(CharStyleTest, Builder_ToString_ANSI8) {
    CharStyle style(100, 200, StyleFlags::NONE);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("38;5;100"), std::string::npos);  // Foreground ANSI8
    EXPECT_NE(result.find("48;5;200"), std::string::npos);  // Background ANSI8
}

TEST_F(CharStyleTest, Builder_ToString_RGB) {
    CharStyle style(255, 128, 64, 32, 16, 8);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("38;2;255;128;64"), std::string::npos);  // Foreground RGB
    EXPECT_NE(result.find("48;2;32;16;8"), std::string::npos);     // Background RGB
}

TEST_F(CharStyleTest, Builder_ToString_AllBrightColors) {
    std::vector<std::pair<Color, int>> bright_colors = {
        {Color::BRIGHT_RED, 91},
        {Color::BRIGHT_GREEN, 92},
        {Color::BRIGHT_YELLOW, 93},
        {Color::BRIGHT_BLUE, 94},
        {Color::BRIGHT_MAGENTA, 95},
        {Color::BRIGHT_CYAN, 96},
        {Color::BRIGHT_WHITE, 97}
    };

    for (const auto& [color, code] : bright_colors) {
        CharStyle style(color, Color::BLACK);
        CharStyleStringBuilder builder(style);
        std::string result = builder.to_string();

        EXPECT_NE(result.find(std::to_string(code)), std::string::npos)
            << "Color code " << code << " not found";
    }
}

TEST_F(CharStyleTest, Builder_ToString_AllStandardColors) {
    std::vector<std::pair<Color, int>> standard_colors = {
        {Color::BLACK, 30},
        {Color::RED, 31},
        {Color::GREEN, 32},
        {Color::YELLOW, 33},
        {Color::BLUE, 34},
        {Color::MAGENTA, 35},
        {Color::CYAN, 36},
        {Color::WHITE, 37}
    };

    for (const auto& [color, code] : standard_colors) {
        CharStyle style(color, Color::BLACK);
        CharStyleStringBuilder builder(style);
        std::string result = builder.to_string();

        EXPECT_NE(result.find(std::to_string(code)), std::string::npos)
            << "Color code " << code << " not found";
    }
}

TEST_F(CharStyleTest, Builder_ToString_AllStyleFlags) {
    std::vector<std::pair<StyleFlags, std::string>> styles = {
        {StyleFlags::BOLD, "1"},
        {StyleFlags::DIM, "2"},
        {StyleFlags::ITALIC, "3"},
        {StyleFlags::UNDERLINED, "4"},
        {StyleFlags::BLINK_SLOW, "5"},
        {StyleFlags::BLINK_FAST, "6"},
        {StyleFlags::INVERSE, "7"},
        {StyleFlags::HIDDEN, "8"},
        {StyleFlags::STRIKE_THROUGH, "9"}
    };

    for (const auto& [flag, code] : styles) {
        CharStyle style(Color::WHITE, Color::BLACK, flag);
        CharStyleStringBuilder builder(style);
        std::string result = builder.to_string();

        EXPECT_NE(result.find(code), std::string::npos)
            << "Style code " << code << " not found";
    }
}

TEST_F(CharStyleTest, Builder_ToString_StartsAndEndsCorrectly) {
    CharStyle style(Color::RED, Color::BLUE);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_EQ(result.substr(0, 2), "\033[");
    EXPECT_EQ(result.back(), 'm');
}

TEST_F(CharStyleTest, Builder_ToString_Grey) {
    CharStyle style(Color::GREY, Color::BLACK);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("90"), std::string::npos);  // GREY
}

// ============================================================================
// Интеграционные тесты
// ============================================================================

TEST_F(CharStyleTest, Integration_CreateModifyValidate) {
    CharStyle style(Color::RED, Color::BLUE);
    EXPECT_TRUE(style.is_valid());

    CharStyle modified = style;
    EXPECT_EQ(modified, style);
    EXPECT_TRUE(modified.is_valid());
}

TEST_F(CharStyleTest, Integration_BuilderAndValidation) {
    CharStyle style(255, 128, 64, 32, 16, 8, StyleFlags::BOLD);
    EXPECT_TRUE(style.is_valid());

    CharStyleStringBuilder builder(style);
    std::string ansi = builder.to_string();

    EXPECT_FALSE(ansi.empty());
    EXPECT_NE(ansi.find("38;2;255;128;64"), std::string::npos);
}

TEST_F(CharStyleTest, Integration_CopyAndCompare) {
    CharStyle original(Color::RED, Color::BLUE, StyleFlags::ITALIC);
    CharStyle copy1(original);
    CharStyle copy2;
    copy2 = original;

    EXPECT_EQ(original, copy1);
    EXPECT_EQ(original, copy2);
    EXPECT_EQ(copy1, copy2);
}

TEST_F(CharStyleTest, Integration_ResetAndReuse) {
    CharStyle style(Color::RED, Color::BLUE, StyleFlags::BOLD);
    style.reset();

    CharStyle default_style;
    EXPECT_EQ(style, default_style);

    style = CharStyle(Color::GREEN, Color::YELLOW);
    EXPECT_NE(style, default_style);
    EXPECT_TRUE(style.is_valid());
}

// ============================================================================
// Граничные случаи
// ============================================================================

TEST_F(CharStyleTest, EdgeCase_MinMaxRGB) {
    CharStyle min_style(0, 0, 0, 0, 0, 0);
    EXPECT_TRUE(min_style.is_valid());

    CharStyle max_style(255, 255, 255, 255, 255, 255);
    EXPECT_TRUE(max_style.is_valid());
}

TEST_F(CharStyleTest, EdgeCase_MinMaxANSI8) {
    CharStyle min_style(0, 0, StyleFlags::NONE);
    EXPECT_TRUE(min_style.is_valid());

    CharStyle max_style(255, 255, StyleFlags::NONE);
    EXPECT_TRUE(max_style.is_valid());
}

TEST_F(CharStyleTest, EdgeCase_AllStyleFlagsCombined) {
    StyleFlags all_flags = StyleFlags::BOLD | StyleFlags::DIM | StyleFlags::ITALIC |
        StyleFlags::UNDERLINED | StyleFlags::BLINK_SLOW |
        StyleFlags::BLINK_FAST | StyleFlags::INVERSE |
        StyleFlags::HIDDEN | StyleFlags::STRIKE_THROUGH;

    CharStyle style(Color::WHITE, Color::BLACK, all_flags);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_NE(result.find("1"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_NE(result.find("3"), std::string::npos);
    EXPECT_NE(result.find("4"), std::string::npos);
    EXPECT_NE(result.find("5"), std::string::npos);
    EXPECT_NE(result.find("6"), std::string::npos);
    EXPECT_NE(result.find("7"), std::string::npos);
    EXPECT_NE(result.find("8"), std::string::npos);
    EXPECT_NE(result.find("9"), std::string::npos);
}

TEST_F(CharStyleTest, EdgeCase_EmptyStyle) {
    CharStyle style(Color::WHITE, Color::BLACK, StyleFlags::NONE);
    CharStyleStringBuilder builder(style);
    std::string result = builder.to_string();

    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("\033["), std::string::npos);
    EXPECT_NE(result.find("m"), std::string::npos);
}