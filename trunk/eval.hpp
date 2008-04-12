#ifndef EVAL_HPP_INCLUDED
#define EVAL_HPP_INCLUDED

class Board;

int Eval(const Board& b, const int* min=0, const int* max=0, const Board* visible_board=0);

#endif
