#include <iostream>

#include "console_player.hpp"

ConsolePlayer::ConsolePlayer(const Board& b)
{
}

void ConsolePlayer::OpponentMove()
{
}

Move ConsolePlayer::MakeMove(const Board& b)
{
	std::cout << b;

	std::string input;
	while(true) {
		std::cin >> input;
		if(input.size() == 4) {
			const int from_col = input[0] - 'a';
			const int from_row = input[1] - '1';
			const int to_col = input[2] - 'a';
			const int to_row = input[3] - '1';

			const int from = from_row*8 + from_col;
			const int to = to_row*8 + to_col;
			if(from >= 0 && from < 64 && to >= 0 && to < 64) {
				Move m;
				m.src = from;
				m.dst = to;
				std::cerr << "returning...\n";
				return m;
			}
		}
	}

	assert(false);
}
