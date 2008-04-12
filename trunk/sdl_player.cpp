#include <iostream>
#include <SDL/SDL.h>
#include <map>
#include <string>

#include "SDL_image.h"

#include "sdl_player.hpp"
#include "square.hpp"

namespace {
std::map<std::string,SDL_Surface*> surfaces;

SDL_Surface* GetSurface(const std::string& name)
{
	SDL_Surface*& surf = surfaces[name];
	if(surf == NULL) {
		surf = IMG_Load(name.c_str());
		if(surf == NULL) {
			std::cerr << "Could not load image: '" << name << "'\n";
			exit(0);
		}
	}

	return surf;
}
}

SDLPlayer::SDLPlayer(const Board& b) : selection_(-1)
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Could not init SDL\n";
		exit(0);
	}

	SDL_Surface* s = GetSurface("Chess_d44.png");

	square_width_ = s->w;
	square_height_ = s->h;

	if(SDL_SetVideoMode(s->w*11, s->h*8, 0, 0) == NULL) {
		std::cerr << "could not set video mode\n";
		exit(0);
	}
}

SDLPlayer::~SDLPlayer()
{
	SDL_Quit();
}

void SDLPlayer::OpponentMove()
{
}

Move SDLPlayer::MakeMove(const Board& b)
{
	bool end_game = true;
	for(int n = 0; n != 64; ++n) {
		Square s = b.GetSquare(n);
		if(s.is_king() && s.white() == b.WhiteToMove()) {
			end_game = false;
			break;
		}
	}

	selection_ = -1;
	while(true) {
		Draw(b);
		SDL_Flip(SDL_GetVideoSurface());
		SDL_Delay(10);
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				SDL_Quit();
				exit(0);
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE) {
					SDL_Quit();
					exit(0);
				}
			case SDL_MOUSEBUTTONDOWN: {
				if(end_game) {
					throw EndGameException();
				}
				int mousex, mousey;
				SDL_GetMouseState(&mousex, &mousey);
				if(event.button.button == SDL_BUTTON_LEFT) {
					const int x = mousex/square_width_;
					const int y = mousey/square_height_;
					const int pos = (7 - y)*8 + x;
					std::cerr << "click on " << x << ", " << y << " -> " << pos << "\n";
					if(pos >= 0 && pos < 64) {
						std::vector<Move> moves;
						b.LegalMoves(&moves);
						if(selection_ >= 0) {
							for(std::vector<Move>::const_iterator i = moves.begin(); i != moves.end(); ++i) {
								if(i->src == selection_ && i->dst == pos) {
									DisplayMove(b, *i);
									if(b.GetSquare(i->dst).is_king()) {
										end_game = true;
										break;
									}
									return *i;
								}
							}
						}

						for(std::vector<Move>::const_iterator i = moves.begin(); i != moves.end(); ++i) {
							if(i->src == pos) {
								selection_ = pos;
								break;
							}
						}
					}
				} else {
					selection_ = -1;
				}
			}
			}
		}
	}

	assert(false);
}

namespace {
std::string SquareImage(Square sq, int n)
{
	std::string key = "Chess_";
	if(sq.is_unknown() == false && sq.empty() == false) {
		key += ChessManChar(sq.chess_man());
		key += sq.white() ? "l" : "d";
	}

	key += ((n/8 + n)%2) == 1 ? "l" : "d";
	key += "44.png";
	return key;
}
}

void SDLPlayer::Draw(const Board& b)
{
	SDL_Surface* fb = SDL_GetVideoSurface();
	int x = 0, y = square_height_*7;
	for(int n = 0; n != 64; ++n) {
		Square sq = b.GetSquare(n);
		std::string key = SquareImage(sq, n);
		SDL_Surface* s = GetSurface(key);

		SDL_Rect rect = {x, y, 0, 0};
		SDL_BlitSurface(s, NULL, fb, &rect);

		if(n == selection_ || sq.is_unknown()) {
			s = GetSurface(sq.is_unknown() ? "dark.png" : "highlight.png");

			SDL_BlitSurface(s, NULL, fb, &rect);
		}

		x += square_width_;
		if(x == square_width_*8) {
			x = 0;
			y -= square_height_;
		}
	}

	x = square_width_*9;
	y = 0;
	for(int n = 0; n != captures_.size(); ++n) {
		SDL_Surface* s = GetSurface(SquareImage(captures_[n], capture_pos_[n]));

		SDL_Rect rect = {x, y, 0, 0};
		SDL_BlitSurface(s, NULL, fb, &rect);

		y += square_height_;
		if(y == square_height_*8) {
			y = 0;
			x += square_width_;
		}
	}

}

void SDLPlayer::DisplayMove(const Board& b, const Move& move)
{
	const Square src = b.GetSquare(move.src);
	const Square dst = b.GetSquare(move.dst);
	if(!dst.empty() && dst.white() != src.white()) {
		captures_.push_back(dst);
		capture_pos_.push_back(move.dst);
	}

	last_board_ = b;
	last_board_.ApplyMove(move.src, move.dst);
	Draw(b);
	SDL_Flip(SDL_GetVideoSurface());
}

void SDLPlayer::PercentComplete(int num)
{
	Draw(last_board_);
	SDL_Surface* fb = SDL_GetVideoSurface();
	SDL_Rect rect = {10, 10, 212, 20};
	SDL_FillRect(fb, &rect, SDL_MapRGBA(fb->format, 0, 0, 0, 100));
	SDL_Rect filled = { 11, 11, num*2, 18};
	SDL_FillRect(fb, &filled, SDL_MapRGBA(fb->format, 255, 0, 0, 100));
	SDL_Flip(SDL_GetVideoSurface());
}
