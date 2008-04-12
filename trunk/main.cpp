#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "ai_player.hpp"
#include "console_player.hpp"
#include "sdl_player.hpp"

int main()
{
	for(;;) {
		srand(time(NULL));
		Board pos;
		std::vector<Player*> players;
		SDLPlayer* human = new SDLPlayer(pos);
		players.push_back(human);
		players.push_back(new AIPlayer(pos, human));

		try {
			for(int player = 0; ; ++player) {
				std::cerr << "PLAYER: " << player << "\n";
				Player* p = players[player%players.size()];
				if(player != 0) {
					p->OpponentMove();
				}

//				std::cout << "POS: " << pos << "\n";
				Board visible_board;
				pos.VisibleBoard(&visible_board);
				Move m = p->MakeMove(visible_board);
				pos.ApplyMove(m.src, m.dst);
			}
		} catch(EndGameException&) {
		}

		delete players.front();
		delete players.back();
	}
}
