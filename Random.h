#pragma once

#include <random>
#include <mutex>
#include <type_traits>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <limits>
#include <thread>


namespace dungeons {


    /**
     * Потокобезопасный статический класс для генерации случайных чисел
     * Предоставляет методы для генерации случайных значений всех базовых типов
     * и выбора случайных элементов из коллекций
     */
    class Random {
    public:
        // Конструкторы удалены - только статические методы
        Random() = delete;
        Random(const Random&) = delete;
        Random& operator=(const Random&) = delete;

        // ==================== Целочисленные типы ====================

        /**
         Генерация случайного целого числа в диапазоне [min, max]
         */
        template<typename T>
        static typename std::enable_if<std::is_integral<T>::value, T>::type
            next_int(T min = std::numeric_limits<T>::min(),
                T max = std::numeric_limits<T>::max()) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::uniform_int_distribution<T> dist(min, max);
            return dist(get_engine());
        }

        /**
         * Генерация случайного булевого значения с заданной вероятностью true
         * @param probability - вероятность возврата true (от 0.0 до 1.0)
         */
        static bool next_bool(double probability = 0.5) {
            return next_double(0.0, 1.0) < probability;
        }

        /**
         Генерация случайного char
         */
        static char next_char(char min = std::numeric_limits<char>::min(),
            char max = std::numeric_limits<char>::max()) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::uniform_int_distribution<int> dist(static_cast<int>(min),
                static_cast<int>(max));
            return static_cast<char>(dist(get_engine()));
        }

        /**
         Генерация случайного unsigned char
         */
        static unsigned char next_uchar(unsigned char min = std::numeric_limits<unsigned char>::min(),
            unsigned char max = std::numeric_limits<unsigned char>::max()) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::uniform_int_distribution<unsigned int> dist(static_cast<unsigned int>(min),
                static_cast<unsigned int>(max));
            return static_cast<unsigned char>(dist(get_engine()));
        }

        /**
         Генерация случайного short
         */
        static short next_short(short min = std::numeric_limits<short>::min(),
            short max = std::numeric_limits<short>::max()) {
            return next_int<short>(min, max);
        }

        /**
         Генерация случайного unsigned short
         */
        static unsigned short next_ushort(unsigned short min = std::numeric_limits<unsigned short>::min(),
            unsigned short max = std::numeric_limits<unsigned short>::max()) {
            return next_int<unsigned short>(min, max);
        }

        /**
         Генерация случайного long
         */
        static long next_long(long min = std::numeric_limits<long>::min(),
            long max = std::numeric_limits<long>::max()) {
            return next_int<long>(min, max);
        }

        /**
         Генерация случайного unsigned long
         */
        static unsigned long next_ulong(unsigned long min = std::numeric_limits<unsigned long>::min(),
            unsigned long max = std::numeric_limits<unsigned long>::max()) {
            return next_int<unsigned long>(min, max);
        }

        /**
         Генерация случайного long long
         */
        static long long next_long_long(long long min = std::numeric_limits<long long>::min(),
            long long max = std::numeric_limits<long long>::max()) {
            return next_int<long long>(min, max);
        }

        /**
         Генерация случайного unsigned long long
         */
        static unsigned long long next_ulong_long(unsigned long long min = std::numeric_limits<unsigned long long>::min(),
            unsigned long long max = std::numeric_limits<unsigned long long>::max()) {
            return next_int<unsigned long long>(min, max);
        }


        // ==================== Типы с плавающей точкой ====================

        /**
         Генерация случайного float в диапазоне [min, max)
         */
        static float next_float(float min = 0.0f, float max = 1.0f) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::uniform_real_distribution<float> dist(min, max);
            return dist(get_engine());
        }

        /**
         Генерация случайного double в диапазоне [min, max)
         */
        static double next_double(double min = 0.0, double max = 1.0) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::uniform_real_distribution<double> dist(min, max);
            return dist(get_engine());
        }

        /**
         Генерация случайного long double в диапазоне [min, max)
         */
        static long double next_long_double(long double min = 0.0L, long double max = 1.0L) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::uniform_real_distribution<long double> dist(min, max);
            return dist(get_engine());
        }


        // ==================== Операции с коллекциями ====================

        /**
         * Выбор случайного элемента из коллекции
         * @throws std::invalid_argument если коллекция пустая
         */
        template<typename Container>
        static typename Container::value_type choice(const Container& collection) {
            if (collection.empty()) {
                throw std::invalid_argument("Невозможно выбрать из пустой коллекции");
            }

            auto it = collection.begin();
            std::advance(it, next_int<size_t>(0, collection.size() - 1));
            return *it;
        }

        /**
         * Выбор случайного элемента из коллекции по ссылке
         * @throws std::invalid_argument если коллекция пустая
         */
        template<typename Container>
        static const typename Container::value_type& choice_ref(const Container& collection) {
            if (collection.empty()) {
                throw std::invalid_argument("Невозможно выбрать из пустой коллекции");
            }

            auto it = collection.begin();
            std::advance(it, next_int<size_t>(0, collection.size() - 1));
            return *it;
        }

        /**
         * Выбор n случайных элементов из коллекции (с повторениями)
         * @param n - количество элементов для выбора
         */
        template<typename Container>
        static std::vector<typename Container::value_type>
            choices(const Container& collection, size_t n) {
            if (collection.empty()) {
                throw std::invalid_argument("Невозможно выбрать из пустой коллекции");
            }

            std::vector<typename Container::value_type> result;
            result.reserve(n);

            for (size_t i = 0; i < n; ++i) {
                result.push_back(choice(collection));
            }

            return result;
        }

        /**
         * Выбор n случайных уникальных элементов из коллекции (без повторений)
         * @param n - количество элементов для выбора
         * @throws std::invalid_argument если n > collection.size()
         */
        template<typename Container>
        static std::vector<typename Container::value_type>
            sample(const Container& collection, size_t n) {
            if (collection.empty()) {
                throw std::invalid_argument("Невозможно сделать выборку из пустой коллекции");
            }
            if (n > collection.size()) {
                throw std::invalid_argument("Размер выборки не может быть больше размера коллекции");
            }

            std::vector<typename Container::value_type> temp(collection.begin(), collection.end());
            shuffle(temp);

            return std::vector<typename Container::value_type>(temp.begin(), temp.begin() + n);
        }

        /**
         Перемешивание коллекции
         */
        template<typename Container>
        static void shuffle(Container& collection) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::shuffle(collection.begin(), collection.end(), get_engine());
        }

        /**
         Получение перемешанной копии коллекции
         */
        template<typename Container>
        static Container shuffled(const Container& collection) {
            Container result = collection;
            shuffle(result);
            return result;
        }

        // ==================== Утилиты ====================

        /**
         Установка seed для генератора случайных чисел
         */
        static void seed(unsigned int seed_value) {
            std::lock_guard<std::mutex> lock(get_mutex());
            get_engine().seed(seed_value);
        }

        /**
         Генерация случайных байтов
         */
        static std::vector<unsigned char> next_bytes(size_t count) {
            std::vector<unsigned char> result;
            result.reserve(count);

            for (size_t i = 0; i < count; ++i) {
                result.push_back(next_uchar());
            }

            return result;
        }

        /**
         Генерация случайной алфавитно-цифровой строки
         */
        static std::string next_string(size_t length,
            const std::string& alphabet =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") {
            if (alphabet.empty()) {
                throw std::invalid_argument("Алфавит не может быть пустым");
            }

            std::string result;
            result.reserve(length);

            for (size_t i = 0; i < length; ++i) {
                result += alphabet[next_int<size_t>(0, alphabet.size() - 1)];
            }

            return result;
        }

        /**
         Генерация случайного значения из нормального (гауссова) распределения
         */
        static double next_gaussian(double mean = 0.0, double stddev = 1.0) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::normal_distribution<double> dist(mean, stddev);
            return dist(get_engine());
        }

        /**
         Генерация случайного значения из экспоненциального распределения
         */
        static double next_exponential(double lambda = 1.0) {
            std::lock_guard<std::mutex> lock(get_mutex());
            std::exponential_distribution<double> dist(lambda);
            return dist(get_engine());
        }

    private:
        /**
         Получение потокобезопасного генератора случайных чисел
         */
        static std::mt19937_64& get_engine() {
            static thread_local std::mt19937_64 engine(
                std::random_device{}() +
                std::hash<std::thread::id>{}(std::this_thread::get_id())
            );
            return engine;
        }

        /**
         Получение мьютекса для синхронизации
         */
        static std::mutex& get_mutex() {
            static std::mutex mtx;
            return mtx;
        }
    };


}