#include <iostream>

#include "beliefs.hpp"
#include "eval.hpp"
#include "player.hpp"

BeliefStates::~BeliefStates()
{
	for(std::vector<Belief*>::iterator i = table_.begin();
	    i != table_.end(); ++i) {
		for(Belief* b = *i; b != NULL; ) {
			Belief* tmp = b;
			b = b->next;
			delete tmp;
		}
	}
}

void BeliefStates::Add(const Board& b)
{
	const uint64_t index = b.Hash()%(int64_t)table_.size();
	for(Belief* ptr = table_[index]; ptr != NULL; ptr = ptr->next) {
		if(ptr->board.Equal(b)) {
			return;
		}
	}

	Belief* belief = new Belief;
	belief->board = b;
	belief->next = table_[index];
	table_[index] = belief;
	++nbeliefs_;
}

void BeliefStates::NextMove(int src, int dst, const Board& known_board, PlayerProgressSink* progress)
{
	BeliefStates new_beliefs;
	std::vector<Board> new_boards;
	for(std::vector<Belief*>::iterator i = table_.begin();
	    i != table_.end(); ++i) {
		if(progress != NULL && (i - table_.begin())%50000 == 0) {
			const int percent = ((i - table_.begin())*50)/table_.size();
			progress->PercentComplete(percent);
		}
		for(Belief* b = *i; b != NULL; b = b->next) {
			if(src != dst) {
				b->board.ApplyMove(src, dst);
			}
			b->board.LegalMoves(&new_boards);
			std::cerr << "found " << new_boards.size() << " legal moves...\n";
			for(std::vector<Board>::const_iterator j = new_boards.begin();
			    j != new_boards.end(); ++j) {
				if(j->ConsistentWith(known_board)) {
					new_beliefs.Add(*j);
				} else {
					//std::cerr << "INCONSISTENT: {{{" << *j << " }}} VS (((" << known_board << ")))\n";
				}
			}
			new_boards.clear();
		}
	}

	nbeliefs_ = new_beliefs.nbeliefs_;
	table_.swap(new_beliefs.table_);
}

MoveEvaluation BeliefStates::EvaluateMove(int src, int dst, const Board& visible_board) const
{
	const int MaxEvals = 1000;
	MoveEvaluation res;
	res.highest = 0;
	res.lowest = 0;
	res.sum = 0;
	res.samples = 0;

	bool first_time = true;
	for(std::vector<Belief*>::const_iterator i = table_.begin();
	    i != table_.end() && res.samples < MaxEvals; ++i) {
		for(const Belief* b = *i; b != NULL; b = b->next) {
			Board::MoveApplier a(const_cast<Board&>(b->board), src, dst);
			const int e = Eval(b->board, NULL, NULL, &visible_board);
			++res.samples;
			if(first_time) {
				first_time = false;
				res.highest = e;
				res.lowest = e;
			}
			res.sum += e;
			if(e > res.highest) {
				res.highest = e;
			}

			if(e < res.lowest) {
				res.lowest = e;
			}
		}
	}

	return res;
}
