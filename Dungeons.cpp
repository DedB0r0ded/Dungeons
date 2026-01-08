#include "Dungeons.h"

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

using dungeons::tui::UnicodeCharMatrix;
using dungeons::tui::FrameSnapshot;
using dungeons::tui::CharStyle;
using dungeons::tui::Color;

int main()
{
	initLocale();
	dungeons::Random::init();
	auto t = dungeons::tui::BaseTerminal(std::cout);

	UnicodeCharMatrix chars({ "АБ你", "ГДЕ", "ЁЖЗ" });
	FrameSnapshot frame(chars, CharStyle(Color::BRIGHT_WHITE, Color::BLACK));
	frame.set_area_style(0, 0, 2, 0, CharStyle(Color::RED, Color::WHITE));
	t.put(frame.to_string().value());
	dungeons::Time a = dungeons::Time::now().value();
	t.put("\033[0m");
	t.print();
	a.move_to_timezone(480);
	t.putln(std::to_string(dungeons::Random::randDouble()));
	t.putln(a.to_string());
	t.print();
	return 0;
}
