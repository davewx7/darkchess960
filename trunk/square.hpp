#ifndef SQUARE_HPP_INCLUDED
#define SQUARE_HPP_INCLUDED

#include <assert.h>
#include <ctype.h>

enum ChessMan {
	ChessManNone,
	ChessManPawn,
	ChessManKnight,
	ChessManBishop,
	ChessManRook,
	ChessManQueen,
	ChessManKing,
};

inline char ChessManChar(ChessMan c) {
	switch(c) {
	case ChessManNone: return '.';
	case ChessManPawn: return 'p';
	case ChessManKnight: return 'n';
	case ChessManBishop: return 'b';
	case ChessManRook: return 'r';
	case ChessManQueen: return 'q';
	case ChessManKing: return 'k';
	default: assert(false);
	}
}

class Square
{
public:
	Square() : s_(0)
	{}
	explicit Square(ChessMan type, bool side) {
		s_ = (side ? (1 << 7) : 0) + type;
	}

	static Square unknown() {
		Square s;
		s.s_ = 0xFF;
		return s;
	}

	char readable_char() const {
		if(is_unknown()) {
			return '?';
		} else if(empty()) {
			return '.';
		} else {
			char c = ChessManChar(chess_man());
			if(black()) {
				c = toupper(c);
			}

			return c;
		}
	}

	bool empty() const { return s_ == 0; }
	bool white() const { return (s_&(1 << 7)); }
	bool black() const { return !empty() && !white(); }
	bool is_chessman(ChessMan type) const { return s_&(1 << type); }
	bool is_pawn() const { return (s_&0x0F) == ChessManPawn; }
	bool is_knight() const { return (s_&0x0F) == ChessManKnight; }
	bool is_bishop() const { return (s_&0x0F) == ChessManBishop; }
	bool is_rook() const { return (s_&0x0F) == ChessManRook; }
	bool is_queen() const { return (s_&0x0F) == ChessManQueen; }
	bool is_king() const { return (s_&0x0F) == ChessManKing; }
	bool is_unknown() const { return s_ == 0xFF; }
	ChessMan chess_man() const { return (ChessMan)(s_&0x0F); }

	bool operator==(Square s) const { return s_ == s.s_; }
	bool operator!=(Square s) const { return s_ != s.s_; }
private:
	unsigned char s_;
};

struct Move
{
	int src;
	int dst;
};

#endif
