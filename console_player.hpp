#ifndef CONSOLE_PLAYER_HPP_INCLUDED
#define CONSOLE_PLAYER_HPP_INCLUDED

#include "player.hpp"

class ConsolePlayer : public Player
{
public:
	explicit ConsolePlayer(const Board& b);
	void OpponentMove();
	Move MakeMove(const Board& b);

private:
};

#endif
