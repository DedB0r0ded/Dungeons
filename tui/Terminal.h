#pragma once


#include <iostream>


namespace dungeons::tui {


	class BaseTerminal {
		std::string buffer_;
		std::ostream& output_;

		void clear_buffer() {
			buffer_ = "";
		}

	public:
		BaseTerminal(std::ostream& output, std::string buffer) : output_{ output }, buffer_ { buffer } {}
		BaseTerminal(std::ostream& output) : BaseTerminal{output, ""} {}
		
		void put(std::string str) {
			buffer_ += str;
		}
		void putln(std::string str) {
			put(str);
			put("\n");
		}
		void print() {
			output_ << buffer_;
			clear_buffer();
		}
	};


}