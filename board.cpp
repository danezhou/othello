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
 * Returns the standard inner product of 2 vectors
 */
int Board::dotProduct(int * pieces, int * weights)
{
	int sum = 0;
	for (int i = 0; i < 64; i++)
	{
		int product = pieces[i];
		product *= weights[i];
		sum += product;
	}
	return sum;
}

/*
 * Sets every value in the array to 0, 1, or -1 based on if it is empty, has my
 * piece, or the other player's piece respectively.
 */
int* Board::fillPieces(Side side)
{
	int *pieces = new int[64];
	int factor = 1;
	if (side == WHITE)
	{
		factor = -1;
	}
	for (int i = 0; i < 64; i++)
	{
		if (!taken[i])
		{
			pieces[i] = 0;
		}
		else if(black[i])
		{
			pieces[i] = factor * 1;
		}
		else
		{
			pieces[i] = factor * -1;
		}
	}
	return pieces;
}

/*
 * Returns a heuristic score
 */
int Board::getScore(Side side, Side opponentSide)
{
	if (count(side) == 0)
	{
		return -999999;
	}
	if (taken.count() > 62)
	{
		return count(side) - count(opponentSide);
	}
	int *pieces = fillPieces(side);
	int weights[64] = {100,-20,10,5,5,10,-20,100,-20,-50,1,1,1,1,-50,-20,10,1,2,2,2,2,
	1,10,5,1,2,2,2,2,1,5,5,1,2,2,2,2,1,5,10,1,2,2,2,2,1,10,-20,-50,
    1,1,1,1,-50,-20,100,-20,10,5,5,10,-20,100};

    if (taken[0]) {
        weights[1] = 20;
        weights[8] = 20;
        weights[9] = -10;
    }
    if (taken[7]) {
        weights[6] = 20;
        weights[15] = 20;
        weights[14] = -10;
    }
    if (taken[56]) {
        weights[48] = 20;
        weights[57] = 20;
        weights[49] = -10;
    }
    if (taken[63]) {
        weights[62] = 20;
        weights[55] = 20;
        weights[54] = -10;
    }

	int weighted_sum = dotProduct(pieces, weights);
	delete pieces;
	
	double mobility_factor = 10.;
	int weighted_factor = 1;

	
	double mobility = 0;
	int myMoves = getMoves(side).count();
	int opMoves = getMoves(opponentSide).count();
	if (myMoves + opMoves != 0)
	{
		mobility = (mobility_factor * (myMoves - opMoves)) / (myMoves + opMoves);
	}
	
	return mobility + weighted_factor * weighted_sum;
}


