#include <gtest/gtest.h>
#include "../tui/frame_building.h"

using namespace dungeons;
using namespace dungeons::tui;

// ============================================================================
// Тесты конструкторов
// ============================================================================

TEST(FrameSnapshotTest, ConstructorFromMatrix) {
    UnicodeCharMatrix chars(U'\u0041', 3, 5); // A
    CharStyle style(Color::RED, Color::BLUE);

    FrameSnapshot frame(chars, style);

    EXPECT_EQ(frame.rows(), 3);
    EXPECT_EQ(frame.cols(), 5);

    auto ch = frame.get_char(0, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'\u0041');

    auto st = frame.style_at(0, 0);
    ASSERT_TRUE(st.is_ok());
    EXPECT_EQ(st.value(), style);
}

TEST(FrameSnapshotTest, ConstructorFromFiller) {
    FrameSnapshot frame(U'\u0058', CharStyle(Color::GREEN, Color::BLACK), 2, 4); // X

    EXPECT_EQ(frame.rows(), 2);
    EXPECT_EQ(frame.cols(), 4);

    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 4; ++j) {
            auto ch = frame.get_char(i, j);
            ASSERT_TRUE(ch.is_ok());
            EXPECT_EQ(ch.value(), U'\u0058');
        }
    }
}

TEST(FrameSnapshotTest, ConstructorWithUnicode) {
    FrameSnapshot frame(U'\u0416', CharStyle(), 2, 3); // Ж

    auto ch = frame.get_char(1, 2);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'\u0416');
}

// ============================================================================
// Тесты работы с символами - массовая установка
// ============================================================================

TEST(FrameSnapshotTest, SetCharsSuccess) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 3); // space

    auto result = frame.set_chars({ "ABC", "DEF" });
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(0, 0).value(), U'\u0041'); // A
    EXPECT_EQ(frame.get_char(0, 1).value(), U'\u0042'); // B
    EXPECT_EQ(frame.get_char(0, 2).value(), U'\u0043'); // C
    EXPECT_EQ(frame.get_char(1, 0).value(), U'\u0044'); // D
    EXPECT_EQ(frame.get_char(1, 1).value(), U'\u0045'); // E
    EXPECT_EQ(frame.get_char(1, 2).value(), U'\u0046'); // F
}

TEST(FrameSnapshotTest, SetCharsWithCyrillic) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 3); // space

    // А=\u0410, Б=\u0411, В=\u0412, Г=\u0413, Д=\u0414, Е=\u0415
    auto result = frame.set_chars({ "\u0410\u0411\u0412", "\u0413\u0414\u0415" });
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(0, 0).value(), U'\u0410');
    EXPECT_EQ(frame.get_char(1, 2).value(), U'\u0415');
}

TEST(FrameSnapshotTest, SetCharsSizeMismatch) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 3); // space

    auto result = frame.set_chars({ "AB", "CD" }); // 2 столбца вместо 3
    ASSERT_FALSE(result.is_ok());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST(FrameSnapshotTest, SetCharsRowMismatch) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 3); // space

    auto result = frame.set_chars({ "ABC", "DEF", "GHI" }); // 3 строки вместо 2
    ASSERT_FALSE(result.is_ok());
}

TEST(FrameSnapshotTest, SetCharRow) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 3, 4); // space

    auto result = frame.set_char_row(1, "TEST");
    ASSERT_TRUE(result.is_ok());

    auto row = frame.get_char_row(1);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "TEST");
}

TEST(FrameSnapshotTest, SetCharRowWithUnicode) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 4); // space

    // Привет = \u041f\u0440\u0438\u0432
    auto result = frame.set_char_row(0, "\u041f\u0440\u0438\u0432");
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(0, 0).value(), U'\u041f');
    EXPECT_EQ(frame.get_char(0, 1).value(), U'\u0440');
}

TEST(FrameSnapshotTest, GetCharRow) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 3); // space
    frame.set_char_row(0, "ABC");

    auto row = frame.get_char_row(0);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "ABC");
}

TEST(FrameSnapshotTest, GetCharRowInvalidIndex) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 3); // space

    auto row = frame.get_char_row(5);
    ASSERT_FALSE(row.is_ok());
    EXPECT_EQ(row.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты работы с отдельными символами
// ============================================================================

TEST(FrameSnapshotTest, GetCharSuccess) {
    FrameSnapshot frame(U'\u005a', CharStyle(), 3, 3); // Z

    auto ch = frame.get_char(1, 1);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'\u005a');
}

TEST(FrameSnapshotTest, GetCharOutOfRange) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space

    auto ch = frame.get_char(2, 0);
    ASSERT_FALSE(ch.is_ok());
    EXPECT_EQ(ch.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST(FrameSnapshotTest, GetCharUtf8) {
    FrameSnapshot frame(U'\u0419', CharStyle(), 2, 2); // Й

    auto ch = frame.get_char_utf8(0, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), "\u0419");
}

TEST(FrameSnapshotTest, SetCharSuccess) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space

    auto result = frame.set_char(0, 1, U'\u0051'); // Q
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(0, 1).value(), U'\u0051');
}

TEST(FrameSnapshotTest, SetCharUtf8) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space

    // Ё = \u0401
    auto result = frame.set_char_utf8(1, 1, "\u0401");
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(1, 1).value(), U'\u0401');
}

TEST(FrameSnapshotTest, SetCharOutOfRange) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space

    auto result = frame.set_char(0, 5, U'\u0058'); // X
    ASSERT_FALSE(result.is_ok());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты работы со стилями
// ============================================================================

TEST(FrameSnapshotTest, SetAreaStyle) {
    FrameSnapshot frame(U'\u0020', CharStyle(Color::BLACK, Color::WHITE), 4, 4); // space
    CharStyle newStyle(Color::RED, Color::BLUE, StyleFlags::BOLD);

    auto result = frame.set_area_style(1, 1, 2, 2, newStyle);
    ASSERT_TRUE(result.is_ok());

    // Проверяем установленную область
    EXPECT_EQ(frame.style_at(1, 1).value(), newStyle);
    EXPECT_EQ(frame.style_at(1, 2).value(), newStyle);
    EXPECT_EQ(frame.style_at(2, 1).value(), newStyle);
    EXPECT_EQ(frame.style_at(2, 2).value(), newStyle);

    // Проверяем, что другие ячейки не изменились
    EXPECT_NE(frame.style_at(0, 0).value(), newStyle);
    EXPECT_NE(frame.style_at(3, 3).value(), newStyle);
}

TEST(FrameSnapshotTest, SetRowStyle) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 3, 3); // space
    CharStyle style(Color::GREEN, Color::YELLOW);

    auto result = frame.set_row_style(1, style);
    ASSERT_TRUE(result.is_ok());

    for (size_t col = 0; col < 3; ++col) {
        EXPECT_EQ(frame.style_at(1, col).value(), style);
    }
}

TEST(FrameSnapshotTest, SetStyleSingleCell) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space
    CharStyle style(Color::MAGENTA, Color::CYAN, StyleFlags::UNDERLINED);

    auto result = frame.set_style(0, 1, style);
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.style_at(0, 1).value(), style);
}

TEST(FrameSnapshotTest, StyleAtOutOfRange) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space

    auto style = frame.style_at(3, 0);
    ASSERT_FALSE(style.is_ok());
    EXPECT_EQ(style.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST(FrameSnapshotTest, DefaultStyle) {
    CharStyle defaultStyle(Color::BLUE, Color::RED);
    FrameSnapshot frame(U'\u0020', defaultStyle, 2, 2); // space

    auto style = frame.default_style();
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), defaultStyle);
}

TEST(FrameSnapshotTest, SetDefaultStyle) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space
    CharStyle newDefault(Color::YELLOW, Color::BLACK);

    auto result = frame.set_default_style(newDefault);
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.default_style().value(), newDefault);
}

// ============================================================================
// Тесты работы с символами и стилями одновременно
// ============================================================================

TEST(FrameSnapshotTest, SetCell) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 3, 3); // space
    CharStyle style(Color::RED, Color::GREEN);

    auto result = frame.set_cell(1, 1, U'\u0058', style); // X
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(1, 1).value(), U'\u0058');
    EXPECT_EQ(frame.style_at(1, 1).value(), style);
}

TEST(FrameSnapshotTest, SetCellUtf8) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space
    CharStyle style(Color::BLUE, Color::WHITE);

    // Щ = \u0429
    auto result = frame.set_cell_utf8(0, 0, "\u0429", style);
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(0, 0).value(), U'\u0429');
    EXPECT_EQ(frame.style_at(0, 0).value(), style);
}

TEST(FrameSnapshotTest, SetCellOutOfRange) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space
    CharStyle style(Color::RED, Color::BLUE);

    auto result = frame.set_cell(5, 5, U'\u0041', style); // A
    ASSERT_FALSE(result.is_ok());
}

// ============================================================================
// Тесты работы с областями - копирование и вставка
// ============================================================================

TEST(FrameSnapshotTest, BlitSuccess) {
    FrameSnapshot source(U'\u0053', CharStyle(Color::RED, Color::WHITE), 2, 2); // S
    FrameSnapshot dest(U'\u0044', CharStyle(Color::BLUE, Color::BLACK), 4, 4); // D

    auto result = dest.blit(source, 0, 0, 1, 1, 2, 2);
    ASSERT_TRUE(result.is_ok());

    // Проверяем скопированную область
    EXPECT_EQ(dest.get_char(1, 1).value(), U'\u0053');
    EXPECT_EQ(dest.get_char(1, 2).value(), U'\u0053');
    EXPECT_EQ(dest.get_char(2, 1).value(), U'\u0053');
    EXPECT_EQ(dest.get_char(2, 2).value(), U'\u0053');

    // Проверяем стили
    EXPECT_EQ(dest.style_at(1, 1).value().fg_color, Color::RED);

    // Проверяем, что остальное не изменилось
    EXPECT_EQ(dest.get_char(0, 0).value(), U'\u0044');
}

TEST(FrameSnapshotTest, BlitWithUnicode) {
    UnicodeCharMatrix sourceChars({ "\u0410\u0411", "\u0412\u0413" }); // АБ, ВГ
    FrameSnapshot source(sourceChars, CharStyle());
    FrameSnapshot dest(U'\u0020', CharStyle(), 4, 4); // space

    auto result = dest.blit(source, 0, 0, 1, 1, 2, 2);
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(dest.get_char(1, 1).value(), U'\u0410');
    EXPECT_EQ(dest.get_char(1, 2).value(), U'\u0411');
    EXPECT_EQ(dest.get_char(2, 1).value(), U'\u0412');
    EXPECT_EQ(dest.get_char(2, 2).value(), U'\u0413');
}

TEST(FrameSnapshotTest, BlitSourceOutOfBounds) {
    FrameSnapshot source(U'\u0053', CharStyle(), 2, 2); // S
    FrameSnapshot dest(U'\u0044', CharStyle(), 4, 4); // D

    auto result = dest.blit(source, 0, 0, 0, 0, 3, 3); // Пытаемся скопировать 3x3 из 2x2
    ASSERT_FALSE(result.is_ok());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST(FrameSnapshotTest, BlitDestOutOfBounds) {
    FrameSnapshot source(U'\u0053', CharStyle(), 2, 2); // S
    FrameSnapshot dest(U'\u0044', CharStyle(), 3, 3); // D

    auto result = dest.blit(source, 0, 0, 2, 2, 2, 2); // Выход за границы dest
    ASSERT_FALSE(result.is_ok());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST(FrameSnapshotTest, CopyRegion) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 5, 5); // space
    CharStyle style(Color::RED, Color::WHITE);

    // Заполняем исходную область
    frame.set_cell(1, 1, U'\u0041', style); // A
    frame.set_cell(1, 2, U'\u0042', style); // B

    auto result = frame.copy_region(1, 1, 3, 3, 2, 2);
    ASSERT_TRUE(result.is_ok());

    // Проверяем скопированную область
    EXPECT_EQ(frame.get_char(3, 3).value(), U'\u0041');
    EXPECT_EQ(frame.get_char(3, 4).value(), U'\u0042');
    EXPECT_EQ(frame.style_at(3, 3).value(), style);
}

TEST(FrameSnapshotTest, CopyRegionOverlapping) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 4, 4); // space
    frame.set_char(0, 0, U'\u0058'); // X
    frame.set_char(0, 1, U'\u0059'); // Y

    // Копирование с перекрытием
    auto result = frame.copy_region(0, 0, 1, 0, 2, 2);
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(1, 0).value(), U'\u0058');
    EXPECT_EQ(frame.get_char(1, 1).value(), U'\u0059');
}

// ============================================================================
// Тесты очистки и заполнения
// ============================================================================

TEST(FrameSnapshotTest, Clear) {
    FrameSnapshot frame(U'\u0058', CharStyle(Color::RED, Color::BLUE), 3, 3); // X

    frame.clear();

    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(frame.get_char(i, j).value(), U'\u0020'); // space
        }
    }
}

TEST(FrameSnapshotTest, ClearRegion) {
    FrameSnapshot frame(U'\u0041', CharStyle(Color::RED, Color::WHITE), 4, 4); // A

    auto result = frame.clear_region(1, 1, 2, 2);
    ASSERT_TRUE(result.is_ok());

    // Проверяем очищенную область
    EXPECT_EQ(frame.get_char(1, 1).value(), U'\u0020'); // space
    EXPECT_EQ(frame.get_char(1, 2).value(), U'\u0020');
    EXPECT_EQ(frame.get_char(2, 1).value(), U'\u0020');
    EXPECT_EQ(frame.get_char(2, 2).value(), U'\u0020');

    // Проверяем, что остальное не изменилось
    EXPECT_EQ(frame.get_char(0, 0).value(), U'\u0041');
    EXPECT_EQ(frame.get_char(3, 3).value(), U'\u0041');
}

TEST(FrameSnapshotTest, ClearRegionInvalidRange) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 3, 3); // space

    auto result = frame.clear_region(2, 0, 1, 2); // start_row > end_row
    ASSERT_FALSE(result.is_ok());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST(FrameSnapshotTest, FillRegion) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 4, 4); // space
    CharStyle style(Color::YELLOW, Color::MAGENTA, StyleFlags::BOLD);

    auto result = frame.fill_region(1, 1, 2, 2, U'\u0023', style); // #
    ASSERT_TRUE(result.is_ok());

    // Проверяем заполненную область
    for (size_t i = 1; i <= 2; ++i) {
        for (size_t j = 1; j <= 2; ++j) {
            EXPECT_EQ(frame.get_char(i, j).value(), U'\u0023');
            EXPECT_EQ(frame.style_at(i, j).value(), style);
        }
    }

    // Проверяем, что остальное не изменилось
    EXPECT_EQ(frame.get_char(0, 0).value(), U'\u0020');
}

TEST(FrameSnapshotTest, FillRegionWithUnicode) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 3, 3); // space
    CharStyle style(Color::GREEN, Color::BLACK);

    // ★ = \u2605
    auto result = frame.fill_region(0, 0, 1, 1, U'\u2605', style);
    ASSERT_TRUE(result.is_ok());

    EXPECT_EQ(frame.get_char(0, 0).value(), U'\u2605');
    EXPECT_EQ(frame.get_char(0, 1).value(), U'\u2605');
    EXPECT_EQ(frame.get_char(1, 0).value(), U'\u2605');
    EXPECT_EQ(frame.get_char(1, 1).value(), U'\u2605');
}

TEST(FrameSnapshotTest, FillRegionOutOfBounds) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space
    CharStyle style(Color::RED, Color::BLUE);

    auto result = frame.fill_region(0, 0, 5, 5, U'\u0058', style); // X
    ASSERT_FALSE(result.is_ok());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты конвертации в строку
// ============================================================================

TEST(FrameSnapshotTest, ToStringSimple) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 3); // space
    frame.set_char(0, 0, U'\u0041'); // A
    frame.set_char(0, 1, U'\u0042'); // B
    frame.set_char(0, 2, U'\u0043'); // C

    auto result = frame.to_string();
    ASSERT_TRUE(result.is_ok());

    // Строка должна содержать ANSI коды и символы
    EXPECT_TRUE(result.value().find("ABC") != std::string::npos);
}

TEST(FrameSnapshotTest, ToStringWithStyles) {
    FrameSnapshot frame(U'\u0058', CharStyle(Color::RED, Color::BLUE), 1, 3); // X

    auto result = frame.to_string();
    ASSERT_TRUE(result.is_ok());

    // Должны быть ANSI escape коды
    EXPECT_TRUE(result.value().find("\033[") != std::string::npos);
}

TEST(FrameSnapshotTest, ToPlainString) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space
    frame.set_char(0, 0, U'\u0041'); // A
    frame.set_char(0, 1, U'\u0042'); // B

    std::string plain = frame.to_plain_string();

    // Не должно быть ANSI кодов
    EXPECT_TRUE(plain.find("\033[") == std::string::npos);
    EXPECT_TRUE(plain.find("A") != std::string::npos);
    EXPECT_TRUE(plain.find("B") != std::string::npos);
}

TEST(FrameSnapshotTest, ToPlainStringWithUnicode) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 2, 2); // space
    // Мир = \u041c\u0438\u0440
    frame.set_char_utf8(0, 0, "\u041c");
    frame.set_char_utf8(0, 1, "\u0438");

    std::string plain = frame.to_plain_string();
    EXPECT_TRUE(plain.find("\u041c") != std::string::npos);
}

TEST(FrameSnapshotTest, Info) {
    FrameSnapshot frame(U'\u0020', CharStyle(Color::RED, Color::BLUE), 3, 5); // space

    std::string info = frame.info();

    EXPECT_TRUE(info.find("3x5") != std::string::npos);
    EXPECT_TRUE(info.find("FrameSnapshot") != std::string::npos);
}

// ============================================================================
// Тесты граничных случаев
// ============================================================================

TEST(FrameSnapshotTest, SingleCell) {
    FrameSnapshot frame(U'\u0058', CharStyle(), 1, 1); // X

    EXPECT_EQ(frame.rows(), 1);
    EXPECT_EQ(frame.cols(), 1);
    EXPECT_EQ(frame.get_char(0, 0).value(), U'\u0058');
}

TEST(FrameSnapshotTest, LargeFrame) {
    FrameSnapshot frame(U'\u002e', CharStyle(), 100, 100); // .

    EXPECT_EQ(frame.rows(), 100);
    EXPECT_EQ(frame.cols(), 100);

    frame.set_char(99, 99, U'\u005a'); // Z
    EXPECT_EQ(frame.get_char(99, 99).value(), U'\u005a');
}

TEST(FrameSnapshotTest, ComplexUnicodeScenario) {
    FrameSnapshot frame(U'\u0020', CharStyle(), 3, 4); // space

    // Emoji: 😀 = \U0001f600, 🎉 = \U0001f389
    frame.set_char(0, 0, U'\U0001f600');
    frame.set_char(0, 1, U'\U0001f389');
    // Cyrillic: Я = \u042f, ё = \u0451
    frame.set_char(1, 0, U'\u042f');
    frame.set_char(1, 1, U'\u0451');

    EXPECT_EQ(frame.get_char(0, 0).value(), U'\U0001f600');
    EXPECT_EQ(frame.get_char(0, 1).value(), U'\U0001f389');
    EXPECT_EQ(frame.get_char(1, 0).value(), U'\u042f');
    EXPECT_EQ(frame.get_char(1, 1).value(), U'\u0451');
}

TEST(FrameSnapshotTest, StyleChangeDetection) {
    FrameSnapshot frame(U'\u0041', CharStyle(Color::RED, Color::WHITE), 2, 3); // A

    CharStyle newStyle(Color::BLUE, Color::BLACK, StyleFlags::BOLD);
    frame.set_style(0, 1, newStyle);

    auto result = frame.to_string();
    ASSERT_TRUE(result.is_ok());

    // Должно быть как минимум 2 разных ANSI последовательности
    size_t escapeCount = 0;
    size_t pos = 0;
    while ((pos = result.value().find("\033[", pos)) != std::string::npos) {
        escapeCount++;
        pos += 2;
    }
    EXPECT_GE(escapeCount, 2);
}