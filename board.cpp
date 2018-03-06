#include "board.hpp"
#include <iostream>
#include <algorithm>

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);
    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}


/*
 * Returns true if the game is finished; false otherwise. The game is finished
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            //std::cerr << side << std::endl;
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    // Passing is only legal if you have no moves.
    if (m == nullptr) return !hasMoves(side);
    
    int X = m->getX();
    int Y = m->getY();
    // Make sure the square hasn't already been taken.

    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A nullptr move means pass.
    if (m == nullptr) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}

/*
 * Returns a bitset containing all the possible moves that one of the sides
 * can make. Value is 1 if move is possible.
 */
bitset<64> Board::getMoves(Side side)
{
	bitset<64> moves;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			Move m(x, y);
			if (checkMove(&m, side))
			{
				moves.set(x + 8 * y);
			}
		}
	}
	return moves;
}

/*
 * Returns the number of corners that on player has
 */
int Board::getCorners(Side side)
{
	int num = 0;
	if (side == BLACK)
	{
		if (black[0])
		{
			num += 1;
		}
		if (black[7])
		{
			num += 1;
		}
		if (black[56])
		{
			num += 1;
		}
		if (black[63])
		{
			num += 1;
		}
	}
	else
	{
		if (taken[0] && !black[0])
		{
			num += 1;
		}
		if (taken[7] && !black[7])
		{
			num += 1;
		}
		if (taken[56] && !black[56])
		{
			num += 1;
		}
		if (taken[63] && !black[63])
		{
			num += 1;
		}
	}
	return num;
}

/*
 * Returns a heuristic score
 */
int Board::getScore(Side side, Side opponentSide, Move move)
{
	if (taken.count() > 60)
	{
		return count(side) - count(opponentSide);
	}
	int X = move.getX();
	int Y = move.getY();
	int c = 0;
	
	int mobility_factor = 150;
	int x_square = -50;
	int c_square = -30;
	int move_factor = 30;
	int corner;
	if (taken.count() < 30)
	{
		corner = 800;
	}
	else
	{
		corner = 100;
	}
	if ((X == 1 || X == 6) && (Y == 1 || Y == 6))
	{
		c += x_square;
	}
	else if ((X == 0 || X == 7) && (Y == 1 || Y == 6))
	{
		c += c_square;
	}
	else if ((X == 1 || X == 6) && (Y == 0 || Y == 7))
	{
		c += c_square;
	}
	
	if ((X == 0 || X == 7) && (Y == 0 || Y == 7))
	{
		c += corner;
	}
	
	int mobility = 0;
	int myMoves = getMoves(side).count();
	int opMoves = getMoves(opponentSide).count();
	if (myMoves + opMoves != 0)
	{
		mobility = mobility_factor * (myMoves - opMoves) / (myMoves + opMoves);
	}
	
	
	return mobility + c + myMoves * move_factor;
}


