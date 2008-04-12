#include <assert.h>
#include <iostream>

#include "board.hpp"
#include "eval.hpp"
#include "player.hpp"

namespace {

const int KnightValues[64] = {
-10, -6, -5, -5, -5, -5, -6, -10
 -6, -5, -3,  0,  0, -3, -5, -6,
 -2,  2,  4,  2,  2,  4,  2, -2,
  0,  4,  4,  4,  4,  4,  4,  0,
  0,  4,  4,  4,  4,  4,  4,  0,
 -2,  2,  4,  2,  2,  4,  2, -2,
 -6, -5, -3,  0,  0, -3, -5, -6,
-10, -6, -5, -5, -5, -5, -6, -10
};

const int BishopValues[64] = {
4, 0, -2, -2, -2, -2, 0, 4,
4, 5,  0,  0,  0,  0, 5, 4,
4, 4,  4,  0,  0,  4, 4, 4,
3, 2,  4,  0,  0,  4, 2, 3,
3, 3,  4,  0,  0,  4, 3, 3,
4, 4,  4,  0,  0,  4, 4, 4,
4, 5,  0,  0,  0,  0, 5, 4,
4, 0, -2, -2, -2, -2, 0, 4,
};

const int QueenValues[64] = {
0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 1, 1, 1, 1, 1, 1,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2, 2, 2, 2, 2, 2,
1, 1, 1, 1, 1, 1, 1, 1,
0, 0, 0, 0, 0, 0, 0, 0,
};

const int PawnValues[64] = {
0, 0, 0, 0, 0, 0, 0, 0,
2, 2, 1, 1, 1, 1, 2, 2,
2, 1, 1, 2, 2, 1, 1, 2,
2, 1, 2, 3, 3, 2, 1, 2,
3, 2, 4, 4, 4, 4, 2, 3,
5, 5, 5, 5, 5, 5, 5, 5,
7, 7, 7, 7, 7, 7, 7, 7,
9, 9, 9, 9, 9, 9, 9, 9,
};

const int EmptyValues[64] = {
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
};

inline int ChessManValue(ChessMan m, int loc)
{
	switch(m) {
	case ChessManNone: return EmptyValues[loc]*10;
	case ChessManPawn: return 60 + PawnValues[loc]*10;
	case ChessManKnight: return 300 + KnightValues[loc]*20;
	case ChessManBishop: return 300 + BishopValues[loc]*10;
	case ChessManRook: return 500;
	case ChessManQueen: return 900 + QueenValues[loc]*20;
	case ChessManKing: return 5000;
	}
}

inline int ChessManPawnDiagonal(ChessMan m, int loc)
{
	switch(m) {
	case ChessManNone: return 0;
	case ChessManPawn: return 30;
	case ChessManKnight: return 0;
	case ChessManBishop: return -80;
	case ChessManRook: return 10;
	case ChessManQueen: return -50;
	case ChessManKing: return 200;
	}
}

int EvalSide(const Board& b, bool white)
{
	int forward = white ? 1 : -1;
	int result = 0;
	uint64_t visible;
	b.VisibleLocs(&visible, white);
	for(uint64_t n = 0; n != 64LL; ++n) {
		const int pos = white ? n : (7 - n/8)*8 + n%8;
		assert(pos >= 0 && pos < 64);
		const Square s = b.GetSquare(n);
		if(s.empty() == false && s.white() == white) {
			result += ChessManValue(s.chess_man(), pos);

			if(s.is_pawn()) {
				int diagonal_behind1 = n - forward*8 + 1;
				int diagonal_behind2 = n - forward*8 - 1;
				if(diagonal_behind1 >= 0 && diagonal_behind1 < 64) {
					Square s = b.GetSquare(diagonal_behind1);
					if(s.empty() == false && s.white() == white) {
						result += ChessManPawnDiagonal(s.chess_man(), pos);
					}
				}

				if(diagonal_behind2 >= 0 && diagonal_behind2 < 64) {
					Square s = b.GetSquare(diagonal_behind2);
					if(s.empty() == false && s.white() == white) {
						result += ChessManPawnDiagonal(s.chess_man(), pos);
					}
				}
			}
		}

		if(visible&(1LL << n)) {
			result += (50 + ChessManValue(s.chess_man(), n))/3;
		}
	}

	return result;
}
}

int depth = 0;

int Eval(const Board& b, const int* min, const int* max, const Board* visible_board)
{
	++depth;
	const bool white_move = b.WhiteToMove();
	int res = EvalSide(b, true) - EvalSide(b, false);

	//Look for all possible captures and evaluate them in detail.
	std::vector<Move> moves;
	if(depth < 4 && (min == NULL || res > *min) && (max == NULL || res < *max)) {
		b.LegalMoves(&moves);
	}
	for(std::vector<Move>::const_iterator i = moves.begin();
	    i != moves.end(); ++i) {
		if(b.GetSquare(i->dst).empty() == false) {
			Board::MoveApplier m(const_cast<Board&>(b), i->src, i->dst);
			const int eval_capture = Eval(b, white_move ? &res : NULL,
			                                 !white_move ? &res : NULL);
			if(white_move && eval_capture > res ||
			   !white_move && eval_capture < res) {
			   res = eval_capture;
			}
		}
	}

	--depth;

	return res;
}
