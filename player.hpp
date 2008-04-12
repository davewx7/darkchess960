#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include "board.hpp"

struct EndGameException {
};

class Player
{
public:
	virtual ~Player() {}
	virtual void OpponentMove() = 0;
	virtual Move MakeMove(const Board& b) = 0;
};

class PlayerProgressSink
{
public:
	virtual void PercentComplete(int num) = 0;
};

#endif
