// money_test.cpp
#include <gtest/gtest.h>
#include "../backend/Money.h"

using namespace dungeons;
using namespace dungeons::backend;

// ========== Конструкторы ==========

TEST(MoneyTest, DefaultConstructor) {
    Money m;
    EXPECT_EQ(m.amount(), 0);
}

TEST(MoneyTest, ParameterizedConstructor) {
    Money m(100);
    EXPECT_EQ(m.amount(), 100);
}

TEST(MoneyTest, ParameterizedConstructorWithZero) {
    Money m(0);
    EXPECT_EQ(m.amount(), 0);
}

TEST(MoneyTest, ParameterizedConstructorWithLargeValue) {
    Money m(9223372036854775807LL); // INT64_MAX
    EXPECT_EQ(m.amount(), 9223372036854775807LL);
}

// ========== Правило пяти ==========

TEST(MoneyTest, CopyConstructor) {
    Money m1(100);
    Money m2(m1);
    EXPECT_EQ(m2.amount(), 100);
    EXPECT_EQ(m1.amount(), 100); // Оригинал не изменился
}

TEST(MoneyTest, MoveConstructor) {
    Money m1(100);
    Money m2(std::move(m1));
    EXPECT_EQ(m2.amount(), 100);
}

TEST(MoneyTest, CopyAssignment) {
    Money m1(100);
    Money m2;
    m2 = m1;
    EXPECT_EQ(m2.amount(), 100);
    EXPECT_EQ(m1.amount(), 100); // Оригинал не изменился
}

TEST(MoneyTest, MoveAssignment) {
    Money m1(100);
    Money m2;
    m2 = std::move(m1);
    EXPECT_EQ(m2.amount(), 100);
}

TEST(MoneyTest, SelfAssignment) {
    Money m1(100);
    m1 = m1;
    EXPECT_EQ(m1.amount(), 100);
}

// ========== Геттер/Сеттер ==========

TEST(MoneyTest, SetAmountValid) {
    Money m;
    auto result = m.amount(500);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 500);
}

TEST(MoneyTest, SetAmountZero) {
    Money m(100);
    auto result = m.amount(0);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 0);
}

TEST(MoneyTest, SetAmountNegativeRejected) {
    Money m(100);
    auto result = m.amount(-50);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().code(), ::dungeons::ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(result.error().message(), "Money amount cannot be negative");
    EXPECT_EQ(m.amount(), 100); // Значение не изменилось
}

TEST(MoneyTest, SetAmountLargeValue) {
    Money m;
    auto result = m.amount(9223372036854775807LL);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 9223372036854775807LL);
}

// ========== Валидация ==========

TEST(MoneyTest, ValidateValidAmount) {
    Money m(100);
    auto result = m.validate();
    EXPECT_TRUE(result);
}

TEST(MoneyTest, ValidateZero) {
    Money m(0);
    auto result = m.validate();
    EXPECT_TRUE(result);
}

TEST(MoneyTest, ValidateNegativeAmount) {
    Money m(-50); // Конструктор не проверяет
    auto result = m.validate();
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().code(), ::dungeons::ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(result.error().message(), "Money amount is negative");
}

// ========== Операции add(int64_t) ==========

TEST(MoneyTest, AddPositiveAmount) {
    Money m(100);
    auto result = m.add(50);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 150);
}

TEST(MoneyTest, AddZero) {
    Money m(100);
    auto result = m.add(0);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 100);
}

TEST(MoneyTest, AddToZero) {
    Money m(0);
    auto result = m.add(100);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 100);
}

TEST(MoneyTest, AddNegativeRejected) {
    Money m(100);
    auto result = m.add(-50);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().code(), ::dungeons::ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(result.error().message(), "Cannot add negative amount");
    EXPECT_EQ(m.amount(), 100); // Значение не изменилось
}

TEST(MoneyTest, AddMultipleTimes) {
    Money m(100);
    m.add(50);
    m.add(25);
    m.add(10);
    EXPECT_EQ(m.amount(), 185);
}

// ========== Операции subtract(int64_t) ==========

TEST(MoneyTest, SubtractValidAmount) {
    Money m(100);
    auto result = m.subtract(30);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 70);
}

TEST(MoneyTest, SubtractZero) {
    Money m(100);
    auto result = m.subtract(0);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 100);
}

TEST(MoneyTest, SubtractToZero) {
    Money m(100);
    auto result = m.subtract(100);
    EXPECT_TRUE(result);
    EXPECT_EQ(m.amount(), 0);
}

TEST(MoneyTest, SubtractInsufficientFunds) {
    Money m(100);
    auto result = m.subtract(150);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().code(), ::dungeons::ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(result.error().message(), "Insufficient funds");
    EXPECT_EQ(m.amount(), 100); // Значение не изменилось
}

TEST(MoneyTest, SubtractNegativeRejected) {
    Money m(100);
    auto result = m.subtract(-50);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().code(), ::dungeons::ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(result.error().message(), "Cannot subtract negative amount");
    EXPECT_EQ(m.amount(), 100); // Значение не изменилось
}

TEST(MoneyTest, SubtractFromZero) {
    Money m(0);
    auto result = m.subtract(10);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().code(), ::dungeons::ErrorCode::VALIDATION_FAILED);
}

TEST(MoneyTest, SubtractMultipleTimes) {
    Money m(100);
    m.subtract(20);
    m.subtract(30);
    m.subtract(10);
    EXPECT_EQ(m.amount(), 40);
}

// ========== Операции add(Money) ==========

TEST(MoneyTest, AddMoney) {
    Money m1(100);
    Money m2(50);
    auto result = m1.add(m2);
    EXPECT_TRUE(result);
    EXPECT_EQ(m1.amount(), 150);
    EXPECT_EQ(result.value().amount(), 150);
}

TEST(MoneyTest, AddMoneyZero) {
    Money m1(100);
    Money m2(0);
    auto result = m1.add(m2);
    EXPECT_TRUE(result);
    EXPECT_EQ(m1.amount(), 100);
}

TEST(MoneyTest, AddMoneyToZero) {
    Money m1(0);
    Money m2(100);
    auto result = m1.add(m2);
    EXPECT_TRUE(result);
    EXPECT_EQ(m1.amount(), 100);
}

// ========== Операции subtract(Money) ==========

TEST(MoneyTest, SubtractMoney) {
    Money m1(100);
    Money m2(30);
    auto result = m1.subtract(m2);
    EXPECT_TRUE(result);
    EXPECT_EQ(m1.amount(), 70);
    EXPECT_EQ(result.value().amount(), 70);
}

TEST(MoneyTest, SubtractMoneyZero) {
    Money m1(100);
    Money m2(0);
    auto result = m1.subtract(m2);
    EXPECT_TRUE(result);
    EXPECT_EQ(m1.amount(), 100);
}

TEST(MoneyTest, SubtractMoneyInsufficientFunds) {
    Money m1(100);
    Money m2(150);
    auto result = m1.subtract(m2);
    EXPECT_FALSE(result);
    EXPECT_EQ(result.error().code(), ::dungeons::ErrorCode::VALIDATION_FAILED);
    EXPECT_EQ(m1.amount(), 100); // Значение не изменилось
}

// ========== Операторы сравнения ==========

TEST(MoneyTest, EqualityOperator) {
    Money m1(100);
    Money m2(100);
    Money m3(150);

    EXPECT_TRUE(m1 == m2);
    EXPECT_FALSE(m1 == m3);
}

TEST(MoneyTest, InequalityOperator) {
    Money m1(100);
    Money m2(150);
    Money m3(100);

    EXPECT_TRUE(m1 != m2);
    EXPECT_FALSE(m1 != m3);
}

TEST(MoneyTest, LessThanOperator) {
    Money m1(100);
    Money m2(150);
    Money m3(100);

    EXPECT_TRUE(m1 < m2);
    EXPECT_FALSE(m2 < m1);
    EXPECT_FALSE(m1 < m3);
}

TEST(MoneyTest, LessThanOrEqualOperator) {
    Money m1(100);
    Money m2(150);
    Money m3(100);

    EXPECT_TRUE(m1 <= m2);
    EXPECT_TRUE(m1 <= m3);
    EXPECT_FALSE(m2 <= m1);
}

TEST(MoneyTest, GreaterThanOperator) {
    Money m1(150);
    Money m2(100);
    Money m3(150);

    EXPECT_TRUE(m1 > m2);
    EXPECT_FALSE(m2 > m1);
    EXPECT_FALSE(m1 > m3);
}

TEST(MoneyTest, GreaterThanOrEqualOperator) {
    Money m1(150);
    Money m2(100);
    Money m3(150);

    EXPECT_TRUE(m1 >= m2);
    EXPECT_TRUE(m1 >= m3);
    EXPECT_FALSE(m2 >= m1);
}

TEST(MoneyTest, CompareWithZero) {
    Money m1(100);
    Money m2(0);

    EXPECT_TRUE(m1 > m2);
    EXPECT_TRUE(m2 < m1);
    EXPECT_FALSE(m1 == m2);
}

// ========== Арифметические операторы ==========

TEST(MoneyTest, AdditionOperator) {
    Money m1(100);
    Money m2(50);

    Money m3 = m1 + m2;

    EXPECT_EQ(m3.amount(), 150);
    EXPECT_EQ(m1.amount(), 100); // m1 не изменился
    EXPECT_EQ(m2.amount(), 50);  // m2 не изменился
}

TEST(MoneyTest, AdditionOperatorWithZero) {
    Money m1(100);
    Money m2(0);

    Money m3 = m1 + m2;
    EXPECT_EQ(m3.amount(), 100);
}

TEST(MoneyTest, SubtractionOperator) {
    Money m1(100);
    Money m2(30);

    Money m3 = m1 - m2;

    EXPECT_EQ(m3.amount(), 70);
    EXPECT_EQ(m1.amount(), 100); // m1 не изменился
    EXPECT_EQ(m2.amount(), 30);  // m2 не изменился
}

TEST(MoneyTest, SubtractionOperatorWithZero) {
    Money m1(100);
    Money m2(0);

    Money m3 = m1 - m2;
    EXPECT_EQ(m3.amount(), 100);
}

TEST(MoneyTest, SubtractionOperatorInsufficientFunds) {
    Money m1(100);
    Money m2(150);

    Money m3 = m1 - m2;

    // При недостатке средств возвращается копия m1
    EXPECT_EQ(m3.amount(), 100);
    EXPECT_EQ(m1.amount(), 100);
}

TEST(MoneyTest, ChainedAddition) {
    Money m1(100);
    Money m2(50);
    Money m3(25);

    Money m4 = m1 + m2 + m3;
    EXPECT_EQ(m4.amount(), 175);
}

TEST(MoneyTest, ChainedSubtraction) {
    Money m1(100);
    Money m2(20);
    Money m3(30);

    Money m4 = m1 - m2 - m3;
    EXPECT_EQ(m4.amount(), 50);
}

TEST(MoneyTest, MixedOperations) {
    Money m1(100);
    Money m2(50);
    Money m3(20);

    Money m4 = (m1 + m2) - m3;
    EXPECT_EQ(m4.amount(), 130);
}

// ========== Составные операторы присваивания ==========

TEST(MoneyTest, CompoundAdditionOperator) {
    Money m1(100);
    Money m2(50);

    m1 += m2;

    EXPECT_EQ(m1.amount(), 150);
    EXPECT_EQ(m2.amount(), 50); // m2 не изменился
}

TEST(MoneyTest, CompoundAdditionOperatorWithZero) {
    Money m1(100);
    Money m2(0);

    m1 += m2;
    EXPECT_EQ(m1.amount(), 100);
}

TEST(MoneyTest, CompoundAdditionOperatorMultipleTimes) {
    Money m1(100);
    Money m2(20);
    Money m3(30);

    m1 += m2;
    m1 += m3;

    EXPECT_EQ(m1.amount(), 150);
}

TEST(MoneyTest, CompoundSubtractionOperator) {
    Money m1(100);
    Money m2(30);

    m1 -= m2;

    EXPECT_EQ(m1.amount(), 70);
    EXPECT_EQ(m2.amount(), 30); // m2 не изменился
}

TEST(MoneyTest, CompoundSubtractionOperatorWithZero) {
    Money m1(100);
    Money m2(0);

    m1 -= m2;
    EXPECT_EQ(m1.amount(), 100);
}

TEST(MoneyTest, CompoundSubtractionOperatorInsufficientFunds) {
    Money m1(100);
    Money m2(150);

    m1 -= m2;

    // При недостатке средств значение не меняется
    EXPECT_EQ(m1.amount(), 100);
}

TEST(MoneyTest, CompoundSubtractionOperatorMultipleTimes) {
    Money m1(100);
    Money m2(20);
    Money m3(30);

    m1 -= m2;
    m1 -= m3;

    EXPECT_EQ(m1.amount(), 50);
}

TEST(MoneyTest, MixedCompoundOperators) {
    Money m1(100);
    Money m2(50);
    Money m3(20);

    m1 += m2;  // 150
    m1 -= m3;  // 130

    EXPECT_EQ(m1.amount(), 130);
}

// ========== Граничные случаи ==========

TEST(MoneyTest, MaxValue) {
    Money m(9223372036854775807LL); // INT64_MAX
    EXPECT_EQ(m.amount(), 9223372036854775807LL);
    EXPECT_TRUE(m.validate());
}

TEST(MoneyTest, MinValue) {
    Money m(0);
    EXPECT_EQ(m.amount(), 0);
    EXPECT_TRUE(m.validate());
}

TEST(MoneyTest, NegativeValueThroughConstructor) {
    Money m(-100);
    EXPECT_EQ(m.amount(), -100);
    EXPECT_FALSE(m.validate()); // Валидация должна провалиться
}

TEST(MoneyTest, MultipleOperationsPreservingInvariants) {
    Money m(1000);

    m.add(500);     // 1500
    m.subtract(200); // 1300
    m.add(100);     // 1400
    m.subtract(400); // 1000

    EXPECT_EQ(m.amount(), 1000);
    EXPECT_TRUE(m.validate());
}

// ========== Практические сценарии ==========

TEST(MoneyTest, TransactionScenario) {
    Money wallet(1000);
    Money item_price(150);

    // Проверить достаточно ли денег
    EXPECT_TRUE(wallet.amount() >= item_price.amount());

    // Купить предмет
    auto result = wallet.subtract(item_price);
    EXPECT_TRUE(result);
    EXPECT_EQ(wallet.amount(), 850);
}

TEST(MoneyTest, InsufficientFundsScenario) {
    Money wallet(100);
    Money item_price(150);

    // Попытка купить слишком дорогой предмет
    auto result = wallet.subtract(item_price);
    EXPECT_FALSE(result);
    EXPECT_EQ(wallet.amount(), 100); // Деньги не потрачены
}

TEST(MoneyTest, RewardScenario) {
    Money player_money(500);
    Money quest_reward(250);

    player_money += quest_reward;
    EXPECT_EQ(player_money.amount(), 750);
}

TEST(MoneyTest, SplitMoneyScenario) {
    Money total(1000);
    Money share(250);

    total -= share; // Первый игрок
    EXPECT_EQ(total.amount(), 750);

    total -= share; // Второй игрок
    EXPECT_EQ(total.amount(), 500);

    total -= share; // Третий игрок
    EXPECT_EQ(total.amount(), 250);

    total -= share; // Четвертый игрок
    EXPECT_EQ(total.amount(), 0);
}