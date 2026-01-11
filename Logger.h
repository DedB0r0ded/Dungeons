// Logger.h
#pragma once

#include <string>
#include <sstream>
#include <mutex>
#include <thread>
#include <vector>
#include "../time.h"
#include "../result.h"

namespace dungeons::backend {


    class Logger {
    private:
        std::ostringstream buffer_;
        mutable std::mutex mutex_;

        Logger() = default;

    public:
        // Удаляем конструкторы копирования и перемещения
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;
        ~Logger() = default;

        // Singleton
        static Logger& instance() {
            static Logger logger;
            return logger;
        }

        // Логирование сообщения
        void log(const std::string& message) {
            std::lock_guard<std::mutex> lock(mutex_);

            auto time_result = Time::now();
            Time timestamp = time_result ? time_result.value() : Time();

            auto thread_id = std::this_thread::get_id();
            std::ostringstream thread_id_stream;
            thread_id_stream << thread_id;

            buffer_ << "[" << timestamp.to_iso8601() << "]"
                << "[Thread:" << thread_id_stream.str() << "] "
                << message << "\n";
        }

        // Логирование с уровнем
        void log(const std::string& level, const std::string& message) {
            std::lock_guard<std::mutex> lock(mutex_);

            auto time_result = Time::now();
            Time timestamp = time_result ? time_result.value() : Time();

            auto thread_id = std::this_thread::get_id();
            std::ostringstream thread_id_stream;
            thread_id_stream << thread_id;

            buffer_ << "[" << timestamp.to_iso8601() << "]"
                << "[" << level << "]"
                << "[Thread:" << thread_id_stream.str() << "] "
                << message << "\n";
        }

        // Получение всего лога
        std::string get_log() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return buffer_.str();
        }

        // Очистка лога
        void clear() {
            std::lock_guard<std::mutex> lock(mutex_);
            buffer_.str("");
            buffer_.clear();
        }

        // Удобные методы для разных уровней логирования
        void info(const std::string& message) {
            log("INFO", message);
        }

        void warning(const std::string& message) {
            log("WARNING", message);
        }

        void error(const std::string& message) {
            log("ERROR", message);
        }

        void debug(const std::string& message) {
            log("DEBUG", message);
        }
    };


} // namespace dungeons::backend