#include <iostream>

void clean_screen() {
	std::cout << "\x1b[2J";
}

void get_input() {

}

void update_world() {

}

void refresh_screen() {
	
	
}



void start_event_loop() {
	while (true) {
		get_input();
		update_world();
		refresh_screen();
	}
}