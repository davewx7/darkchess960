#ifndef SDL_PLAYER_HPP_INCLUDED
#define SDL_PLAYER_HPP_INCLUDED

#include "board.hpp"
#include "player.hpp"

class SDLPlayer : public Player, public PlayerProgressSink
{
public:
	explicit SDLPlayer(const Board& b);
	~SDLPlayer();
	void OpponentMove();
	Move MakeMove(const Board& b);
	void PercentComplete(int num);
private:
	void Draw(const Board& b);
	void DisplayMove(const Board& b, const Move& move);

	int square_width_, square_height_;
	int selection_;

	std::vector<Square> captures_;
	std::vector<int> capture_pos_;
	Board last_board_;
};

#endif
