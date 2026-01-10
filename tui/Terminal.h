#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include <iostream>
#include <utility>
#include <chrono>
#include <string>
#include <algorithm>


namespace dungeons::tui {


    // Структура для хранения размеров
    struct TerminalSize {
        int width;
        int height;
    };


    class Terminal {
#ifdef _WIN32
        static HANDLE g_hConsoleOut;
        static HANDLE g_hConsoleIn;
#endif
        static const inline std::string window_title_ = "Dungeons - TUI Game";

        std::string buffer_;
        std::ostream& output_;

        void clear_buffer() {
            buffer_ = "";
        }

    public:
        Terminal(std::ostream& output, std::string buffer) : output_{ output }, buffer_{ buffer } {}
        Terminal(std::ostream& output) : Terminal{ output, "" } {}

        void put(const std::string& str) {
            buffer_ += str;
        }

        void putln(const std::string& str) {
            put(str);
            put("\n");
        }

        void print() {
            output_ << buffer_;
            clear_buffer();
        }

        // Получить текущий размер окна терминала
        static TerminalSize get_size() {
            TerminalSize size = { 0, 0 };
#ifdef _WIN32
            HANDLE hConsole = g_hConsoleOut;
            if (hConsole == INVALID_HANDLE_VALUE) {
                hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            }
            if (hConsole == INVALID_HANDLE_VALUE)
                return size;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
                size.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                size.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            }
#else
            struct winsize w;
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
                size.width = w.ws_col;
                size.height = w.ws_row;
            }
#endif
            return size;
        }

        // Установить размер окна терминала (только в символах)
        static bool set_size(int width, int height) {
#ifdef _WIN32
            HANDLE hConsole = g_hConsoleOut;
            if (hConsole == INVALID_HANDLE_VALUE)
                hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hConsole == INVALID_HANDLE_VALUE)
                return false;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
                return false;
            COORD maxSize = GetLargestConsoleWindowSize(hConsole);
            int actualWidth = std::min(width, (int)maxSize.X);
            int actualHeight = std::min(height, (int)maxSize.Y);
            SMALL_RECT windowSize = { 0, 0, (SHORT)(actualWidth - 1), (SHORT)(actualHeight - 1) };
            COORD bufferSize = { (SHORT)actualWidth, (SHORT)actualHeight };
            SHORT currentWindowWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            SHORT currentWindowHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            // Порядок операций зависит от того, увеличиваем или уменьшаем
            if (actualWidth < currentWindowWidth || actualHeight < currentWindowHeight) {
                // Уменьшение: сначала окно, потом буфер
                if (!SetConsoleWindowInfo(hConsole, TRUE, &windowSize))
                    return false;
                if (!SetConsoleScreenBufferSize(hConsole, bufferSize))
                    return false;
            }
            else {
                // Увеличение: сначала буфер, потом окно
                if (!SetConsoleScreenBufferSize(hConsole, bufferSize))
                    return false;
                if (!SetConsoleWindowInfo(hConsole, TRUE, &windowSize))
                    return false;
            }
            return true;
#else
            // ANSI escape-последовательность для изменения размера
            std::cout << "\033[8;" << height << ";" << width << "t" << std::flush;
            return true;
#endif
        }

        // Очистить экран
        static void clear_screen() {
#ifdef _WIN32
            HANDLE hConsole = g_hConsoleOut;
            if (hConsole == INVALID_HANDLE_VALUE) {
                hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            }
            if (hConsole == INVALID_HANDLE_VALUE)
                return;
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
                return;
            DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
            COORD homeCoords = { 0, 0 };
            DWORD count;
            // Заполняем консоль пробелами
            if (!FillConsoleOutputCharacterA(hConsole, ' ', cellCount, homeCoords, &count))
                return;
            // Сбрасываем атрибуты
            if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count))
                return;
            // Перемещаем курсор в начало
            SetConsoleCursorPosition(hConsole, homeCoords);
#else
            std::cout << "\033[2J\033[H" << std::flush;
#endif
        }

        // Установить позицию курсора
        static void set_cursor_position(int x, int y) {
#ifdef _WIN32
            HANDLE hConsole = g_hConsoleOut;
            if (hConsole == INVALID_HANDLE_VALUE)
                hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hConsole == INVALID_HANDLE_VALUE)
                return;
            COORD pos = { (SHORT)x, (SHORT)y };
            SetConsoleCursorPosition(hConsole, pos);
#else
            std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H" << std::flush;
#endif
        }

        // Скрыть/показать курсор
        static void set_cursor_visible(bool visible) {
#ifdef _WIN32
            HANDLE hConsole = g_hConsoleOut;
            if (hConsole == INVALID_HANDLE_VALUE) {
                hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            }
            if (hConsole == INVALID_HANDLE_VALUE) {
                return;
            }
            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(hConsole, &cursorInfo);
            cursorInfo.bVisible = visible;
            SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
            if (visible) {
                std::cout << "\033[?25h" << std::flush;
            }
            else {
                std::cout << "\033[?25l" << std::flush;
            }
#endif
        }

        // Включить поддержку ANSI escape-последовательностей в Windows
        static bool enable_virtual() {
#ifdef _WIN32
            HANDLE hOut = g_hConsoleOut;
            if (hOut == INVALID_HANDLE_VALUE)
                hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE)
                return false;
            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode))
                return false;
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            if (!SetConsoleMode(hOut, dwMode))
                return false;
            return true;
#else
            return true;
#endif
        }

        // Инициализация консоли
        static void init_console() {
#ifdef _WIN32
            g_hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
            g_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);
            set_title(window_title_);
            // Включаем поддержку виртуального терминала
            enable_virtual();
#endif
        }

        // Закрыть консоль
        static void close_console() {
#ifdef _WIN32
            // Сбрасываем дескрипторы
            g_hConsoleOut = INVALID_HANDLE_VALUE;
            g_hConsoleIn = INVALID_HANDLE_VALUE;
#endif
        }

        // Инициализация локали для поддержки UTF-8
        static void init_locale() {
#ifdef _WIN32
            setlocale(LC_ALL, ".UTF-8");
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
#else
            setlocale(LC_ALL, "");
#endif
        }

        // Установить заголовок окна консоли
        static void set_title(const std::string& title) {
#ifdef _WIN32
            SetConsoleTitleA(title.c_str());
#else
            std::cout << "\033]0;" << title << "\007" << std::flush;
#endif
        }
    };

    // Инициализация статических членов
#ifdef _WIN32
    HANDLE Terminal::g_hConsoleOut = INVALID_HANDLE_VALUE;
    HANDLE Terminal::g_hConsoleIn = INVALID_HANDLE_VALUE;
#endif


} // namespace dungeons::tui