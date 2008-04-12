#ifndef AI_PLAYER_HPP_INCLUDED
#define AI_PLAYER_HPP_INCLUDED

#include "beliefs.hpp"
#include "player.hpp"

class AIPlayer : public Player
{
public:
	explicit AIPlayer(const Board& b, PlayerProgressSink* progress=0);
	void OpponentMove();
	Move MakeMove(const Board& b);

private:
	BeliefStates beliefs_;
	bool opponent_moved_;
	Move last_move_;
	PlayerProgressSink* progress_;
};

#endif
