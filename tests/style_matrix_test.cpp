#include <gtest/gtest.h>
#include "../tui/StyleMatrix.h"

using namespace dungeons;
using namespace dungeons::tui;

class StyleMatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем базовые стили для тестов
        default_style = CharStyle(Color::WHITE, Color::BLACK);
        red_style = CharStyle(Color::RED, Color::BLACK);
        green_style = CharStyle(Color::GREEN, Color::WHITE);
        blue_style = CharStyle(Color::BLUE, Color::YELLOW);

        // RGB стиль
        rgb_style = CharStyle(255, 0, 0, 0, 0, 255, StyleFlags::BOLD);

        // ANSI8 стиль
        ansi_style = CharStyle(100, 200, StyleFlags::ITALIC);
    }

    CharStyle default_style;
    CharStyle red_style;
    CharStyle green_style;
    CharStyle blue_style;
    CharStyle rgb_style;
    CharStyle ansi_style;
};

// ============================================================================
// Тесты конструкторов
// ============================================================================

TEST_F(StyleMatrixTest, Constructor_WithDefaultStyle) {
    StyleMatrix matrix(default_style, 3, 5);

    EXPECT_EQ(matrix.rows(), 3);
    auto cols_result = matrix.cols();
    ASSERT_TRUE(cols_result.is_ok());
    EXPECT_EQ(cols_result.value(), 5);

    // Проверяем заполнение default_style
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 5; ++j) {
            auto style = matrix.at(i, j);
            ASSERT_TRUE(style.is_ok());
            EXPECT_EQ(style.value(), default_style);
        }
    }
}

TEST_F(StyleMatrixTest, Constructor_WithCustomStyle) {
    StyleMatrix matrix(red_style, 2, 3);

    auto style = matrix.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), red_style);
}

TEST_F(StyleMatrixTest, Constructor_ZeroRows) {
    EXPECT_THROW(StyleMatrix(default_style, 0, 5), std::invalid_argument);
}

TEST_F(StyleMatrixTest, Constructor_ZeroCols) {
    EXPECT_THROW(StyleMatrix(default_style, 3, 0), std::invalid_argument);
}

TEST_F(StyleMatrixTest, Constructor_BothZero) {
    EXPECT_THROW(StyleMatrix(default_style, 0, 0), std::invalid_argument);
}

// ============================================================================
// Тесты копирования и перемещения
// ============================================================================

TEST_F(StyleMatrixTest, CopyConstructor) {
    StyleMatrix original(red_style, 2, 3);
    original.set_at(0, 0, green_style);
    original.set_at(1, 1, blue_style);

    StyleMatrix copy(original);

    EXPECT_EQ(copy.rows(), original.rows());
    EXPECT_EQ(copy.cols().value(), original.cols().value());

    auto style1 = copy.at(0, 0);
    ASSERT_TRUE(style1.is_ok());
    EXPECT_EQ(style1.value(), green_style);

    auto style2 = copy.at(1, 1);
    ASSERT_TRUE(style2.is_ok());
    EXPECT_EQ(style2.value(), blue_style);
}

TEST_F(StyleMatrixTest, CopyAssignment) {
    StyleMatrix original(red_style, 2, 3);
    original.set_at(0, 0, green_style);

    StyleMatrix copy(default_style, 1, 1);
    copy = original;

    EXPECT_EQ(copy.rows(), original.rows());
    EXPECT_EQ(copy.cols().value(), original.cols().value());

    auto style = copy.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), green_style);
}

TEST_F(StyleMatrixTest, MoveConstructor) {
    StyleMatrix original(red_style, 2, 3);
    original.set_at(0, 0, green_style);

    StyleMatrix moved(std::move(original));

    EXPECT_EQ(moved.rows(), 2);
    EXPECT_EQ(moved.cols().value(), 3);

    auto style = moved.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), green_style);
}

TEST_F(StyleMatrixTest, MoveAssignment) {
    StyleMatrix original(red_style, 2, 3);
    original.set_at(0, 0, green_style);

    StyleMatrix moved(default_style, 1, 1);
    moved = std::move(original);

    EXPECT_EQ(moved.rows(), 2);
    EXPECT_EQ(moved.cols().value(), 3);

    auto style = moved.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), green_style);
}

TEST_F(StyleMatrixTest, SelfAssignment) {
    StyleMatrix matrix(red_style, 2, 3);
    matrix.set_at(0, 0, green_style);

    StyleMatrix& ref = matrix;
    matrix = ref;  // Самоприсваивание

    auto style = matrix.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), green_style);
}

// ============================================================================
// Тесты геттеров размеров
// ============================================================================

TEST_F(StyleMatrixTest, Rows) {
    StyleMatrix matrix(default_style, 7, 3);
    EXPECT_EQ(matrix.rows(), 7);
}

TEST_F(StyleMatrixTest, Cols) {
    StyleMatrix matrix(default_style, 3, 9);
    auto result = matrix.cols();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), 9);
}

TEST_F(StyleMatrixTest, Empty_False) {
    StyleMatrix matrix(default_style, 1, 1);
    EXPECT_FALSE(matrix.empty());
}

// ============================================================================
// Тесты at и set_at
// ============================================================================

TEST_F(StyleMatrixTest, At_Valid) {
    StyleMatrix matrix(default_style, 3, 3);
    matrix.set_at(1, 1, red_style);

    auto result = matrix.at(1, 1);
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), red_style);
}

TEST_F(StyleMatrixTest, At_InvalidRow) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.at(5, 0);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(StyleMatrixTest, At_InvalidCol) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.at(0, 10);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(StyleMatrixTest, SetAt_Valid) {
    StyleMatrix matrix(default_style, 3, 3);

    auto result = matrix.set_at(0, 1, red_style);
    EXPECT_TRUE(result.is_ok());

    auto style = matrix.at(0, 1);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), red_style);
}

TEST_F(StyleMatrixTest, SetAt_InvalidRow) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.set_at(5, 0, red_style);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(StyleMatrixTest, SetAt_InvalidCol) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.set_at(0, 10, red_style);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(StyleMatrixTest, SetAt_InvalidStyle_RGB) {
    StyleMatrix matrix(default_style, 2, 2);

    // Некорректный RGB стиль (значения вне диапазона)
    CharStyle invalid_style(999, 0, 0, 0, 0, 0, StyleFlags::NONE);

    auto result = matrix.set_at(0, 0, invalid_style);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(StyleMatrixTest, SetAt_ValidRGBStyle) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.set_at(0, 0, rgb_style);
    EXPECT_TRUE(result.is_ok());

    auto style = matrix.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), rgb_style);
}

TEST_F(StyleMatrixTest, SetAt_ValidANSI8Style) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.set_at(0, 0, ansi_style);
    EXPECT_TRUE(result.is_ok());

    auto style = matrix.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), ansi_style);
}

TEST_F(StyleMatrixTest, SetAt_AllPositions) {
    StyleMatrix matrix(default_style, 2, 3);

    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            auto result = matrix.set_at(i, j, red_style);
            EXPECT_TRUE(result.is_ok());
        }
    }

    // Проверяем все позиции
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            auto style = matrix.at(i, j);
            ASSERT_TRUE(style.is_ok());
            EXPECT_EQ(style.value(), red_style);
        }
    }
}

// ============================================================================
// Тесты set_row
// ============================================================================

TEST_F(StyleMatrixTest, SetRow_Valid) {
    StyleMatrix matrix(default_style, 3, 4);

    auto result = matrix.set_row(1, red_style);
    EXPECT_TRUE(result.is_ok());

    // Проверяем всю строку
    for (size_t j = 0; j < 4; ++j) {
        auto style = matrix.at(1, j);
        ASSERT_TRUE(style.is_ok());
        EXPECT_EQ(style.value(), red_style);
    }

    // Проверяем, что другие строки не изменились
    auto style0 = matrix.at(0, 0);
    ASSERT_TRUE(style0.is_ok());
    EXPECT_EQ(style0.value(), default_style);
}

TEST_F(StyleMatrixTest, SetRow_InvalidRow) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.set_row(5, red_style);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(StyleMatrixTest, SetRow_InvalidStyle) {
    StyleMatrix matrix(default_style, 2, 2);

    CharStyle invalid_style(999, 0, 0, 0, 0, 0, StyleFlags::NONE);
    auto result = matrix.set_row(0, invalid_style);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(StyleMatrixTest, SetRow_AllRows) {
    StyleMatrix matrix(default_style, 3, 4);

    EXPECT_TRUE(matrix.set_row(0, red_style).is_ok());
    EXPECT_TRUE(matrix.set_row(1, green_style).is_ok());
    EXPECT_TRUE(matrix.set_row(2, blue_style).is_ok());

    auto style0 = matrix.at(0, 0);
    ASSERT_TRUE(style0.is_ok());
    EXPECT_EQ(style0.value(), red_style);

    auto style1 = matrix.at(1, 0);
    ASSERT_TRUE(style1.is_ok());
    EXPECT_EQ(style1.value(), green_style);

    auto style2 = matrix.at(2, 0);
    ASSERT_TRUE(style2.is_ok());
    EXPECT_EQ(style2.value(), blue_style);
}

// ============================================================================
// Тесты set_area
// ============================================================================

TEST_F(StyleMatrixTest, SetArea_Valid) {
    StyleMatrix matrix(default_style, 5, 5);

    auto result = matrix.set_area(1, 1, 3, 3, red_style);
    EXPECT_TRUE(result.is_ok());

    // Проверяем область
    for (size_t i = 1; i <= 3; ++i) {
        for (size_t j = 1; j <= 3; ++j) {
            auto style = matrix.at(i, j);
            ASSERT_TRUE(style.is_ok());
            EXPECT_EQ(style.value(), red_style);
        }
    }

    // Проверяем, что остальное не изменилось
    auto style_outside = matrix.at(0, 0);
    ASSERT_TRUE(style_outside.is_ok());
    EXPECT_EQ(style_outside.value(), default_style);
}

TEST_F(StyleMatrixTest, SetArea_SingleCell) {
    StyleMatrix matrix(default_style, 3, 3);

    auto result = matrix.set_area(1, 1, 1, 1, red_style);
    EXPECT_TRUE(result.is_ok());

    auto style = matrix.at(1, 1);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), red_style);
}

TEST_F(StyleMatrixTest, SetArea_FullMatrix) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.set_area(0, 0, 1, 1, red_style);
    EXPECT_TRUE(result.is_ok());

    // Вся матрица должна быть красной
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            auto style = matrix.at(i, j);
            ASSERT_TRUE(style.is_ok());
            EXPECT_EQ(style.value(), red_style);
        }
    }
}

TEST_F(StyleMatrixTest, SetArea_InvalidRange_StartGreaterThanEnd) {
    StyleMatrix matrix(default_style, 3, 3);

    auto result = matrix.set_area(2, 0, 0, 2, red_style);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST_F(StyleMatrixTest, SetArea_OutOfRange) {
    StyleMatrix matrix(default_style, 3, 3);

    auto result = matrix.set_area(0, 0, 10, 10, red_style);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(StyleMatrixTest, SetArea_InvalidStyle) {
    StyleMatrix matrix(default_style, 3, 3);

    CharStyle invalid_style(999, 0, 0, 0, 0, 0, StyleFlags::NONE);
    auto result = matrix.set_area(0, 0, 1, 1, invalid_style);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::VALIDATION_FAILED);
}

TEST_F(StyleMatrixTest, SetArea_MultipleAreas) {
    StyleMatrix matrix(default_style, 5, 5);

    EXPECT_TRUE(matrix.set_area(0, 0, 1, 1, red_style).is_ok());
    EXPECT_TRUE(matrix.set_area(3, 3, 4, 4, green_style).is_ok());

    auto red = matrix.at(0, 0);
    ASSERT_TRUE(red.is_ok());
    EXPECT_EQ(red.value(), red_style);

    auto green = matrix.at(3, 3);
    ASSERT_TRUE(green.is_ok());
    EXPECT_EQ(green.value(), green_style);

    auto default_mid = matrix.at(2, 2);
    ASSERT_TRUE(default_mid.is_ok());
    EXPECT_EQ(default_mid.value(), default_style);
}

// ============================================================================
// Тесты submatrix
// ============================================================================

TEST_F(StyleMatrixTest, Submatrix_Valid) {
    StyleMatrix matrix(default_style, 4, 4);
    matrix.set_at(1, 1, red_style);
    matrix.set_at(1, 2, green_style);
    matrix.set_at(2, 1, blue_style);
    matrix.set_at(2, 2, red_style);

    auto result = matrix.submatrix(1, 1, 2, 2);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 2);
    EXPECT_EQ(sub.cols().value(), 2);

    auto style00 = sub.at(0, 0);
    ASSERT_TRUE(style00.is_ok());
    EXPECT_EQ(style00.value(), red_style);

    auto style01 = sub.at(0, 1);
    ASSERT_TRUE(style01.is_ok());
    EXPECT_EQ(style01.value(), green_style);
}

TEST_F(StyleMatrixTest, Submatrix_SingleElement) {
    StyleMatrix matrix(default_style, 3, 3);
    matrix.set_at(1, 1, red_style);

    auto result = matrix.submatrix(1, 1, 1, 1);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 1);
    EXPECT_EQ(sub.cols().value(), 1);

    auto style = sub.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), red_style);
}

TEST_F(StyleMatrixTest, Submatrix_FullMatrix) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.submatrix(0, 0, 1, 1);
    ASSERT_TRUE(result.is_ok());

    auto sub = result.value();
    EXPECT_EQ(sub.rows(), 2);
    EXPECT_EQ(sub.cols().value(), 2);
}

TEST_F(StyleMatrixTest, Submatrix_InvalidRange) {
    StyleMatrix matrix(default_style, 3, 3);

    auto result = matrix.submatrix(2, 0, 0, 2);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::INVALID_ARGUMENT);
}

TEST_F(StyleMatrixTest, Submatrix_OutOfRange) {
    StyleMatrix matrix(default_style, 2, 2);

    auto result = matrix.submatrix(0, 0, 10, 10);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты set_submatrix
// ============================================================================

TEST_F(StyleMatrixTest, SetSubmatrix_Valid) {
    StyleMatrix matrix(default_style, 4, 4);
    StyleMatrix sub(red_style, 2, 2);

    auto result = matrix.set_submatrix(1, 1, sub);
    EXPECT_TRUE(result.is_ok());

    // Проверяем область
    auto style11 = matrix.at(1, 1);
    ASSERT_TRUE(style11.is_ok());
    EXPECT_EQ(style11.value(), red_style);

    auto style22 = matrix.at(2, 2);
    ASSERT_TRUE(style22.is_ok());
    EXPECT_EQ(style22.value(), red_style);

    // Проверяем, что остальное не изменилось
    auto style00 = matrix.at(0, 0);
    ASSERT_TRUE(style00.is_ok());
    EXPECT_EQ(style00.value(), default_style);
}

TEST_F(StyleMatrixTest, SetSubmatrix_AtCorner) {
    StyleMatrix matrix(default_style, 3, 3);
    StyleMatrix sub(red_style, 2, 2);

    auto result = matrix.set_submatrix(0, 0, sub);
    EXPECT_TRUE(result.is_ok());

    auto style00 = matrix.at(0, 0);
    ASSERT_TRUE(style00.is_ok());
    EXPECT_EQ(style00.value(), red_style);

    auto style11 = matrix.at(1, 1);
    ASSERT_TRUE(style11.is_ok());
    EXPECT_EQ(style11.value(), red_style);

    // Нижний правый угол не должен измениться
    auto style22 = matrix.at(2, 2);
    ASSERT_TRUE(style22.is_ok());
    EXPECT_EQ(style22.value(), default_style);
}

TEST_F(StyleMatrixTest, SetSubmatrix_DoesntFit) {
    StyleMatrix matrix(default_style, 2, 2);
    StyleMatrix sub(red_style, 3, 3);

    auto result = matrix.set_submatrix(0, 0, sub);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

TEST_F(StyleMatrixTest, SetSubmatrix_InvalidPosition) {
    StyleMatrix matrix(default_style, 3, 3);
    StyleMatrix sub(red_style, 1, 1);

    auto result = matrix.set_submatrix(10, 10, sub);
    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
}

// ============================================================================
// Тесты fill_with и clear
// ============================================================================

TEST_F(StyleMatrixTest, FillWith) {
    StyleMatrix matrix(default_style, 3, 3);
    matrix.set_at(0, 0, green_style);
    matrix.set_at(1, 1, blue_style);

    matrix.fill_with(red_style);

    // Все ячейки должны быть красными
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            auto style = matrix.at(i, j);
            ASSERT_TRUE(style.is_ok());
            EXPECT_EQ(style.value(), red_style);
        }
    }
}

TEST_F(StyleMatrixTest, Clear) {
    StyleMatrix matrix(default_style, 3, 3);
    matrix.set_at(0, 0, red_style);
    matrix.set_at(1, 1, green_style);
    matrix.set_at(2, 2, blue_style);

    matrix.clear();

    // Все ячейки должны вернуться к default_style
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            auto style = matrix.at(i, j);
            ASSERT_TRUE(style.is_ok());
            EXPECT_EQ(style.value(), default_style);
        }
    }
}

TEST_F(StyleMatrixTest, Clear_PreservesBasicStyle) {
    CharStyle custom_basic = CharStyle(Color::MAGENTA, Color::CYAN);
    StyleMatrix matrix(custom_basic, 2, 2);

    matrix.set_at(0, 0, red_style);
    matrix.clear();

    auto style = matrix.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), custom_basic);
}

// ============================================================================
// Интеграционные тесты
// ============================================================================

TEST_F(StyleMatrixTest, Integration_ComplexScenario) {
    StyleMatrix matrix(default_style, 5, 5);

    // Устанавливаем строки разными стилями
    EXPECT_TRUE(matrix.set_row(0, red_style).is_ok());
    EXPECT_TRUE(matrix.set_row(1, green_style).is_ok());
    EXPECT_TRUE(matrix.set_row(2, blue_style).is_ok());

    // Устанавливаем область
    EXPECT_TRUE(matrix.set_area(3, 0, 4, 2, red_style).is_ok());

    // Получаем субматрицу
    auto sub_result = matrix.submatrix(1, 1, 3, 3);
    ASSERT_TRUE(sub_result.is_ok());
    auto sub = sub_result.value();

    EXPECT_EQ(sub.rows(), 3);
    EXPECT_EQ(sub.cols().value(), 3);

    // Модифицируем оригинал
    EXPECT_TRUE(matrix.set_at(2, 2, red_style).is_ok());

    // Субматрица не должна измениться (это копия)
    auto sub_style = sub.at(1, 1);
    ASSERT_TRUE(sub_style.is_ok());
    EXPECT_EQ(sub_style.value(), blue_style);

    // Проверяем оригинал
    auto orig_style = matrix.at(2, 2);
    ASSERT_TRUE(orig_style.is_ok());
    EXPECT_EQ(orig_style.value(), red_style);
}

TEST_F(StyleMatrixTest, Integration_CopyAndModify) {
    StyleMatrix original(default_style, 3, 3);
    original.set_at(0, 0, red_style);
    original.set_at(1, 1, green_style);

    StyleMatrix copy = original;

    // Модифицируем копию
    EXPECT_TRUE(copy.set_at(0, 0, blue_style).is_ok());

    // Оригинал не должен измениться
    auto orig_style = original.at(0, 0);
    ASSERT_TRUE(orig_style.is_ok());
    EXPECT_EQ(orig_style.value(), red_style);

    // Копия изменилась
    auto copy_style = copy.at(0, 0);
    ASSERT_TRUE(copy_style.is_ok());
    EXPECT_EQ(copy_style.value(), blue_style);
}

TEST_F(StyleMatrixTest, Integration_MultipleLayers) {
    StyleMatrix matrix(default_style, 10, 10);

    // Слой 1: заполняем всё красным
    matrix.fill_with(red_style);

    // Слой 2: зелёная рамка
    EXPECT_TRUE(matrix.set_row(0, green_style).is_ok());
    EXPECT_TRUE(matrix.set_row(9, green_style).is_ok());
    EXPECT_TRUE(matrix.set_area(0, 0, 9, 0, green_style).is_ok());
    EXPECT_TRUE(matrix.set_area(0, 9, 9, 9, green_style).is_ok());

    // Слой 3: синий центр
    EXPECT_TRUE(matrix.set_area(4, 4, 5, 5, blue_style).is_ok());

    // Проверяем структуру
    auto corner = matrix.at(0, 0);
    ASSERT_TRUE(corner.is_ok());
    EXPECT_EQ(corner.value(), green_style);

    auto center = matrix.at(4, 4);
    ASSERT_TRUE(center.is_ok());
    EXPECT_EQ(center.value(), blue_style);

    auto middle = matrix.at(2, 2);
    ASSERT_TRUE(middle.is_ok());
    EXPECT_EQ(middle.value(), red_style);
}

// ============================================================================
// Граничные случаи
// ============================================================================

TEST_F(StyleMatrixTest, EdgeCase_SingleElementMatrix) {
    StyleMatrix matrix(red_style, 1, 1);

    EXPECT_EQ(matrix.rows(), 1);
    EXPECT_EQ(matrix.cols().value(), 1);

    auto style = matrix.at(0, 0);
    ASSERT_TRUE(style.is_ok());
    EXPECT_EQ(style.value(), red_style);
}

TEST_F(StyleMatrixTest, EdgeCase_LargeMatrix) {
    StyleMatrix matrix(default_style, 100, 100);

    EXPECT_EQ(matrix.rows(), 100);
    EXPECT_EQ(matrix.cols().value(), 100);

    auto corner = matrix.at(99, 99);
    ASSERT_TRUE(corner.is_ok());
    EXPECT_EQ(corner.value(), default_style);
}

TEST_F(StyleMatrixTest, EdgeCase_RectangularMatrix) {
    StyleMatrix matrix(default_style, 2, 10);

    EXPECT_EQ(matrix.rows(), 2);
    EXPECT_EQ(matrix.cols().value(), 10);

    EXPECT_TRUE(matrix.set_row(0, red_style).is_ok());

    auto style_first_row = matrix.at(0, 5);
    ASSERT_TRUE(style_first_row.is_ok());
    EXPECT_EQ(style_first_row.value(), red_style);
}

TEST_F(StyleMatrixTest, EdgeCase_AllStyleTypes) {
    StyleMatrix matrix(default_style, 3, 3);

    // Базовый цвет
    EXPECT_TRUE(matrix.set_at(0, 0, red_style).is_ok());

    // RGB
    EXPECT_TRUE(matrix.set_at(0, 1, rgb_style).is_ok());

    // ANSI8
    EXPECT_TRUE(matrix.set_at(0, 2, ansi_style).is_ok());

    auto red = matrix.at(0, 0);
    ASSERT_TRUE(red.is_ok());
    EXPECT_EQ(red.value(), red_style);

    auto rgb = matrix.at(0, 1);
    ASSERT_TRUE(rgb.is_ok());
    EXPECT_EQ(rgb.value(), rgb_style);

    auto ansi = matrix.at(0, 2);
    ASSERT_TRUE(ansi.is_ok());
    EXPECT_EQ(ansi.value(), ansi_style);
}