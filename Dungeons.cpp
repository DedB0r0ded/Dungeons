#include "Dungeons.h"
#include "tui/UnicodeCharMatrix.h"

// Символы, чтобы заставить Windows сохранить исходный код в UTF-8.
// Нужно для правильной работы библиотеки nlohmann::json.
//  |
//  V
//"Привет! Hello! 你好！

void initLocale() {
	const char* locale_result = setlocale(LC_ALL, "");
#ifdef _WIN32
	locale_result = setlocale(LC_ALL, ".UTF-8");
	SetConsoleOutputCP(CP_UTF8);
#endif
}


int main()
{
	initLocale();
	dungeons::Random::init();
	auto t = dungeons::tui::BaseTerminal(std::cout);

	t.put("\033[1;3;4;38;2;255;100;0;48;5;34m");
	dungeons::tui::UnicodeCharMatrix matrix({ "АБВ", "ГДЕ", "ЁЖЗ" });
	
	auto ch = matrix.get_at(1, 1);
	t.put(matrix.to_string());
	t.putln(std::to_string(matrix.cols().value()));
	dungeons::Time a = dungeons::Time::now().value();
	//auto frame = dungeons::FrameSnapshot(10, 100);
	//t.put(frame.to_string());
	t.put("\033[0m");
	t.print();
	a.move_to_timezone(480);
	t.putln(std::to_string(dungeons::Random::randDouble()));
	t.putln(a.to_string());
	t.print();
	return 0;
}
