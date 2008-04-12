#ifndef BELIEFS_HPP_INCLUDED
#define BELIEFS_HPP_INCLUDED

#include <vector>

#include "board.hpp"

class PlayerProgressSink;

struct MoveEvaluation
{
	int highest;
	int lowest;
	int sum;
	int samples;
};

struct Belief
{
	Board board;
	Belief* next;
};

class BeliefStates
{
public:
	BeliefStates() : table_(1000000), nbeliefs_(0)
	{}
	~BeliefStates();
	void Add(const Board& b);
	void NextMove(int src, int dst, const Board& known_board, PlayerProgressSink* progress);
	int Size() const { return nbeliefs_; }
	MoveEvaluation EvaluateMove(int src, int dst, const Board& known_board) const;
private:
	BeliefStates(const BeliefStates&);
	void operator=(const BeliefStates);
	std::vector<Belief*> table_;
	int nbeliefs_;
};

#endif
