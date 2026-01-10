#include "../Random.h"
#include <gtest/gtest.h>
#include <vector>
#include <set>
#include <map>
#include <thread>
#include <algorithm>
#include <cmath>
#include <numeric>

using namespace dungeons;

// ==================== Тесты целочисленных типов ====================

TEST(RandomIntTest, NextIntReturnsValueInRange) {
    for (int i = 0; i < 100; ++i) {
        int value = Random::next_int(10, 20);
        EXPECT_GE(value, 10);
        EXPECT_LE(value, 20);
    }
}

TEST(RandomIntTest, NextIntSingleValue) {
    for (int i = 0; i < 10; ++i) {
        int value = Random::next_int(42, 42);
        EXPECT_EQ(value, 42);
    }
}

TEST(RandomIntTest, NextIntNegativeRange) {
    for (int i = 0; i < 100; ++i) {
        int value = Random::next_int(-50, -10);
        EXPECT_GE(value, -50);
        EXPECT_LE(value, -10);
    }
}

TEST(RandomIntTest, NextIntDistribution) {
    const int iterations = 10000;
    const int min_val = 0;
    const int max_val = 9;
    std::map<int, int> counts;

    for (int i = 0; i < iterations; ++i) {
        int value = Random::next_int(min_val, max_val);
        counts[value]++;
    }

    // Проверяем, что все значения встречаются
    for (int i = min_val; i <= max_val; ++i) {
        EXPECT_GT(counts[i], 0);
    }

    // Проверяем примерное равномерное распределение (с допуском 30%)
    int expected = iterations / (max_val - min_val + 1);
    for (const auto& pair : counts) {
        EXPECT_NEAR(pair.second, expected, expected * 0.3);
    }
}

TEST(RandomIntTest, NextShort) {
    for (int i = 0; i < 100; ++i) {
        short value = Random::next_short(100, 200);
        EXPECT_GE(value, 100);
        EXPECT_LE(value, 200);
    }
}

TEST(RandomIntTest, NextLong) {
    for (int i = 0; i < 100; ++i) {
        long value = Random::next_long(1000L, 2000L);
        EXPECT_GE(value, 1000L);
        EXPECT_LE(value, 2000L);
    }
}

TEST(RandomIntTest, NextLongLong) {
    for (int i = 0; i < 100; ++i) {
        long long value = Random::next_long_long(10000LL, 20000LL);
        EXPECT_GE(value, 10000LL);
        EXPECT_LE(value, 20000LL);
    }
}

TEST(RandomIntTest, NextUnsignedTypes) {
    unsigned int ui = Random::next_int<unsigned int>(100u, 200u);
    EXPECT_GE(ui, 100u);
    EXPECT_LE(ui, 200u);

    unsigned long ul = Random::next_ulong(1000ul, 2000ul);
    EXPECT_GE(ul, 1000ul);
    EXPECT_LE(ul, 2000ul);

    unsigned long long ull = Random::next_ulong_long(10000ull, 20000ull);
    EXPECT_GE(ull, 10000ull);
    EXPECT_LE(ull, 20000ull);
}

// ==================== Тесты типов с плавающей точкой ====================

TEST(RandomFloatTest, NextFloatReturnsValueInRange) {
    for (int i = 0; i < 100; ++i) {
        float value = Random::next_float(0.0f, 10.0f);
        EXPECT_GE(value, 0.0f);
        EXPECT_LT(value, 10.0f);
    }
}

TEST(RandomFloatTest, NextFloatDefaultRange) {
    for (int i = 0; i < 100; ++i) {
        float value = Random::next_float();
        EXPECT_GE(value, 0.0f);
        EXPECT_LT(value, 1.0f);
    }
}

TEST(RandomFloatTest, NextDoubleReturnsValueInRange) {
    for (int i = 0; i < 100; ++i) {
        double value = Random::next_double(5.0, 15.0);
        EXPECT_GE(value, 5.0);
        EXPECT_LT(value, 15.0);
    }
}

TEST(RandomFloatTest, NextDoubleDistribution) {
    const int iterations = 10000;
    const int buckets = 10;
    std::vector<int> counts(buckets, 0);

    for (int i = 0; i < iterations; ++i) {
        double value = Random::next_double(0.0, 1.0);
        int bucket = static_cast<int>(value * buckets);
        if (bucket >= buckets) bucket = buckets - 1;
        counts[bucket]++;
    }

    // Проверяем примерное равномерное распределение
    int expected = iterations / buckets;
    for (int count : counts) {
        EXPECT_NEAR(count, expected, expected * 0.3);
    }
}

TEST(RandomFloatTest, NextLongDouble) {
    for (int i = 0; i < 100; ++i) {
        long double value = Random::next_long_double(0.0L, 100.0L);
        EXPECT_GE(value, 0.0L);
        EXPECT_LT(value, 100.0L);
    }
}

// ==================== Тесты булевых значений ====================

TEST(RandomBoolTest, NextBoolDefaultProbability) {
    const int iterations = 10000;
    int true_count = 0;

    for (int i = 0; i < iterations; ++i) {
        if (Random::next_bool()) {
            true_count++;
        }
    }

    // Ожидаем около 50% true (с допуском)
    double ratio = static_cast<double>(true_count) / iterations;
    EXPECT_NEAR(ratio, 0.5, 0.05);
}

TEST(RandomBoolTest, NextBoolCustomProbability) {
    const int iterations = 10000;
    const double probability = 0.75;
    int true_count = 0;

    for (int i = 0; i < iterations; ++i) {
        if (Random::next_bool(probability)) {
            true_count++;
        }
    }

    double ratio = static_cast<double>(true_count) / iterations;
    EXPECT_NEAR(ratio, probability, 0.05);
}

TEST(RandomBoolTest, NextBoolZeroProbability) {
    for (int i = 0; i < 100; ++i) {
        EXPECT_FALSE(Random::next_bool(0.0));
    }
}

TEST(RandomBoolTest, NextBoolOneProbability) {
    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(Random::next_bool(1.0));
    }
}

// ==================== Тесты символов ====================

TEST(RandomCharTest, NextCharReturnsValueInRange) {
    for (int i = 0; i < 100; ++i) {
        char value = Random::next_char('a', 'z');
        EXPECT_GE(value, 'a');
        EXPECT_LE(value, 'z');
    }
}

TEST(RandomCharTest, NextCharNumbers) {
    for (int i = 0; i < 100; ++i) {
        char value = Random::next_char('0', '9');
        EXPECT_GE(value, '0');
        EXPECT_LE(value, '9');
    }
}

TEST(RandomCharTest, NextUChar) {
    for (int i = 0; i < 100; ++i) {
        unsigned char value = Random::next_uchar(100, 200);
        EXPECT_GE(value, 100);
        EXPECT_LE(value, 200);
    }
}

// ==================== Тесты коллекций ====================

TEST(RandomCollectionTest, ChoiceFromVector) {
    std::vector<int> vec = { 10, 20, 30, 40, 50 };

    for (int i = 0; i < 50; ++i) {
        int value = Random::choice(vec);
        EXPECT_TRUE(std::find(vec.begin(), vec.end(), value) != vec.end());
    }
}

TEST(RandomCollectionTest, ChoiceFromList) {
    std::list<std::string> lst = { "один", "два", "три", "четыре" };

    for (int i = 0; i < 20; ++i) {
        std::string value = Random::choice(lst);
        EXPECT_TRUE(std::find(lst.begin(), lst.end(), value) != lst.end());
    }
}

TEST(RandomCollectionTest, ChoiceEmptyCollection) {
    std::vector<int> empty_vec;
    EXPECT_THROW(Random::choice(empty_vec), std::invalid_argument);
}

TEST(RandomCollectionTest, ChoiceRefFromVector) {
    std::vector<int> vec = { 10, 20, 30, 40, 50 };

    for (int i = 0; i < 50; ++i) {
        const int& value = Random::choice_ref(vec);
        EXPECT_TRUE(std::find(vec.begin(), vec.end(), value) != vec.end());
    }
}

TEST(RandomCollectionTest, ChoicesWithReplacement) {
    std::vector<int> vec = { 1, 2, 3 };
    auto result = Random::choices(vec, 10);

    EXPECT_EQ(result.size(), 10);

    // Все элементы должны быть из исходного вектора
    for (int value : result) {
        EXPECT_TRUE(std::find(vec.begin(), vec.end(), value) != vec.end());
    }
}

TEST(RandomCollectionTest, ChoicesDistribution) {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    const int iterations = 5000;
    std::map<int, int> counts;

    for (int i = 0; i < iterations; ++i) {
        int value = Random::choice(vec);
        counts[value]++;
    }

    // Проверяем, что все элементы встречаются
    for (int val : vec) {
        EXPECT_GT(counts[val], 0);
    }
}

TEST(RandomCollectionTest, SampleWithoutReplacement) {
    std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto result = Random::sample(vec, 5);

    EXPECT_EQ(result.size(), 5);

    // Все элементы должны быть уникальными
    std::set<int> unique_values(result.begin(), result.end());
    EXPECT_EQ(unique_values.size(), 5);

    // Все элементы должны быть из исходного вектора
    for (int value : result) {
        EXPECT_TRUE(std::find(vec.begin(), vec.end(), value) != vec.end());
    }
}

TEST(RandomCollectionTest, SampleSizeEqualsCollectionSize) {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    auto result = Random::sample(vec, 5);

    EXPECT_EQ(result.size(), 5);

    // Должны быть все элементы (но возможно в другом порядке)
    std::sort(result.begin(), result.end());
    EXPECT_EQ(result, vec);
}

TEST(RandomCollectionTest, SampleSizeTooLarge) {
    std::vector<int> vec = { 1, 2, 3 };
    EXPECT_THROW(Random::sample(vec, 5), std::invalid_argument);
}

TEST(RandomCollectionTest, SampleEmptyCollection) {
    std::vector<int> empty_vec;
    EXPECT_THROW(Random::sample(empty_vec, 1), std::invalid_argument);
}

TEST(RandomCollectionTest, ShuffleVector) {
    std::vector<int> original = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    std::vector<int> vec = original;

    Random::shuffle(vec);

    // Размер не изменился
    EXPECT_EQ(vec.size(), original.size());

    // Все элементы присутствуют
    std::sort(vec.begin(), vec.end());
    EXPECT_EQ(vec, original);
}

TEST(RandomCollectionTest, ShuffledVector) {
    std::vector<int> original = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    auto shuffled = Random::shuffled(original);

    // Оригинал не изменился
    EXPECT_EQ(original, (std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}));

    // Shuffled содержит все элементы
    std::sort(shuffled.begin(), shuffled.end());
    EXPECT_EQ(shuffled, original);
}

TEST(RandomCollectionTest, ShuffleActuallyRandomizes) {
    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    int same_count = 0;
    const int trials = 100;

    for (int i = 0; i < trials; ++i) {
        auto shuffled = Random::shuffled(vec);
        if (shuffled == vec) {
            same_count++;
        }
    }

    // Вероятность того, что порядок не изменится при перемешивании 5 элементов = 1/120
    // За 100 попыток ожидаем менее 5 совпадений
    EXPECT_LT(same_count, 5);
}

// ==================== Тесты утилит ====================

TEST(RandomUtilityTest, NextBytes) {
    auto bytes = Random::next_bytes(100);

    EXPECT_EQ(bytes.size(), 100);

    // Проверяем, что есть разнообразие в байтах
    std::set<unsigned char> unique_bytes(bytes.begin(), bytes.end());
    EXPECT_GT(unique_bytes.size(), 10); // Должно быть хотя бы 10 различных значений
}

TEST(RandomUtilityTest, NextBytesEmpty) {
    auto bytes = Random::next_bytes(0);
    EXPECT_EQ(bytes.size(), 0);
}

TEST(RandomUtilityTest, NextStringDefaultAlphabet) {
    std::string str = Random::next_string(20);

    EXPECT_EQ(str.length(), 20);

    // Проверяем, что все символы алфавитно-цифровые
    for (char c : str) {
        EXPECT_TRUE(std::isalnum(static_cast<unsigned char>(c)));
    }
}

TEST(RandomUtilityTest, NextStringCustomAlphabet) {
    std::string alphabet = "ABCD";
    std::string str = Random::next_string(50, alphabet);

    EXPECT_EQ(str.length(), 50);

    // Проверяем, что все символы из алфавита
    for (char c : str) {
        EXPECT_TRUE(alphabet.find(c) != std::string::npos);
    }
}

TEST(RandomUtilityTest, NextStringEmptyLength) {
    std::string str = Random::next_string(0);
    EXPECT_EQ(str.length(), 0);
}

TEST(RandomUtilityTest, NextStringEmptyAlphabet) {
    EXPECT_THROW(Random::next_string(10, ""), std::invalid_argument);
}

TEST(RandomUtilityTest, NextStringDigitsOnly) {
    std::string str = Random::next_string(15, "0123456789");

    EXPECT_EQ(str.length(), 15);

    for (char c : str) {
        EXPECT_TRUE(c >= '0' && c <= '9');
    }
}

// ==================== Тесты распределений ====================

TEST(RandomDistributionTest, NextGaussianMeanAndStddev) {
    const int iterations = 10000;
    const double expected_mean = 50.0;
    const double expected_stddev = 10.0;

    double sum = 0.0;
    std::vector<double> values;

    for (int i = 0; i < iterations; ++i) {
        double value = Random::next_gaussian(expected_mean, expected_stddev);
        sum += value;
        values.push_back(value);
    }

    // Проверяем среднее значение
    double actual_mean = sum / iterations;
    EXPECT_NEAR(actual_mean, expected_mean, 1.0);

    // Проверяем стандартное отклонение
    double variance_sum = 0.0;
    for (double value : values) {
        double diff = value - actual_mean;
        variance_sum += diff * diff;
    }
    double actual_stddev = std::sqrt(variance_sum / iterations);
    EXPECT_NEAR(actual_stddev, expected_stddev, 1.0);
}

TEST(RandomDistributionTest, NextGaussianDefaultParameters) {
    const int iterations = 10000;
    double sum = 0.0;

    for (int i = 0; i < iterations; ++i) {
        sum += Random::next_gaussian();
    }

    double mean = sum / iterations;
    EXPECT_NEAR(mean, 0.0, 0.1);
}

TEST(RandomDistributionTest, NextExponential) {
    const int iterations = 10000;
    double sum = 0.0;

    for (int i = 0; i < iterations; ++i) {
        double value = Random::next_exponential(1.0);
        EXPECT_GE(value, 0.0); // Экспоненциальное распределение всегда >= 0
        sum += value;
    }

    // Для экспоненциального распределения с lambda=1, среднее = 1/lambda = 1
    double mean = sum / iterations;
    EXPECT_NEAR(mean, 1.0, 0.1);
}

TEST(RandomDistributionTest, NextExponentialWithDifferentLambda) {
    const int iterations = 10000;
    const double lambda = 2.0;
    double sum = 0.0;

    for (int i = 0; i < iterations; ++i) {
        double value = Random::next_exponential(lambda);
        sum += value;
    }

    double mean = sum / iterations;
    EXPECT_NEAR(mean, 1.0 / lambda, 0.1);
}

// ==================== Тесты seed ====================

TEST(RandomSeedTest, SeedReproducibility) {
    const unsigned int seed_value = 12345;

    // Генерируем последовательность с seed
    Random::seed(seed_value);
    std::vector<int> sequence1;
    for (int i = 0; i < 10; ++i) {
        sequence1.push_back(Random::next_int(0, 1000000));
    }

    // Генерируем ещё одну последовательность с тем же seed
    Random::seed(seed_value);
    std::vector<int> sequence2;
    for (int i = 0; i < 10; ++i) {
        sequence2.push_back(Random::next_int(0, 1000000));
    }

    // Последовательности должны совпадать
    EXPECT_EQ(sequence1, sequence2);
}

// ==================== Тесты потокобезопасности ====================

TEST(RandomThreadSafetyTest, ConcurrentNextInt) {
    const int num_threads = 10;
    const int iterations_per_thread = 1000;
    std::vector<std::thread> threads;
    std::vector<std::vector<int>> results(num_threads);

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&results, t, iterations_per_thread]() {
            for (int i = 0; i < iterations_per_thread; ++i) {
                results[t].push_back(Random::next_int(0, 1000));
            }
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // Проверяем, что все потоки сгенерировали нужное количество значений
    for (int t = 0; t < num_threads; ++t) {
        EXPECT_EQ(results[t].size(), iterations_per_thread);

        // Проверяем, что все значения в допустимом диапазоне
        for (int value : results[t]) {
            EXPECT_GE(value, 0);
            EXPECT_LE(value, 1000);
        }
    }
}

TEST(RandomThreadSafetyTest, ConcurrentMixedOperations) {
    const int num_threads = 8;
    std::vector<std::thread> threads;
    std::atomic<int> errors{ 0 };

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&errors]() {
            try {
                // Микс различных операций
                Random::next_int(0, 100);
                Random::next_double(0.0, 1.0);
                Random::next_bool();
                Random::next_string(10);

                std::vector<int> vec = { 1, 2, 3, 4, 5 };
                Random::choice(vec);
                Random::shuffle(vec);
            }
            catch (...) {
                errors++;
            }
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(errors.load(), 0);
}

TEST(RandomThreadSafetyTest, ConcurrentCollectionOperations) {
    const int num_threads = 10;
    std::vector<std::thread> threads;
    std::vector<int> shared_vector = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&shared_vector]() {
            for (int i = 0; i < 100; ++i) {
                // Только чтение из shared_vector
                int value = Random::choice(shared_vector);
                EXPECT_GE(value, 1);
                EXPECT_LE(value, 10);

                auto sample = Random::sample(shared_vector, 3);
                EXPECT_EQ(sample.size(), 3);
            }
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

// ==================== Граничные случаи ====================

TEST(RandomEdgeCaseTest, IntMinMax) {
    // Тестируем с экстремальными значениями
    int value = Random::next_int(std::numeric_limits<int>::min(),
        std::numeric_limits<int>::min() + 100);
    EXPECT_GE(value, std::numeric_limits<int>::min());
    EXPECT_LE(value, std::numeric_limits<int>::min() + 100);
}

TEST(RandomEdgeCaseTest, LargeCollectionSample) {
    std::vector<int> large_vec(10000);
    std::iota(large_vec.begin(), large_vec.end(), 0);

    auto sample = Random::sample(large_vec, 100);
    EXPECT_EQ(sample.size(), 100);

    // Проверяем уникальность
    std::set<int> unique(sample.begin(), sample.end());
    EXPECT_EQ(unique.size(), 100);
}

TEST(RandomEdgeCaseTest, SingleElementCollection) {
    std::vector<int> single = { 42 };

    EXPECT_EQ(Random::choice(single), 42);

    auto sample = Random::sample(single, 1);
    EXPECT_EQ(sample.size(), 1);
    EXPECT_EQ(sample[0], 42);

    Random::shuffle(single);
    EXPECT_EQ(single[0], 42);
}