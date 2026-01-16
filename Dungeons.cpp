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

using namespace std::chrono_literals;


static constexpr size_t ROWS{ 30 };
static constexpr size_t COLS{ 80 };

static constexpr size_t LOC_ROWS{ 10 };
static constexpr size_t LOC_COLS{ COLS };

static constexpr size_t STAT_ROWS{ 7 };
static constexpr size_t STAT_COLS{ 13 };

static constexpr size_t RES_ROWS{ 7 };
static constexpr size_t RES_COLS{ 15 };


static constexpr size_t FIRST_ROW{ 0 };
static constexpr size_t FIRST_COL{ 0 };
static constexpr size_t LAST_ROW{ ROWS - 1 };
static constexpr size_t LAST_COL{ COLS - 1 };

static constexpr size_t FLOCR{ FIRST_ROW };
static constexpr size_t FLOCC{ FIRST_COL };
static constexpr size_t LLOCR{ FLOCR + LOC_ROWS - 1 };
static constexpr size_t LLOCC{ FLOCC + LOC_COLS - 1 };

static constexpr size_t FSTTR{ LAST_ROW - STAT_ROWS + 1 };
static constexpr size_t FSTTC{ FIRST_COL };
static constexpr size_t LSTTR{ FSTTR + STAT_ROWS };
static constexpr size_t LSTTC{ FSTTC + STAT_COLS };

static constexpr size_t FRESR{ LAST_ROW - RES_ROWS + 1 };
static constexpr size_t FRESC{ LAST_COL - RES_COLS + 1 };
static constexpr size_t LRESR{ FRESR + RES_ROWS };
static constexpr size_t LRESC{ FRESC + RES_COLS };


static constexpr unsigned int FPS{ 20 };
static constexpr std::chrono::milliseconds DELAY(1000 / FPS);
static unsigned int fi = 0;


static void printFrame(FrameSnapshot& frame, Terminal& t) {
	t.put(frame.to_string().value());
	t.print();
	t.set_cursor_position(0, 0);
}


static void onStart() {

}


static void onUpdate(Terminal& t, FrameSnapshot& main_frame, FrameSnapshot& location_segment, FrameSnapshot& stats_segment) {
	main_frame.blit(location_segment, 0, 0, FLOCR, FLOCC, LOC_ROWS, LOC_COLS);
	main_frame.blit(stats_segment, 0, 0, FSTTR, FSTTC, STAT_ROWS, STAT_COLS);
	printFrame(main_frame, t);
}


int main()
{
	Terminal::init_locale();
	Terminal::set_size(ROWS + 2, COLS + 2);
	Terminal::set_cursor_visible(false);
	auto t = Terminal(std::cout);

	Color bgc = Color::WHITE;
	Color fgc = Color::BLACK;
	
	UnicodeCharMatrix location_segment_chars('L', LOC_ROWS, LOC_COLS);
	CharStyle location_segment_style(Color::GREEN, bgc);
	FrameSnapshot location_segment(location_segment_chars, location_segment_style);

	UnicodeCharMatrix stats_segment_chars('S', STAT_ROWS, STAT_COLS);
	CharStyle stats_segment_style(Color::BLUE, bgc);
	FrameSnapshot stats_segment(stats_segment_chars, stats_segment_style);

	UnicodeCharMatrix chars('X', ROWS, COLS);
	FrameSnapshot frame(chars, CharStyle(fgc, bgc));

	using clock = std::chrono::steady_clock;

	auto next_frame = clock::now();

	onStart();

	while (true) {
		next_frame += DELAY;
		onUpdate(t, frame, location_segment, stats_segment);
		fi >= FPS ? fi = 0 : fi++;
		std::this_thread::sleep_until(next_frame);
	}

	return 0;
}
