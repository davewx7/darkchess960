#include <iostream>
#include <sstream>
#include <vector>

#include "board.hpp"
#include "player.hpp"

Board::Board() : whites_move_(true), bits_(0x0)
{
	//static const ChessMan HomeRow[8] = { ChessManRook, ChessManKnight, ChessManBishop, ChessManQueen, ChessManKing, ChessManBishop, ChessManKnight, ChessManRook };
	ChessMan HomeRow[8] = { ChessManNone, ChessManNone, ChessManNone, ChessManNone, ChessManNone, ChessManNone, ChessManNone, ChessManNone };

	std::vector<int> indexes;
	for(int n = 0; n != 8; ++n) {
		indexes.push_back(n);
	}

	const int bishop1 = (rand()%4)*2;
	const int bishop2 = (rand()%4)*2 + 1;
	HomeRow[bishop1] = ChessManBishop;
	HomeRow[bishop2] = ChessManBishop;

	indexes.erase(std::find(indexes.begin(), indexes.end(), bishop1));
	indexes.erase(std::find(indexes.begin(), indexes.end(), bishop2));

	const int queen = rand()%6;
	HomeRow[indexes[queen]] = ChessManQueen;
	indexes.erase(indexes.begin() + queen);
	const int knight1 = rand()%5;
	HomeRow[indexes[knight1]] = ChessManKnight;
	indexes.erase(indexes.begin() + knight1);
	const int knight2 = rand()%4;
	HomeRow[indexes[knight2]] = ChessManKnight;
	indexes.erase(indexes.begin() + knight2);
	HomeRow[indexes[0]] = ChessManRook;
	HomeRow[indexes[1]] = ChessManKing;
	HomeRow[indexes[2]] = ChessManRook;

	for(int i = 0; i != 8; ++i) {
		squares_[i] = Square(HomeRow[i], true);
		squares_[56 + i] = Square(HomeRow[i], false);
		squares_[i + 8] = Square(ChessManPawn, true);
		squares_[i + 48] = Square(ChessManPawn, false);
	}
}

void Board::VisibleBoard(Board* result) const
{
	*result = *this;
	std::set<int> visible;
	VisibleLocs(&visible);
	for(int n = 0; n != 64; ++n) {
		if(visible.count(n) == 0) {
			result->squares_[n] = Square::unknown();
		}
	}
}

void Board::VisibleLocs(std::set<int>* results) const
{
	std::vector<int> moves;
	for(int n = 0; n != 64; ++n) {
		if(squares_[n].empty() == false && squares_[n].white() == whites_move_) {
			results->insert(n);
			moves.clear();
			LegalMovesFromSquare(&moves, n, true);
			for(std::vector<int>::const_iterator i = moves.begin(); i != moves.end(); ++i) {
				results->insert(*i);
			}
		}
	}
}

void Board::VisibleLocs(uint64_t* bitmap, bool white) const
{
	*bitmap = 0LL;
	std::vector<int> moves;
	for(int n = 0; n != 64; ++n) {
		if(squares_[n].empty() == false && squares_[n].white() == white) {
			*bitmap |= 1LL << int64_t(n);
			moves.clear();
			LegalMovesFromSquare(&moves, n, true);
			for(std::vector<int>::const_iterator i = moves.begin(); i != moves.end(); ++i) {
				*bitmap |= 1LL << int64_t(*i);
			}
		}
	}
}

void Board::LegalMoves(std::vector<Board>* results) const
{
	std::vector<int> moves;
	for(int n = 0; n != 64; ++n) {
		if(squares_[n].empty() == false && squares_[n].white() == whites_move_) {
			moves.clear();
			LegalMovesFromSquare(&moves, n);
			for(std::vector<int>::const_iterator i = moves.begin(); i != moves.end(); ++i) {
				results->push_back(*this);
				results->back().ApplyMove(n, *i);
			}
		}
	}
}

void Board::LegalMoves(std::vector<Move>* results) const
{
	std::vector<int> moves;
	for(int n = 0; n != 64; ++n) {
		if(squares_[n].empty() == false && squares_[n].white() == whites_move_) {
			moves.clear();
			LegalMovesFromSquare(&moves, n);
			Move m;
			m.src = n;
			for(std::vector<int>::const_iterator i = moves.begin(); i != moves.end(); ++i) {
				m.dst = *i;
				results->push_back(m);
			}
		}
	}
}

void Board::LegalMovesFromSquare(std::vector<int>* results, int pos, bool include_protects) const
{
	switch(squares_[pos].chess_man()) {
	case ChessManPawn: {
		int dir = squares_[pos].white() ? 1 : -1;
		int forward = pos + 8*dir;
		if(forward >= 0 && forward < 64) {
			if(squares_[forward].empty()) {
				results->push_back(forward);

				if(dir > 0 && (pos/8) == 1 || dir < 0 && (pos/8) == 6) {
					const int forward2 = forward + 8*dir;
					if(squares_[forward2].empty()) {
						results->push_back(forward2);
					}
				}
			}

			//not on the leftmost column
			if((forward%8) != 0) {
				if(include_protects || squares_[forward-1].empty() == false && squares_[forward-1].white() != squares_[pos].white()) {
					results->push_back(forward - 1);
				}
			}

			//not on the rightmost column
			if((forward%8) != 7) {
				if(include_protects || squares_[forward+1].empty() == false && squares_[forward+1].white() != squares_[pos].white()) {
					results->push_back(forward + 1);
				}
			}
		}
		break;
	}
		
	case ChessManKnight: {
		const int moves[] = { (pos - 8*2) - 1, (pos - 8*2) + 1,
		                      (pos - 8) - 2, (pos - 8) + 2,
		                      (pos + 8*2) - 1, (pos + 8*2) + 1,
							  (pos + 8) - 2, (pos + 8) + 2 };
		for(int n = 0; n != sizeof(moves)/sizeof(*moves); ++n) {
			const int move = moves[n];
			if(move < 0 || move >= 64) {
				continue;
			}

			const int xdiff = move%8 - pos%8;
			if(xdiff < -2 || xdiff > 2) {
				continue;
			}

			if(include_protects || squares_[move].empty() || squares_[move].white() != squares_[pos].white()) {
				results->push_back(move);
			}
		}
		break;
	}
	case ChessManQueen:
	case ChessManBishop: {
		for(int dst = pos - 9; dst >= 0 && (dst%8) != 7; dst -= 9) {
			if(!include_protects && (squares_[dst].empty() == false && squares_[dst].white() == squares_[pos].white())) {
				break;
			}

			results->push_back(dst);

			if(squares_[dst].empty() == false) {
				break;
			}
		}

		for(int dst = pos - 7; dst >= 0 && (dst%8) != 0; dst -= 7) {
			if(!include_protects && (squares_[dst].empty() == false && squares_[dst].white() == squares_[pos].white())) {
				break;
			}

			results->push_back(dst);

			if(squares_[dst].empty() == false) {
				break;
			}
		}

		for(int dst = pos + 7; dst < 64 && (dst%8) != 7; dst += 7) {
			if(!include_protects && (squares_[dst].empty() == false && squares_[dst].white() == squares_[pos].white())) {
				break;
			}

			results->push_back(dst);

			if(squares_[dst].empty() == false) {
				break;
			}
		}

		for(int dst = pos + 9; dst < 64 && (dst%8) != 0; dst += 9) {
			if(!include_protects && (squares_[dst].empty() == false && squares_[dst].white() == squares_[pos].white())) {
				break;
			}

			results->push_back(dst);

			if(squares_[dst].empty() == false) {
				break;
			}
		}
	}
	if(squares_[pos].chess_man() == ChessManBishop) {
		break;
	}
	case ChessManRook: {
		for(int dst = pos - 8; dst >= 0; dst -= 8) {
			if(!include_protects && (squares_[dst].empty() == false && squares_[dst].white() == squares_[pos].white())) {
				break;
			}

			results->push_back(dst);

			if(squares_[dst].empty() == false) {
				break;
			}
		}

		for(int dst = pos + 8; dst < 64; dst += 8) {
			if(!include_protects && (squares_[dst].empty() == false && squares_[dst].white() == squares_[pos].white())) {
				break;
			}

			results->push_back(dst);

			if(squares_[dst].empty() == false) {
				break;
			}
		}

		for(int dst = pos+1; (dst%8) != 0 ; ++dst) {
			if(!include_protects && (squares_[dst].empty() == false && squares_[dst].white() == squares_[pos].white())) {
				break;
			}

			results->push_back(dst);

			if(squares_[dst].empty() == false) {
				break;
			}
		}

		for(int dst = pos-1; dst >= 0 && (dst%8) != 7 ; --dst) {
			if(!include_protects && (squares_[dst].empty() == false && squares_[dst].white() == squares_[pos].white())) {
				break;
			}

			results->push_back(dst);

			if(squares_[dst].empty() == false) {
				break;
			}
		}

		break;
	}
	case ChessManKing: {
		const int moves[] = { pos - 9, pos - 8, pos - 7, pos - 1, pos + 1,
		                      pos + 9, pos + 8, pos + 7 };
		for(int n = 0; n != sizeof(moves)/sizeof(*moves); ++n) {
			const int move = moves[n];
			if(move < 0 || move >= 64) {
				continue;
			}

			const int xdiff = move%8 - pos%8;
			if(xdiff < -1 || xdiff > 1) {
				continue;
			}

			if(include_protects || squares_[move].empty() || squares_[move].white() != squares_[pos].white()) {
				results->push_back(move);
			}
		}

		const bool white = squares_[pos].white();
		if(white) {
			if((bits_&(1 << WHITE_CASTLE_QUEEN)) == 0) {
				int rook = -1;
				for(int n = pos - 1; n >= 0; --n) {
					if(squares_[n].is_rook() && squares_[n].white()) {
						rook = n;
						break;
					}
				}

				if(rook != -1) {
					if((squares_[2].empty() || rook == 2 || pos == 2) &&
					   (squares_[3].empty() || rook == 3 || pos == 3)) {
					   results->push_back(rook);
					}
				}
			}

			if((bits_&(1 << WHITE_CASTLE_KING)) == 0) {
				int rook = -1;
				for(int n = pos + 1; n < 8; ++n) {
					if(squares_[n].is_rook() && squares_[n].white()) {
						rook = n;
						break;
					}
				}

				if(rook != -1) {
					if((squares_[5].empty() || rook == 5 || pos == 5) &&
					   (squares_[6].empty() || rook == 6 || pos == 6)) {
					   results->push_back(rook);
					}
				}
			}
		} else {
			if((bits_&(1 << BLACK_CASTLE_QUEEN)) == 0) {
				int rook = -1;
				for(int n = pos - 1; n >= 56; --n) {
					if(squares_[n].is_rook() && squares_[n].black()) {
						rook = n;
						break;
					}
				}

				if(rook != -1) {
					if((squares_[58].empty() || rook == 58 || pos == 58) &&
					   (squares_[59].empty() || rook == 59 || pos == 59)) {
					   results->push_back(rook);
					}
				}
			}

			if((bits_&(1 << BLACK_CASTLE_KING)) == 0) {
				int rook = -1;
				for(int n = pos + 1; n < 64; ++n) {
					if(squares_[n].is_rook() && squares_[n].black()) {
						rook = n;
						break;
					}
				}

				if(rook != -1) {
					if((squares_[61].empty() || rook == 61 || pos == 61) &&
					   (squares_[62].empty() || rook == 62 || pos == 62)) {
					   results->push_back(rook);
					}
				}
			}
		}
		break;
	}
	}
}

bool Board::ApplyMove(int pos_from, int pos_to)
{
	if(pos_from != pos_to) {
		if(squares_[pos_from].chess_man() == ChessManKing) {
			// disable castling
			bits_ |= (WHITE_CASTLE_QUEEN|WHITE_CASTLE_KING) << (squares_[pos_from].white() ? 0 : 2);

			// handle castling. We assume that the move has been checked
			// for legality and don't do any checking.
			if(squares_[pos_to].empty() == false &&
			   squares_[pos_to].white() == squares_[pos_from].white() &&
			   squares_[pos_to].chess_man() == ChessManRook) {
				const bool side = squares_[pos_to].white();
				int row_base = pos_to - pos_to%8;
				int rook_dst = pos_to < pos_from ? 3 : 5;
				int king_dst = pos_to < pos_from ? 2 : 6;
				squares_[pos_to] = Square();
				squares_[pos_from] = Square();
				squares_[row_base + rook_dst] = Square(ChessManRook, side);
				squares_[row_base + king_dst] = Square(ChessManKing, side);
				whites_move_ = !whites_move_;
				return true;
			}
		}

		if(squares_[pos_from].chess_man() == ChessManRook) {
			// check if this disables castling
			const unsigned char can_castle_mask = (WHITE_CASTLE_QUEEN|WHITE_CASTLE_KING) << (squares_[pos_from].white() ? 0 : 2);
			if(bits_&can_castle_mask != can_castle_mask) {
			
				//find where the king is to determine if this is the king
				//or queen side rook
				int king = -1;
				int beg = pos_from - pos_from%8;
				int end = beg + 8;
				for(int n = beg; n != end; ++n) {
					if(squares_[n].chess_man() == ChessManKing &&
					   squares_[n].white() == squares_[pos_from].white()) {
					   king = n;
					}
				}

				if(king != -1) {
					if(pos_from < king) {
						bits_ |= WHITE_CASTLE_QUEEN << (squares_[pos_from].white() ? 0 : 2);
					} else {
						bits_ |= WHITE_CASTLE_KING << (squares_[pos_from].white() ? 0 : 2);
					}
				}
			}
		}

		squares_[pos_to] = squares_[pos_from];
		squares_[pos_from] = Square();

		// handle queening
		if(squares_[pos_to].is_pawn()) {
			if(squares_[pos_to].white() && pos_to/8 == 7 ||
			   squares_[pos_to].black() && pos_to/8 == 0) {
			   squares_[pos_to] = Square(ChessManQueen, squares_[pos_to].white());
			}
		}
	}
	whites_move_ = !whites_move_;
	return false;
}

void Board::RevertCastle(int king_orig, int rook_orig)
{
	const bool side = king_orig < 8;
	int row_base = king_orig - king_orig%8;
	int rook_dst = row_base + (rook_orig < king_orig ? 3 : 5);
	int king_dst = row_base + (rook_orig < king_orig ? 2 : 6);
	squares_[rook_dst] = Square();
	squares_[king_dst] = Square();
	squares_[king_orig] = Square(ChessManKing, side);
	squares_[rook_orig] = Square(ChessManRook, side);
}

std::ostream& operator<<(std::ostream& s, const Board& b)
{
	for(int row = 0; row != 8; ++row) {
		s << "\n";
		for(int col = 0; col != 8; ++col) {
			s << b.squares_[64 - (row+1)*8 + col].readable_char();
		}
	}

	s << "\n";
}
