#include <gtest/gtest.h>
#include "../tui/RawCharMatrix.h"

using namespace dungeons;
using namespace dungeons::tui;

class RawCharMatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Общая инициализация
    }
};

// ============================================================================
// Тесты конструкторов
// ============================================================================

TEST_F(RawCharMatrixTest, Constructor_WithFiller) {
    RawCharMatrix matrix('#', 3, 5);

    EXPECT_EQ(matrix.rows(), 3);
    auto cols_result = matrix.cols();
    ASSERT_TRUE(cols_result.is_ok());
    EXPECT_EQ(cols_result.value(), 5);

    // Проверяем заполнение
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            auto ch = matrix.get_at(i, j);
            ASSERT_TRUE(ch.is_ok());
            EXPECT_EQ(ch.value(), '#');
        }
    }
}

TEST_F(RawCharMatrixTest, Constructor_WithFiller_ZeroRows) {
    EXPECT_THROW(RawCharMatrix('#', 0, 5), std::invalid_argument);
}

TEST_F(RawCharMatrixTest, Constructor_WithFiller_ZeroCols) {
    EXPECT_THROW(RawCharMatrix('#', 3, 0), std::invalid_argument);
}

TEST_F(RawCharMatrixTest, Constructor_FromStringWithDimensions) {
    std::string input = "ABCDEFGHIJKL";
    RawCharMatrix matrix(input, 3, 4);

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 4);

    // Проверяем содержимое
    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ABCD");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "EFGH");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "IJKL");
}

TEST_F(RawCharMatrixTest, Constructor_FromStringWithDimensions_ShortString) {
    std::string input = "ABC";
    RawCharMatrix matrix(input, 2, 3);

    // Первая строка заполнена, вторая - пробелами
    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ABC");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "   ");
}

TEST_F(RawCharMatrixTest, Constructor_FromStringWithNewlines) {
    std::string input = "ABC\nDEF\nGHI";
    RawCharMatrix matrix(input);

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 3);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ABC");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "DEF");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "GHI");
}

TEST_F(RawCharMatrixTest, Constructor_FromStringWithNewlines_DifferentLengths) {
    std::string input = "AB\nDEFG\nH";
    RawCharMatrix matrix(input);

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 4); // max_cols = 4

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "AB  "); // Дополнено пробелами

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "DEFG");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "H   ");
}

TEST_F(RawCharMatrixTest, Constructor_FromStringWithNewlines_EmptyString) {
    EXPECT_THROW(RawCharMatrix(""), std::invalid_argument);
}

TEST_F(RawCharMatrixTest, Constructor_FromStringWithNewlines_OnlyNewlines) {
    EXPECT_THROW(RawCharMatrix("\n\n\n"), std::invalid_argument);
}

TEST_F(RawCharMatrixTest, Constructor_FromInitializerList) {
    RawCharMatrix matrix({ "ABC", "DEF", "GHI" });

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 3);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ABC");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "DEF");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "GHI");
}

TEST_F(RawCharMatrixTest, Constructor_FromInitializerList_DifferentLengths) {
    RawCharMatrix matrix({ "AB", "DEFG", "H" });

    EXPECT_EQ(matrix.rows(), 3);
    EXPECT_EQ(matrix.cols().value(), 4);

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "AB  ");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "DEFG");
}

TEST_F(RawCharMatrixTest, Constructor_FromInitializerList_Empty) {
    EXPECT_THROW(RawCharMatrix({}), std::invalid_argument);
}

TEST_F(RawCharMatrixTest, Constructor_FromInitializerList_EmptyStrings) {
    EXPECT_THROW(RawCharMatrix({ "", "", "" }), std::invalid_argument);
}

// ============================================================================
// Тесты копирования и перемещения
// ============================================================================

TEST_F(RawCharMatrixTest, CopyConstructor) {
    RawCharMatrix original({ "ABC", "DEF" });
    RawCharMatrix copy(original);

    EXPECT_EQ(copy.rows(), original.rows());
    EXPECT_EQ(copy.cols().value(), original.cols().value());

    auto row0 = copy.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ABC");
}

TEST_F(RawCharMatrixTest, CopyAssignment) {
    RawCharMatrix original({ "ABC", "DEF" });
    RawCharMatrix copy('#', 1, 1);

    copy = original;

    EXPECT_EQ(copy.rows(), original.rows());
    EXPECT_EQ(copy.cols().value(), original.cols().value());

    auto row0 = copy.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ABC");
}

TEST_F(RawCharMatrixTest, MoveConstructor) {
    RawCharMatrix original({ "ABC", "DEF" });
    RawCharMatrix moved(std::move(original));

    EXPECT_EQ(moved.rows(), 2);
    EXPECT_EQ(moved.cols().value(), 3);

    auto row0 = moved.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ABC");
}

TEST_F(RawCharMatrixTest, MoveAssignment) {
    RawCharMatrix original({ "ABC", "DEF" });
    RawCharMatrix moved('#', 1, 1);

    moved = std::move(original);

    EXPECT_EQ(moved.rows(), 2);
    EXPECT_EQ(moved.cols().value(), 3);
}

// ============================================================================
// Тесты геттеров размеров
// ============================================================================

TEST_F(RawCharMatrixTest, Rows) {
    RawCharMatrix matrix('#', 5, 3);
    EXPECT_EQ(matrix.rows(), 5);
}

TEST_F(RawCharMatrixTest, Cols) {
    RawCharMatrix matrix('#', 3, 7);
    auto result = matrix.cols();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), 7);
}

TEST_F(RawCharMatrixTest, Empty_False) {
    RawCharMatrix matrix('#', 1, 1);
    EXPECT_FALSE(matrix.empty());
}

// ============================================================================
// Тесты get_at и set_at
// ============================================================================

TEST_F(RawCharMatrixTest, GetAt_Valid) {
    RawCharMatrix matrix({ "ABC", "DEF", "GHI" });

    auto ch = matrix.get_at(1, 1);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), 'E');
}

TEST_F(RawCharMatrixTest, GetAt_InvalidRow) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.get_at(5, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(RawCharMatrixTest, GetAt_InvalidCol) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.get_at(0, 10);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(RawCharMatrixTest, SetAt_Valid) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.set_at(0, 1, 'X');
    EXPECT_TRUE(result.is_ok());

    auto ch = matrix.get_at(0, 1);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), 'X');
}

TEST_F(RawCharMatrixTest, SetAt_InvalidRow) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.set_at(5, 0, 'X');
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(RawCharMatrixTest, SetAt_InvalidCol) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.set_at(0, 10, 'X');
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(RawCharMatrixTest, SetAt_AllPositions) {
    RawCharMatrix matrix('#', 2, 3);

    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            char ch = 'A' + (i * 3 + j);
            auto result = matrix.set_at(i, j, ch);
            EXPECT_TRUE(result.is_ok());
        }
    }

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "ABC");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "DEF");
}

// ============================================================================
// Тесты get_row и set_row
// ============================================================================

TEST_F(RawCharMatrixTest, GetRow_Valid) {
    RawCharMatrix matrix({ "ABC", "DEF", "GHI" });

    auto result = matrix.get_row(1);
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), "DEF");
}

TEST_F(RawCharMatrixTest, GetRow_InvalidRow) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.get_row(5);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(RawCharMatrixTest, SetRow_InitializerList) {
    RawCharMatrix matrix({ "ABC", "DEF", "GHI" });

    auto result = matrix.set_row(1, { 'X', 'Y', 'Z' });
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(1);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "XYZ");
}

TEST_F(RawCharMatrixTest, SetRow_InitializerList_WrongSize) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.set_row(0, { 'X', 'Y' });
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST_F(RawCharMatrixTest, SetRow_StringView) {
    RawCharMatrix matrix({ "ABC", "DEF", "GHI" });

    auto result = matrix.set_row(1, std::string_view("XYZ"));
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(1);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "XYZ");
}

TEST_F(RawCharMatrixTest, SetRow_String) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.set_row(0, std::string("XYZ"));
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(0);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "XYZ");
}

TEST_F(RawCharMatrixTest, SetRow_CString) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.set_row(1, "XYZ");
    EXPECT_TRUE(result.is_ok());

    auto row = matrix.get_row(1);
    ASSERT_TRUE(row.is_ok());
    EXPECT_EQ(row.value(), "XYZ");
}

TEST_F(RawCharMatrixTest, SetRow_InvalidRow) {
    RawCharMatrix matrix({ "ABC", "DEF" });

    auto result = matrix.set_row(5, "XYZ");
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты submatrix
// ============================================================================

TEST_F(RawCharMatrixTest, Submatrix_Valid) {
    RawCharMatrix matrix({ "ABCD", "EFGH", "IJKL", "MNOP" });

    auto result = matrix.submatrix(1, 1, 2, 2);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 2);
    EXPECT_EQ(sub.cols().value(), 2);

    auto row0 = sub.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "FG");

    auto row1 = sub.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "JK");
}

TEST_F(RawCharMatrixTest, Submatrix_SingleElement) {
    RawCharMatrix matrix({ "ABCD", "EFGH" });

    auto result = matrix.submatrix(0, 2, 0, 2);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 1);
    EXPECT_EQ(sub.cols().value(), 1);

    auto ch = sub.get_at(0, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), 'C');
}

TEST_F(RawCharMatrixTest, Submatrix_FullMatrix) {
    RawCharMatrix matrix({ "AB", "CD" });

    auto result = matrix.submatrix(0, 0, 1, 1);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 2);
    EXPECT_EQ(sub.cols().value(), 2);
}

TEST_F(RawCharMatrixTest, Submatrix_InvalidRange_StartGreaterThanEnd) {
    RawCharMatrix matrix({ "ABCD", "EFGH" });

    auto result = matrix.submatrix(1, 0, 0, 2);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST_F(RawCharMatrixTest, Submatrix_OutOfRange) {
    RawCharMatrix matrix({ "AB", "CD" });

    auto result = matrix.submatrix(0, 0, 5, 5);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты set_submatrix
// ============================================================================

TEST_F(RawCharMatrixTest, SetSubmatrix_Valid) {
    RawCharMatrix matrix({ "AAAA", "AAAA", "AAAA", "AAAA" });
    RawCharMatrix sub({ "XY", "ZW" });

    auto result = matrix.set_submatrix(1, 1, sub);
    EXPECT_TRUE(result.is_ok());

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "AAAA");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "AXYA");

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    EXPECT_EQ(row2.value(), "AZWA");
}

TEST_F(RawCharMatrixTest, SetSubmatrix_AtCorner) {
    RawCharMatrix matrix({ "AAA", "AAA", "AAA" });
    RawCharMatrix sub({ "XY", "ZW" });

    auto result = matrix.set_submatrix(0, 0, sub);
    EXPECT_TRUE(result.is_ok());

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "XYA");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "ZWA");
}

TEST_F(RawCharMatrixTest, SetSubmatrix_DoesntFit) {
    RawCharMatrix matrix({ "AA", "AA" });
    RawCharMatrix sub({ "XXXX", "YYYY", "ZZZZ" });

    auto result = matrix.set_submatrix(0, 0, sub);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(RawCharMatrixTest, SetSubmatrix_InvalidPosition) {
    RawCharMatrix matrix({ "AAA", "AAA" });
    RawCharMatrix sub({ "X", "Y" });

    auto result = matrix.set_submatrix(5, 5, sub);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты clear
// ============================================================================

TEST_F(RawCharMatrixTest, Clear_DefaultFiller) {
    RawCharMatrix matrix({ "ABC", "DEF" });
    matrix.clear();

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "   ");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "   ");
}

TEST_F(RawCharMatrixTest, Clear_CustomFiller) {
    RawCharMatrix matrix({ "ABC", "DEF" });
    matrix.clear('#');

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "###");

    auto row1 = matrix.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "###");
}

// ============================================================================
// Тесты to_string
// ============================================================================

TEST_F(RawCharMatrixTest, ToString) {
    RawCharMatrix matrix({ "ABC", "DEF", "GHI" });
    std::string result = matrix.to_string();
    EXPECT_EQ(result, "ABC\nDEF\nGHI\n");
}

TEST_F(RawCharMatrixTest, ToString_SingleRow) {
    RawCharMatrix matrix({ "ABCDEF" });
    std::string result = matrix.to_string();
    EXPECT_EQ(result, "ABCDEF\n");
}

TEST_F(RawCharMatrixTest, ToString_SingleChar) {
    RawCharMatrix matrix({ "X" });
    std::string result = matrix.to_string();
    EXPECT_EQ(result, "X\n");
}

// ============================================================================
// Интеграционные тесты
// ============================================================================

TEST_F(RawCharMatrixTest, Integration_ComplexScenario) {
    // Создаем матрицу
    RawCharMatrix matrix('#', 5, 5);

    // Устанавливаем строки
    EXPECT_TRUE(matrix.set_row(0, "AAAAA").is_ok());
    EXPECT_TRUE(matrix.set_row(1, "BBBBB").is_ok());
    EXPECT_TRUE(matrix.set_row(2, "CCCCC").is_ok());
    EXPECT_TRUE(matrix.set_row(3, "DDDDD").is_ok());
    EXPECT_TRUE(matrix.set_row(4, "EEEEE").is_ok());

    // Получаем субматрицу
    auto sub_result = matrix.submatrix(1, 1, 3, 3);
    ASSERT_TRUE(sub_result.is_ok());
    auto sub = sub_result.value();

    // Проверяем субматрицу
    EXPECT_EQ(sub.rows(), 3);
    EXPECT_EQ(sub.cols().value(), 3);

    auto sub_row0 = sub.get_row(0);
    ASSERT_TRUE(sub_row0.is_ok());
    EXPECT_EQ(sub_row0.value(), "BBB");

    // Модифицируем оригинал
    EXPECT_TRUE(matrix.set_at(2, 2, 'X').is_ok());

    // Субматрица не должна измениться (это копия)
    auto sub_ch = sub.get_at(1, 1);
    ASSERT_TRUE(sub_ch.is_ok());
    EXPECT_EQ(sub_ch.value(), 'C');

    // Проверяем оригинал
    auto orig_ch = matrix.get_at(2, 2);
    ASSERT_TRUE(orig_ch.is_ok());
    EXPECT_EQ(orig_ch.value(), 'X');
}

TEST_F(RawCharMatrixTest, Integration_SetAndGetAllElements) {
    RawCharMatrix matrix(' ', 3, 3);

    char ch = 'A';
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_TRUE(matrix.set_at(i, j, ch++).is_ok());
        }
    }

    ch = 'A';
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            auto result = matrix.get_at(i, j);
            ASSERT_TRUE(result.is_ok());
            EXPECT_EQ(result.value(), ch++);
        }
    }
}

TEST_F(RawCharMatrixTest, Integration_CopyAndModify) {
    RawCharMatrix original({ "ABC", "DEF" });
    RawCharMatrix copy = original;

    // Модифицируем копию
    EXPECT_TRUE(copy.set_at(0, 0, 'X').is_ok());

    // Оригинал не должен измениться
    auto orig_ch = original.get_at(0, 0);
    ASSERT_TRUE(orig_ch.is_ok());
    EXPECT_EQ(orig_ch.value(), 'A');

    // Копия изменилась
    auto copy_ch = copy.get_at(0, 0);
    ASSERT_TRUE(copy_ch.is_ok());
    EXPECT_EQ(copy_ch.value(), 'X');
}

TEST_F(RawCharMatrixTest, Integration_NestedSubmatrices) {
    RawCharMatrix matrix({ "ABCDEF", "GHIJKL", "MNOPQR", "STUVWX", "YZ1234", "567890" });

    // Получаем большую субматрицу
    auto big_sub_result = matrix.submatrix(1, 1, 4, 4);
    ASSERT_TRUE(big_sub_result.is_ok());
    auto big_sub = big_sub_result.value();

    EXPECT_EQ(big_sub.rows(), 4);
    EXPECT_EQ(big_sub.cols().value(), 4);

    // Получаем маленькую субматрицу из большой
    auto small_sub_result = big_sub.submatrix(1, 1, 2, 2);
    ASSERT_TRUE(small_sub_result.is_ok());
    auto small_sub = small_sub_result.value();

    EXPECT_EQ(small_sub.rows(), 2);
    EXPECT_EQ(small_sub.cols().value(), 2);

    auto row0 = small_sub.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_EQ(row0.value(), "OP");

    auto row1 = small_sub.get_row(1);
    ASSERT_TRUE(row1.is_ok());
    EXPECT_EQ(row1.value(), "UV");
}

// ============================================================================
// Граничные случаи
// ============================================================================

TEST_F(RawCharMatrixTest, EdgeCase_SingleElementMatrix) {
    RawCharMatrix matrix('#', 1, 1);

    EXPECT_EQ(matrix.rows(), 1);
    EXPECT_EQ(matrix.cols().value(), 1);

    auto ch = matrix.get_at(0, 0);
    ASSERT_TRUE(ch.is_ok());
    EXPECT_EQ(ch.value(), '#');
}

TEST_F(RawCharMatrixTest, EdgeCase_LargeMatrix) {
    RawCharMatrix matrix('#', 100, 100);

    EXPECT_EQ(matrix.rows(), 100);
    EXPECT_EQ(matrix.cols().value(), 100);

    // Проверяем углы
    auto corner = matrix.get_at(99, 99);
    ASSERT_TRUE(corner.is_ok());
    EXPECT_EQ(corner.value(), '#');
}

TEST_F(RawCharMatrixTest, EdgeCase_SpecialCharacters) {
    RawCharMatrix matrix({ "\t\n ", "!@#", "АБВ" });

    auto row0 = matrix.get_row(0);
    ASSERT_TRUE(row0.is_ok());
    EXPECT_GE(row0.value().size(), 3);

    auto row2 = matrix.get_row(2);
    ASSERT_TRUE(row2.is_ok());
    // UTF-8 символы займут больше байт
    EXPECT_GT(row2.value().size(), 0);
}