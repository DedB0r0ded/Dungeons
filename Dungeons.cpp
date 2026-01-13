#include "Dungeons.h"

// Символы, чтобы заставить Windows сохранить исходный код в UTF-8.
// Нужно для правильной работы библиотеки nlohmann::json.
//  |
//  V
//"Привет! Hello! 你好！


using dungeons::tui::UnicodeCharMatrix;
using dungeons::tui::FrameSnapshot;
using dungeons::tui::CharStyle;
using dungeons::tui::Color;

using dungeons::tui::Terminal;

int main()
{
	Terminal::init_locale();
	Terminal::set_size(80, 30);
	auto t = Terminal(std::cout);

	UnicodeCharMatrix chars({ "АБ你", "ГД\U0001F3AE", "ЁЖЗ" });
	FrameSnapshot frame(chars, CharStyle(Color::BRIGHT_WHITE, Color::BLACK));
	frame.set_area_style(0, 0, 2, 0, CharStyle(Color::RED, Color::WHITE));
	t.put(frame.to_string().value());
	dungeons::Time a = dungeons::Time::now().value();
	t.put("\033[0m");
	t.print();
	a.move_to_timezone(480);
	t.putln(std::to_string(dungeons::Random::next_double()));
	t.putln(a.to_string());
	t.print();
	return 0;
}
