#include <gtest/gtest.h>
#include <type_traits>
#include "../tui/StyleStack.h"

using namespace dungeons;
using namespace dungeons::tui;

class StyleStackTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем тестовые стили
        style1 = CharStyle(Color::RED, Color::BLACK);
        style2 = CharStyle(Color::GREEN, Color::WHITE);
        style3 = CharStyle(Color::BLUE, Color::YELLOW);
    }

    CharStyle style1;
    CharStyle style2;
    CharStyle style3;
};

// ============================================================================
// Тесты конструктора
// ============================================================================

TEST_F(StyleStackTest, DefaultConstructor) {
    StyleStack stack;
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
}

// ============================================================================
// Тесты копирования и перемещения (должны быть удалены)
// ============================================================================

TEST_F(StyleStackTest, CopyConstructorDeleted) {
    static_assert(!std::is_copy_constructible_v<StyleStack>,
        "StyleStack should not be copy constructible");
}

TEST_F(StyleStackTest, CopyAssignmentDeleted) {
    static_assert(!std::is_copy_assignable_v<StyleStack>,
        "StyleStack should not be copy assignable");
}

TEST_F(StyleStackTest, MoveConstructorDeleted) {
    static_assert(!std::is_move_constructible_v<StyleStack>,
        "StyleStack should not be move constructible");
}

TEST_F(StyleStackTest, MoveAssignmentDeleted) {
    static_assert(!std::is_move_assignable_v<StyleStack>,
        "StyleStack should not be move assignable");
}

// ============================================================================
// Тесты empty() и size()
// ============================================================================

TEST_F(StyleStackTest, Empty_InitiallyTrue) {
    StyleStack stack;
    EXPECT_TRUE(stack.empty());
}

TEST_F(StyleStackTest, Empty_FalseAfterPush) {
    StyleStack stack;
    stack.push(style1);
    EXPECT_FALSE(stack.empty());
}

TEST_F(StyleStackTest, Empty_TrueAfterPushAndPop) {
    StyleStack stack;
    stack.push(style1);
    auto result = stack.pop();
    EXPECT_TRUE(result.is_ok());
    EXPECT_TRUE(stack.empty());
}

TEST_F(StyleStackTest, Size_InitiallyZero) {
    StyleStack stack;
    EXPECT_EQ(stack.size(), 0);
}

TEST_F(StyleStackTest, Size_IncreasesWithPush) {
    StyleStack stack;
    EXPECT_EQ(stack.size(), 0);

    stack.push(style1);
    EXPECT_EQ(stack.size(), 1);

    stack.push(style2);
    EXPECT_EQ(stack.size(), 2);

    stack.push(style3);
    EXPECT_EQ(stack.size(), 3);
}

TEST_F(StyleStackTest, Size_DecreasesWithPop) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);
    stack.push(style3);
    EXPECT_EQ(stack.size(), 3);

    stack.pop();
    EXPECT_EQ(stack.size(), 2);

    stack.pop();
    EXPECT_EQ(stack.size(), 1);

    stack.pop();
    EXPECT_EQ(stack.size(), 0);
}

// ============================================================================
// Тесты push()
// ============================================================================

TEST_F(StyleStackTest, Push_SingleElement) {
    StyleStack stack;
    stack.push(style1);

    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 1);
}

TEST_F(StyleStackTest, Push_MultipleElements) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);
    stack.push(style3);

    EXPECT_EQ(stack.size(), 3);
}

TEST_F(StyleStackTest, Push_OrderPreserved) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);

    // peek() должен вернуть последний добавленный (style2)
    auto result = stack.peek();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), style2);
}

// ============================================================================
// Тесты peek()
// ============================================================================

TEST_F(StyleStackTest, Peek_EmptyStack) {
    StyleStack stack;
    auto result = stack.peek();

    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
    EXPECT_EQ(result.error().message(), "Style stack was empty.");
}

TEST_F(StyleStackTest, Peek_SingleElement) {
    StyleStack stack;
    stack.push(style1);

    auto result = stack.peek();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), style1);

    // Проверяем, что peek не удаляет элемент
    EXPECT_EQ(stack.size(), 1);
}

TEST_F(StyleStackTest, Peek_MultipleElements) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);
    stack.push(style3);

    // peek() возвращает элемент из front (последний добавленный)
    auto result = stack.peek();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), style3);

    // Размер не изменился
    EXPECT_EQ(stack.size(), 3);
}

TEST_F(StyleStackTest, Peek_DoesNotModifyStack) {
    StyleStack stack;
    stack.push(style1);

    auto result1 = stack.peek();
    auto result2 = stack.peek();

    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    EXPECT_EQ(result1.value(), result2.value());
    EXPECT_EQ(stack.size(), 1);
}

TEST_F(StyleStackTest, Peek_AfterMultiplePushes) {
    StyleStack stack;

    stack.push(style1);
    auto result1 = stack.peek();
    ASSERT_TRUE(result1.is_ok());
    EXPECT_EQ(result1.value(), style1);

    stack.push(style2);
    auto result2 = stack.peek();
    ASSERT_TRUE(result2.is_ok());
    EXPECT_EQ(result2.value(), style2);

    stack.push(style3);
    auto result3 = stack.peek();
    ASSERT_TRUE(result3.is_ok());
    EXPECT_EQ(result3.value(), style3);
}

// ============================================================================
// Тесты peek_basic()
// ============================================================================

TEST_F(StyleStackTest, PeekBasic_EmptyStack) {
    StyleStack stack;
    auto result = stack.peek_basic();

    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
    EXPECT_EQ(result.error().message(), "Style stack was empty.");
}

TEST_F(StyleStackTest, PeekBasic_SingleElement) {
    StyleStack stack;
    stack.push(style1);

    auto result = stack.peek_basic();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), style1);

    // Проверяем, что peek_basic не удаляет элемент
    EXPECT_EQ(stack.size(), 1);
}

TEST_F(StyleStackTest, PeekBasic_MultipleElements) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);
    stack.push(style3);

    // peek_basic() возвращает элемент из back (первый добавленный)
    auto result = stack.peek_basic();
    ASSERT_TRUE(result.is_ok());
    EXPECT_EQ(result.value(), style1);

    // Размер не изменился
    EXPECT_EQ(stack.size(), 3);
}

TEST_F(StyleStackTest, PeekBasic_DoesNotModifyStack) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);

    auto result1 = stack.peek_basic();
    auto result2 = stack.peek_basic();

    ASSERT_TRUE(result1.is_ok());
    ASSERT_TRUE(result2.is_ok());
    EXPECT_EQ(result1.value(), result2.value());
    EXPECT_EQ(stack.size(), 2);
}

// ============================================================================
// Тесты различия между peek() и peek_basic()
// ============================================================================

TEST_F(StyleStackTest, Peek_vs_PeekBasic_Difference) {
    StyleStack stack;
    stack.push(style1);  // back
    stack.push(style2);
    stack.push(style3);  // front

    auto peek_result = stack.peek();
    auto peek_basic_result = stack.peek_basic();

    ASSERT_TRUE(peek_result.is_ok());
    ASSERT_TRUE(peek_basic_result.is_ok());

    // peek() возвращает front (последний добавленный)
    EXPECT_EQ(peek_result.value(), style3);

    // peek_basic() возвращает back (первый добавленный)
    EXPECT_EQ(peek_basic_result.value(), style1);
}

// ============================================================================
// Тесты pop()
// ============================================================================

TEST_F(StyleStackTest, Pop_EmptyStack) {
    StyleStack stack;
    auto result = stack.pop();

    EXPECT_TRUE(result.is_error());
    EXPECT_EQ(result.error().code(), ErrorCode::OUT_OF_RANGE);
    EXPECT_EQ(result.error().message(), "Nothing to pop.");
}

TEST_F(StyleStackTest, Pop_SingleElement) {
    StyleStack stack;
    stack.push(style1);

    auto result = stack.pop();
    EXPECT_TRUE(result.is_ok());
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
}

TEST_F(StyleStackTest, Pop_MultipleElements) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);
    stack.push(style3);

    auto result1 = stack.pop();
    EXPECT_TRUE(result1.is_ok());
    EXPECT_EQ(stack.size(), 2);

    auto result2 = stack.pop();
    EXPECT_TRUE(result2.is_ok());
    EXPECT_EQ(stack.size(), 1);

    auto result3 = stack.pop();
    EXPECT_TRUE(result3.is_ok());
    EXPECT_EQ(stack.size(), 0);
    EXPECT_TRUE(stack.empty());
}

TEST_F(StyleStackTest, Pop_LIFO_Order) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);
    stack.push(style3);

    // Проверяем LIFO порядок
    auto peek1 = stack.peek();
    ASSERT_TRUE(peek1.is_ok());
    EXPECT_EQ(peek1.value(), style3);

    stack.pop();
    auto peek2 = stack.peek();
    ASSERT_TRUE(peek2.is_ok());
    EXPECT_EQ(peek2.value(), style2);

    stack.pop();
    auto peek3 = stack.peek();
    ASSERT_TRUE(peek3.is_ok());
    EXPECT_EQ(peek3.value(), style1);
}

TEST_F(StyleStackTest, Pop_UntilEmpty) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);

    EXPECT_TRUE(stack.pop().is_ok());
    EXPECT_TRUE(stack.pop().is_ok());

    // Следующий pop должен вернуть ошибку
    auto result = stack.pop();
    EXPECT_TRUE(result.is_error());
}

TEST_F(StyleStackTest, Pop_AfterPopError) {
    StyleStack stack;

    // Первый pop на пустом стеке
    auto result1 = stack.pop();
    EXPECT_TRUE(result1.is_error());

    // Добавляем элемент
    stack.push(style1);

    // Теперь pop должен работать
    auto result2 = stack.pop();
    EXPECT_TRUE(result2.is_ok());
}

// ============================================================================
// Интеграционные тесты - сложные сценарии
// ============================================================================

TEST_F(StyleStackTest, Integration_PushPopSequence) {
    StyleStack stack;

    stack.push(style1);
    EXPECT_EQ(stack.size(), 1);

    stack.push(style2);
    EXPECT_EQ(stack.size(), 2);

    auto pop_result = stack.pop();
    EXPECT_TRUE(pop_result.is_ok());
    EXPECT_EQ(stack.size(), 1);

    auto peek_result = stack.peek();
    ASSERT_TRUE(peek_result.is_ok());
    EXPECT_EQ(peek_result.value(), style1);
}

TEST_F(StyleStackTest, Integration_MultiplePushPopCycles) {
    StyleStack stack;

    // Цикл 1
    stack.push(style1);
    stack.push(style2);
    EXPECT_EQ(stack.size(), 2);

    stack.pop();
    stack.pop();
    EXPECT_TRUE(stack.empty());

    // Цикл 2
    stack.push(style3);
    EXPECT_EQ(stack.size(), 1);

    auto peek_result = stack.peek();
    ASSERT_TRUE(peek_result.is_ok());
    EXPECT_EQ(peek_result.value(), style3);
}

TEST_F(StyleStackTest, Integration_PeekWithoutPop) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);

    // Множественные peek не должны изменять стек
    for (int i = 0; i < 10; ++i) {
        auto result = stack.peek();
        ASSERT_TRUE(result.is_ok());
        EXPECT_EQ(result.value(), style2);
        EXPECT_EQ(stack.size(), 2);
    }
}

TEST_F(StyleStackTest, Integration_AlternatingPeekTypes) {
    StyleStack stack;
    stack.push(style1);
    stack.push(style2);
    stack.push(style3);

    // Чередование peek и peek_basic
    auto peek1 = stack.peek();
    auto peek_basic1 = stack.peek_basic();
    auto peek2 = stack.peek();
    auto peek_basic2 = stack.peek_basic();

    ASSERT_TRUE(peek1.is_ok());
    ASSERT_TRUE(peek_basic1.is_ok());
    ASSERT_TRUE(peek2.is_ok());
    ASSERT_TRUE(peek_basic2.is_ok());

    // Результаты должны быть согласованными
    EXPECT_EQ(peek1.value(), peek2.value());
    EXPECT_EQ(peek_basic1.value(), peek_basic2.value());

    // И размер не должен измениться
    EXPECT_EQ(stack.size(), 3);
}

TEST_F(StyleStackTest, Integration_LargeStack) {
    StyleStack stack;
    const size_t COUNT = 1000;

    // Добавляем много элементов
    for (size_t i = 0; i < COUNT; ++i) {
        stack.push(style1);
    }

    EXPECT_EQ(stack.size(), COUNT);
    EXPECT_FALSE(stack.empty());

    // Удаляем половину
    for (size_t i = 0; i < COUNT / 2; ++i) {
        auto result = stack.pop();
        EXPECT_TRUE(result.is_ok());
    }

    EXPECT_EQ(stack.size(), COUNT / 2);
}

TEST_F(StyleStackTest, Integration_ErrorRecovery) {
    StyleStack stack;

    // Пытаемся pop на пустом стеке
    auto pop_result1 = stack.pop();
    EXPECT_TRUE(pop_result1.is_error());

    // Пытаемся peek на пустом стеке
    auto peek_result1 = stack.peek();
    EXPECT_TRUE(peek_result1.is_error());

    // Добавляем элемент и проверяем восстановление
    stack.push(style1);

    auto peek_result2 = stack.peek();
    EXPECT_TRUE(peek_result2.is_ok());

    auto pop_result2 = stack.pop();
    EXPECT_TRUE(pop_result2.is_ok());
}

// ============================================================================
// Граничные случаи
// ============================================================================

TEST_F(StyleStackTest, EdgeCase_PeekAfterPop) {
    StyleStack stack;
    stack.push(style1);

    stack.pop();
    auto result = stack.peek();

    EXPECT_TRUE(result.is_error());
}

TEST_F(StyleStackTest, EdgeCase_PeekBasicAfterPop) {
    StyleStack stack;
    stack.push(style1);

    stack.pop();
    auto result = stack.peek_basic();

    EXPECT_TRUE(result.is_error());
}

TEST_F(StyleStackTest, EdgeCase_SingleElementBothPeeks) {
    StyleStack stack;
    stack.push(style1);

    auto peek_result = stack.peek();
    auto peek_basic_result = stack.peek_basic();

    ASSERT_TRUE(peek_result.is_ok());
    ASSERT_TRUE(peek_basic_result.is_ok());

    // Для одного элемента оба должны вернуть одинаковый результат
    EXPECT_EQ(peek_result.value(), peek_basic_result.value());
}

TEST_F(StyleStackTest, EdgeCase_PushSameStyleMultipleTimes) {
    StyleStack stack;

    stack.push(style1);
    stack.push(style1);
    stack.push(style1);

    EXPECT_EQ(stack.size(), 3);

    auto peek_result = stack.peek();
    ASSERT_TRUE(peek_result.is_ok());
    EXPECT_EQ(peek_result.value(), style1);
}

TEST_F(StyleStackTest, EdgeCase_EmptyAfterOperations) {
    StyleStack stack;

    // Выполняем различные операции
    stack.push(style1);
    stack.push(style2);
    stack.pop();
    stack.push(style3);
    stack.pop();
    stack.pop();

    // Стек должен быть пустым
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
}

// ============================================================================
// Тесты состояния стека
// ============================================================================

TEST_F(StyleStackTest, State_ConsistencyAfterOperations) {
    StyleStack stack;

    // Начальное состояние
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);

    // После push
    stack.push(style1);
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 1);

    // После peek (не должно измениться)
    stack.peek();
    EXPECT_FALSE(stack.empty());
    EXPECT_EQ(stack.size(), 1);

    // После pop
    stack.pop();
    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.size(), 0);
}

TEST_F(StyleStackTest, State_SizeMatchesActualElements) {
    StyleStack stack;

    for (size_t i = 1; i <= 5; ++i) {
        stack.push(style1);
        EXPECT_EQ(stack.size(), i);
    }

    for (size_t i = 5; i > 0; --i) {
        EXPECT_EQ(stack.size(), i);
        stack.pop();
    }

    EXPECT_EQ(stack.size(), 0);
}