#include <assert.h>
#include <iostream>

#include "ai_player.hpp"

AIPlayer::AIPlayer(const Board& b, PlayerProgressSink* progress)
    : opponent_moved_(false), progress_(progress)
{
	last_move_.src = 0;
	last_move_.dst = 0;
	beliefs_.Add(b);
}

void AIPlayer::OpponentMove()
{
	opponent_moved_ = true;
}

Move AIPlayer::MakeMove(const Board& b)
{
	if(opponent_moved_) {
		opponent_moved_ = false;
		beliefs_.NextMove(last_move_.src, last_move_.dst, b, progress_);
		std::cout << "BELIEF STATE SPACE: " << beliefs_.Size() << "\n";
	}
	assert(beliefs_.Size() > 0);

	if(progress_) {
		progress_->PercentComplete(50);
	}

	last_move_.src = last_move_.dst = 0;
	int best_rating = 0;
	std::vector<Move> moves;
	b.LegalMoves(&moves);
	for(std::vector<Move>::const_iterator i = moves.begin();
	    i != moves.end(); ++i) {
		if(progress_) {
			progress_->PercentComplete(50 + ((i - moves.begin())*50)/moves.size());
		}
		std::cout << "EVALUATING MOVE " << (i - moves.begin()) << "/" << moves.size() << std::endl;
		MoveEvaluation eval = beliefs_.EvaluateMove(i->src, i->dst, b);
		if(b.WhiteToMove() == false) {
			std::swap(eval.highest, eval.lowest);
			eval.highest *= -1;
			eval.lowest *= -1;
			eval.sum *= -1;
			eval.sum /= eval.samples;
		}

		const int rating = eval.sum + eval.lowest * 5;

		if(last_move_.src == last_move_.dst || rating > best_rating) {
			last_move_ = *i;
			best_rating = rating;
		}
		std::cerr << "evaluate " << i->src << " -> " << i->dst << ": " << eval.highest << ", " << eval.lowest << ", " << eval.sum << "\n";
	}

	std::cerr << "MOVE " << last_move_.src << " -> " << last_move_.dst << "\n";
	if(b.GetSquare(last_move_.dst).empty() == false) {
		std::cerr << "CASTLE!\n";
	}

	return last_move_;
}
