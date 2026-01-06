#include <gtest/gtest.h>
#include "../tui/UnicodeCharMatrix.h"

using namespace dungeons;
using namespace dungeons::tui;

class UnicodeCharMatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Общая инициализация
    }
};

// ============================================================================
// Тесты конструкторов
// ============================================================================

TEST_F(UnicodeCharMatrixTest, Constructor_WithUnicodeFiller) {
    UnicodeCharMatrix matrix(U'\u0416', 3, 5);

    EXPECT_EQ(matrix.rows(), 3);
    auto cols_result = matrix.cols();
    ASSERT_TRUE(cols_result.is_ok());
    EXPECT_EQ(cols_result.value(), 5);

    // Проверяем заполнение
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            auto ch = matrix.get_at(i, j);
            ASSERT_TRUE(ch.is_ok());
            EXPECT_EQ(ch.value(), U'Ж');
        }
    }
}

TEST_F(UnicodeCharMatrixTest, Constructor_WithEmojiFiller) {
    UnicodeCharMatrix matrix(U'\U0001F3AE', 2, 2);  // ✅ 🎮 через код

    auto ch = matrix.get_at(0, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'\U0001F3AE');
}

TEST_F(UnicodeCharMatrixTest, Constructor_WithFiller_ZeroRows) {
    EXPECT_THROW(UnicodeCharMatrix(U'\u0410', 0, 5), std::invalid_argument);
}

TEST_F(UnicodeCharMatrixTest, Constructor_WithFiller_ZeroCols) {
    EXPECT_THROW(UnicodeCharMatrix(U'\u0410', 3, 0), std::invalid_argument);
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromUTF8WithDimensions_Cyrillic) {
    std::string input = "АБВГДЕЖЗИЙКЛ";
    UnicodeCharMatrix matrix(input, 3, 4);

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 4);

    // Проверяем содержимое
    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АБВГ");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "ДЕЖЗ");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "ИЙКЛ");
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromUTF8WithDimensions_Mixed) {
    std::string input = "АBВГDЕёЖ";
    UnicodeCharMatrix matrix(input, 2, 4);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АBВГ");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "DЕёЖ");
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromUTF8WithNewlines_Cyrillic) {
    std::string input = "АБВ\nГДЕ\nЁЖЗ";
    UnicodeCharMatrix matrix(input);

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 3);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АБВ");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "ГДЕ");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "ЁЖЗ");
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromUTF8WithNewlines_DifferentLengths) {
    std::string input = "АБ\nГДЕЁ\nЖ";
    UnicodeCharMatrix matrix(input);

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 4); // max_cols = 4

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АБ  "); // Дополнено пробелами

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "ГДЕЁ");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "Ж   ");
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromUTF8WithNewlines_Emoji) {
    std::string input = "🎮🎯🎲\n🎪🎨🎭";
    UnicodeCharMatrix matrix(input);

    EXPECT_EQ(matrix.rows(), 2);
    EXPECT_EQ(matrix.cols().value(), 3);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "🎮🎯🎲");
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromUTF8WithNewlines_Chinese) {
    std::string input = "你好\n世界";
    UnicodeCharMatrix matrix(input);

    EXPECT_EQ(matrix.rows(), 2);
    EXPECT_EQ(matrix.cols().value(), 2);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "你好");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "世界");
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromUTF8WithNewlines_EmptyString) {
    EXPECT_THROW(UnicodeCharMatrix(""), std::invalid_argument);
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromInitializerList_Cyrillic) {
    UnicodeCharMatrix matrix({ "Привет", "Мир", "Тест" });

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 6);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "Привет");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "Мир   ");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "Тест  ");
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromInitializerList_Mixed) {
    UnicodeCharMatrix matrix({ "АБ", "CDEFG", "Ё" });

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 5);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АБ   ");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "CDEFG");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "Ё    ");
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromInitializerList_Empty) {
    EXPECT_THROW(UnicodeCharMatrix({}), std::invalid_argument);
}

TEST_F(UnicodeCharMatrixTest, Constructor_FromInitializerList_EmptyStrings) {
    EXPECT_THROW(UnicodeCharMatrix({ "", "", "" }), std::invalid_argument);
}

// ============================================================================
// Тесты копирования и перемещения
// ============================================================================

TEST_F(UnicodeCharMatrixTest, CopyConstructor) {
    UnicodeCharMatrix original({ "АБВ", "ГДЕ" });
    UnicodeCharMatrix copy(original);

    EXPECT_EQ(copy.rows(), original.rows());
    EXPECT_EQ(copy.cols().value(), original.cols().value());

    auto row0 = copy.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АБВ");
}

TEST_F(UnicodeCharMatrixTest, CopyAssignment) {
    UnicodeCharMatrix original({ "АБВ", "ГДЕ" });
    UnicodeCharMatrix copy(U'#', 1, 1);

    copy = original;

    EXPECT_EQ(copy.rows(), original.rows());
    EXPECT_EQ(copy.cols().value(), original.cols().value());

    auto row0 = copy.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АБВ");
}

TEST_F(UnicodeCharMatrixTest, MoveConstructor) {
    UnicodeCharMatrix original({ "АБВ", "ГДЕ" });
    UnicodeCharMatrix moved(std::move(original));

    EXPECT_EQ(moved.rows(), 2);
    EXPECT_EQ(moved.cols().value(), 3);

    auto row0 = moved.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АБВ");
}

TEST_F(UnicodeCharMatrixTest, MoveAssignment) {
    UnicodeCharMatrix original({ "АБВ", "ГДЕ" });
    UnicodeCharMatrix moved(U'#', 1, 1);

    moved = std::move(original);

    EXPECT_EQ(moved.rows(), 2);
    EXPECT_EQ(moved.cols().value(), 3);
}

// ============================================================================
// Тесты геттеров размеров
// ============================================================================

TEST_F(UnicodeCharMatrixTest, Rows) {
    UnicodeCharMatrix matrix(U'#', 5, 3);
    EXPECT_EQ(matrix.rows(), 5);
}

TEST_F(UnicodeCharMatrixTest, Cols) {
    UnicodeCharMatrix matrix(U'#', 3, 7);
    auto result = matrix.cols();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), 7);
}

TEST_F(UnicodeCharMatrixTest, Empty_False) {
    UnicodeCharMatrix matrix(U'#', 1, 1);
    EXPECT_FALSE(matrix.empty());
}

// ============================================================================
// Тесты get_at и set_at
// ============================================================================

TEST_F(UnicodeCharMatrixTest, GetAt_Valid_Cyrillic) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ", "ЁЖЗ" });

    auto ch = matrix.get_at(1, 1);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'Д');
}

TEST_F(UnicodeCharMatrixTest, GetAt_Valid_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎯", "🎲🎪" });

    auto ch = matrix.get_at(0, 1);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'🎯');
}

TEST_F(UnicodeCharMatrixTest, GetAt_InvalidRow) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.get_at(5, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(UnicodeCharMatrixTest, GetAt_InvalidCol) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.get_at(0, 10);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(UnicodeCharMatrixTest, SetAt_Valid_Cyrillic) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_at(0, 1, U'Я');
    EXPECT_TRUE(result.is_ok());

    auto ch = matrix.get_at(0, 1);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'Я');
}

TEST_F(UnicodeCharMatrixTest, SetAt_Valid_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎯", "🎲🎪" });

    auto result = matrix.set_at(1, 0, U'🎨');
    EXPECT_TRUE(result.is_ok());

    auto ch = matrix.get_at(1, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'🎨');
}

TEST_F(UnicodeCharMatrixTest, SetAt_InvalidRow) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_at(5, 0, U'Я');
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(UnicodeCharMatrixTest, SetAt_InvalidCol) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_at(0, 10, U'Я');
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(UnicodeCharMatrixTest, SetAtUTF8_Valid) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_at_utf8(0, 1, "Ё");
    EXPECT_TRUE(result.is_ok());

    auto ch_result = matrix.get_at_utf8(0, 1);
    ASSERT_TRUE(ch_result.is_ok());
    EXPECT_EQ(ch_result.value(), "Ё");
}

TEST_F(UnicodeCharMatrixTest, SetAtUTF8_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎯", "🎲🎪" });

    auto result = matrix.set_at_utf8(1, 1, "🎨");
    EXPECT_TRUE(result.is_ok());

    auto ch_result = matrix.get_at_utf8(1, 1);
    ASSERT_TRUE(ch_result.is_ok());
    EXPECT_EQ(ch_result.value(), "🎨");
}

TEST_F(UnicodeCharMatrixTest, SetAtUTF8_EmptyString) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_at_utf8(0, 0, "");
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST_F(UnicodeCharMatrixTest, SetAtUTF8_MultiCharString_TakesFirst) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_at_utf8(0, 0, "ЯЮ");
    EXPECT_TRUE(result.is_ok());

    auto ch = matrix.get_at(0, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'Я'); // Берется только первый символ
}

// ============================================================================
// Тесты get_row и set_row
// ============================================================================

TEST_F(UnicodeCharMatrixTest, GetRow_Valid_Cyrillic) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ", "ЁЖЗ" });

    auto result = matrix.get_row(1);
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), "ГДЕ");
}

TEST_F(UnicodeCharMatrixTest, GetRow_InvalidRow) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.get_row(5);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(UnicodeCharMatrixTest, SetRow_InitializerList) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ", "ЁЖЗ" });

    auto result = matrix.set_row(1, { U'Я', U'Ю', U'Э' });
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(1);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "ЯЮЭ");
}

TEST_F(UnicodeCharMatrixTest, SetRow_InitializerList_WrongSize) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_row(0, { U'Я', U'Ю' });
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST_F(UnicodeCharMatrixTest, SetRow_String_Cyrillic) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ", "ЁЖЗ" });

    auto result = matrix.set_row(1, "ЯЮЭ");
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(1);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "ЯЮЭ");
}

TEST_F(UnicodeCharMatrixTest, SetRow_String_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎯🎲", "🎪🎨🎭" });

    auto result = matrix.set_row(0, "🎵🎶🎤");
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(0);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "🎵🎶🎤");
}

TEST_F(UnicodeCharMatrixTest, SetRow_StringView) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_row(0, std::string_view("ЯЮЭ"));
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(0);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "ЯЮЭ");
}

TEST_F(UnicodeCharMatrixTest, SetRow_CString) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_row(1, "ЯЮЭ");
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(1);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "ЯЮЭ");
}

TEST_F(UnicodeCharMatrixTest, SetRow_InvalidRow) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_row(5, "ЯЮЭ");
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(UnicodeCharMatrixTest, SetRow_WrongSize) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });

    auto result = matrix.set_row(0, "ЯЮЭЩ");
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

// ============================================================================
// Тесты submatrix
// ============================================================================

TEST_F(UnicodeCharMatrixTest, Submatrix_Valid_Cyrillic) {
    UnicodeCharMatrix matrix({ "АБВГ", "ДЕЖЗ", "ИЙКЛ", "МНОП" });

    auto result = matrix.submatrix(1, 1, 2, 2);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 2);
    EXPECT_EQ(sub.cols().value(), 2);

    auto row0 = sub.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ЕЖ");

    auto row1 = sub.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "ЙК");
}

TEST_F(UnicodeCharMatrixTest, Submatrix_SingleElement_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎯🎲🎪", "🎨🎭🎵🎶" });

    auto result = matrix.submatrix(0, 2, 0, 2);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 1);
    EXPECT_EQ(sub.cols().value(), 1);

    auto ch = sub.get_at(0, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'🎲');
}

TEST_F(UnicodeCharMatrixTest, Submatrix_FullMatrix) {
    UnicodeCharMatrix matrix({ "АБ", "ВГ" });

    auto result = matrix.submatrix(0, 0, 1, 1);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 2);
    EXPECT_EQ(sub.cols().value(), 2);
}

TEST_F(UnicodeCharMatrixTest, Submatrix_InvalidRange) {
    UnicodeCharMatrix matrix({ "АБВГ", "ДЕЖЗ" });

    auto result = matrix.submatrix(1, 0, 0, 2);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST_F(UnicodeCharMatrixTest, Submatrix_OutOfRange) {
    UnicodeCharMatrix matrix({ "АБ", "ВГ" });

    auto result = matrix.submatrix(0, 0, 5, 5);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты set_submatrix
// ============================================================================

TEST_F(UnicodeCharMatrixTest, SetSubmatrix_Valid_Cyrillic) {
    UnicodeCharMatrix matrix({ "АААА", "АААА", "АААА", "АААА" });
    UnicodeCharMatrix sub({ "ЯЮ", "ЭЩ" });

    auto result = matrix.set_submatrix(1, 1, sub);
    EXPECT_TRUE(result.is_ok());

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "АААА");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "АЯЮА");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "АЭЩА");
}

TEST_F(UnicodeCharMatrixTest, SetSubmatrix_AtCorner_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎮🎮", "🎮🎮🎮", "🎮🎮🎮" });
    UnicodeCharMatrix sub({ "🎯🎲", "🎪🎨" });

    auto result = matrix.set_submatrix(0, 0, sub);
    EXPECT_TRUE(result.is_ok());

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "🎯🎲🎮");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "🎪🎨🎮");
}

TEST_F(UnicodeCharMatrixTest, SetSubmatrix_DoesntFit) {
    UnicodeCharMatrix matrix({ "АА", "АА" });
    UnicodeCharMatrix sub({ "ЯЯЯЯ", "ЮЮЮЮ", "ЭЭЭЭЭ" });

    auto result = matrix.set_submatrix(0, 0, sub);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(UnicodeCharMatrixTest, SetSubmatrix_InvalidPosition) {
    UnicodeCharMatrix matrix({ "ААА", "ААА" });
    UnicodeCharMatrix sub({ "Я", "Ю" });

    auto result = matrix.set_submatrix(5, 5, sub);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты fill_with и clear
// ============================================================================

TEST_F(UnicodeCharMatrixTest, FillWith_Cyrillic) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });
    matrix.fill_with(U'Я');

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ЯЯЯ");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "ЯЯЯ");
}

TEST_F(UnicodeCharMatrixTest, FillWith_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎯", "🎲🎪" });
    matrix.fill_with(U'🎨');

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "🎨🎨");
}

TEST_F(UnicodeCharMatrixTest, Clear_DefaultSpace) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ" });
    matrix.clear();

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "   ");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "   ");
}

// ============================================================================
// Тесты to_string
// ============================================================================

TEST_F(UnicodeCharMatrixTest, ToString_Cyrillic) {
    UnicodeCharMatrix matrix({ "АБВ", "ГДЕ", "ЁЖЗ" });
    std::string result = matrix.to_string();
    EXPECT_EQ(result, "АБВ\nГДЕ\nЁЖЗ\n");
}

TEST_F(UnicodeCharMatrixTest, ToString_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎯", "🎲🎪" });
    std::string result = matrix.to_string();
    EXPECT_EQ(result, "🎮🎯\n🎲🎪\n");
}

TEST_F(UnicodeCharMatrixTest, ToString_Mixed) {
    UnicodeCharMatrix matrix({ "АBC", "123", "🎮ЯZ" });
    std::string result = matrix.to_string();
    EXPECT_EQ(result, "АBC\n123\n🎮ЯZ\n");
}

TEST_F(UnicodeCharMatrixTest, ToString_SingleRow) {
    UnicodeCharMatrix matrix({ "АБВГДЕ" });
    std::string result = matrix.to_string();
    EXPECT_EQ(result, "АБВГДЕ\n");
}

// ============================================================================
// Тесты UTF8 byte size
// ============================================================================

TEST_F(UnicodeCharMatrixTest, UTF8ByteSize_ASCII) {
    UnicodeCharMatrix matrix({ "ABC", "DEF" });
    size_t size = matrix.utf8_byte_size();
    EXPECT_EQ(size, 8); // "ABC\n" + "DEF\n" = 3+1+3+1 = 8
}

TEST_F(UnicodeCharMatrixTest, UTF8ByteSize_Cyrillic) {
    UnicodeCharMatrix matrix({ "АБВ" }); // 3 символа * 2 байта = 6 байт
    size_t size = matrix.utf8_byte_size();
    EXPECT_EQ(size, 7); // 6 + 1 (\n)
}

TEST_F(UnicodeCharMatrixTest, UTF8ByteSize_Emoji) {
    UnicodeCharMatrix matrix({ "🎮🎯" }); // 2 символа * 4 байта = 8 байт
    size_t size = matrix.utf8_byte_size();
    EXPECT_EQ(size, 9); // 8 + 1 (\n)
}

// ============================================================================
// Интеграционные тесты
// ============================================================================

TEST_F(UnicodeCharMatrixTest, Integration_ComplexScenario_Cyrillic) {
    // Создаем матрицу
    UnicodeCharMatrix matrix(U'#', 5, 5);

    // Устанавливаем строки
    EXPECT_TRUE(matrix.set_row(0, "ААААА").is_ok());
    EXPECT_TRUE(matrix.set_row(1, "БББББ").is_ok());
    EXPECT_TRUE(matrix.set_row(2, "ВВВВВ").is_ok());
    EXPECT_TRUE(matrix.set_row(3, "ГГГГГ").is_ok());
    EXPECT_TRUE(matrix.set_row(4, "ДДДДД").is_ok());

    // Получаем субматрицу
    auto sub_result = matrix.submatrix(1, 1, 3, 3);
    ASSERT_TRUE(sub_result.is_ok());
    auto sub = sub_result.value();

    // Проверяем субматрицу
    EXPECT_EQ(sub.rows(), 3);
    EXPECT_EQ(sub.cols().value(), 3);

    auto sub_row0 = sub.get_row(0);
    ASSERT_TRUE(sub_row0.is_ok());
    EXPECT_EQ(sub_row0.value(), "БББ");

    // Модифицируем оригинал
    EXPECT_TRUE(matrix.set_at(2, 2, U'Я').is_ok());

    // Субматрица не должна измениться (это копия)
    auto sub_ch = sub.get_at(1, 1);
    ASSERT_TRUE(sub_ch.is_ok());
    EXPECT_EQ(sub_ch.value(), U'В');

    // Проверяем оригинал
    auto orig_ch = matrix.get_at(2, 2);
    ASSERT_TRUE(orig_ch.is_ok());
    EXPECT_EQ(orig_ch.value(), U'Я');
}

TEST_F(UnicodeCharMatrixTest, Integration_MixedContent) {
    UnicodeCharMatrix matrix({ "АBC", "123", "🎮ЯZ" });

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 3);

    // Проверяем смешанное содержимое
    auto ch0 = matrix.get_at(0, 0);
    ASSERT_TRUE(ch0.is_ok());
    EXPECT_EQ(ch0.value(), U'А');

    auto ch1 = matrix.get_at(0, 1);
    ASSERT_TRUE(ch1.is_ok());
    EXPECT_EQ(ch1.value(), U'B');

    auto ch2 = matrix.get_at(2, 0);
    ASSERT_TRUE(ch2.is_ok());
    EXPECT_EQ(ch2.value(), U'🎮');
}

TEST_F(UnicodeCharMatrixTest, Integration_CopyAndModify) {
    UnicodeCharMatrix original({ "АБВ", "ГДЕ" });
    UnicodeCharMatrix copy = original;

    // Модифицируем копию
    EXPECT_TRUE(copy.set_at(0, 0, U'Я').is_ok());

    // Оригинал не должен измениться
    auto orig_ch = original.get_at(0, 0);
    ASSERT_TRUE(orig_ch.is_ok());
    EXPECT_EQ(orig_ch.value(), U'А');

    // Копия изменилась
    auto copy_ch = copy.get_at(0, 0);
    ASSERT_TRUE(copy_ch.is_ok());
    EXPECT_EQ(copy_ch.value(), U'Я');
}

// ============================================================================
// Граничные случаи
// ============================================================================

TEST_F(UnicodeCharMatrixTest, EdgeCase_SingleElementMatrix) {
    UnicodeCharMatrix matrix(U'Я', 1, 1);

    EXPECT_EQ(matrix.rows(), 1);
    EXPECT_EQ(matrix.cols().value(), 1);

    auto ch = matrix.get_at(0, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), U'Я');
}

TEST_F(UnicodeCharMatrixTest, EdgeCase_LargeMatrix_Cyrillic) {
    UnicodeCharMatrix matrix(U'Я', 100, 100);

    EXPECT_EQ(matrix.rows(), 100);
    EXPECT_EQ(matrix.cols().value(), 100);

    // Проверяем углы
    auto corner = matrix.get_at(99, 99);
    ASSERT_TRUE(corner.is_ok());
    EXPECT_EQ(corner.value(), U'Я');
}

TEST_F(UnicodeCharMatrixTest, EdgeCase_AllSpaces) {
    UnicodeCharMatrix matrix(U' ', 3, 3);
    matrix.fill_with(U' ');

    std::string result = matrix.to_string();
    EXPECT_EQ(result, "   \n   \n   \n");
}

TEST_F(UnicodeCharMatrixTest, EdgeCase_ComplexEmoji) {
    // Некоторые эмодзи могут быть составными (ZWJ sequences)
    UnicodeCharMatrix matrix({ "👨‍👩‍👧‍👦", "👍🏻" }); // Family emoji, thumbs up with skin tone

    EXPECT_GT(matrix.rows(), 0);
    EXPECT_TRUE(matrix.cols().is_ok());
}

// ============================================================================
// Сравнение с RawCharMatrix (демонстрация разницы)
// ============================================================================

TEST_F(UnicodeCharMatrixTest, Comparison_CyrillicHandling) {
    std::string cyrillic = "АБВ";

    // RawCharMatrix посчитает 6 байт (2 байта на символ)
    // UnicodeCharMatrix посчитает 3 символа
    UnicodeCharMatrix unicode_matrix(cyrillic);

    EXPECT_EQ(unicode_matrix.rows(), 1);
    EXPECT_EQ(unicode_matrix.cols().value(), 3); // ✅ 3 символа!

    auto row = unicode_matrix.get_row(0);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "АБВ");
}