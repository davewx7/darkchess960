#ifndef BOARD_HPP_INCLUDED
#define BOARD_HPP_INCLUDED

#include <vector>
#include <inttypes.h>
#include <iosfwd>
#include <set>

#include "square.hpp"

class Move;

class Board
{
public:
	Board();

	Square GetSquare(int pos) const { return squares_[pos]; }
	void VisibleBoard(Board* result) const;
	void VisibleLocs(std::set<int>* results) const;
	void VisibleLocs(uint64_t* bitmap, bool white) const;
	void LegalMoves(std::vector<Board>* results) const;
	void LegalMoves(std::vector<Move>* results) const;
	void LegalMovesFromSquare(std::vector<int>* results, int pos, bool include_protects=false) const;

	//make a move; returns true iff it's a castling move
	bool ApplyMove(int pos_from, int pos_to);
	void RevertCastle(int king_orig, int rook_orig);
	uint64_t Hash() const { uint64_t res = 0; for(uint64_t n = 0; n != 64; ++n) { if(!squares_[n].empty()) { res |= (1LL << n); } } return res; }
	bool Equal(const Board& b) const { return memcmp((const void*)&squares_[0], (const void*)&b.squares_[0], sizeof(squares_)) == 0 && bits_ == b.bits_; }
	bool ConsistentWith(const Board& b) const {
		for(int n = 0; n != 64; ++n) {
			if(b.squares_[n].is_unknown() == false && squares_[n] != b.squares_[n]) {
				return false;
			}
		}

		return true;
	}
	
	bool WhiteToMove() const { return whites_move_; }

	friend std::ostream& operator<<(std::ostream& s, const Board& b);

	friend class MoveApplier;

	class MoveApplier {
	public:
		MoveApplier(Board& b, int src, int dst)
		  : b_(&b),
		    from_(b.squares_[src]), to_(b.squares_[dst]),
		    src_(src), dst_(dst), bits_(b_->bits_)
		{
			castle_ = b_->ApplyMove(src, dst);
		}

		~MoveApplier() {
			if(b_ && src_ != dst_) {
				if(castle_) {
					b_->RevertCastle(src_, dst_);
				} else {
					b_->squares_[src_] = from_;
					b_->squares_[dst_] = to_;
				}
				b_->whites_move_ = !b_->whites_move_;
				b_->bits_ = bits_;
			}
		}

		void Commit() {
			b_ = NULL;
		}
	private:
		Board* b_;
		Square from_, to_;
		int src_, dst_;
		bool castle_;
		unsigned char bits_;
	};
private:
	Square squares_[64];
	bool whites_move_;
	unsigned char bits_;
	enum CASTLE_FLAG { WHITE_CASTLE_QUEEN, WHITE_CASTLE_KING,
	                   BLACK_CASTLE_QUEEN, BLACK_CASTLE_KING };
};

std::ostream& operator<<(std::ostream& s, const Board& b);

#endif
