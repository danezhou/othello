#include "player.hpp"

// hey Dane

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 * Small change here...
 */
Player::Player(Side side1) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

	board = new Board();

	side = side1;
	if (side == WHITE)
	{
		opponentSide = BLACK;
	}
	else
	{
		opponentSide = WHITE;
	}
}

/*
 * Destructor for the player.
 */
Player::~Player() {
	delete board;
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    board->doMove(opponentsMove, opponentSide);
    if (!(board->hasMoves(side)))
    {
    	return nullptr;
    }
    bitset<64> moves = board->getMoves(side);
    int maxScore = -99999999;
    Move maxMove(-1,-1);
    for (int i = 0; i < 64; i++)
    {
    	if (moves[i])
    	{
    		Board *b = board->copy();
    		Move move(i % 8, i / 8);
    		b->doMove(&move, side);
    		int score = b->getScore(side, opponentSide, move);
    		if (score > maxScore)
    		{
    			maxMove = move;
    		}
    	}
    }
    Move *m =  new Move(maxMove.getX(), maxMove.getY());
    board->doMove(m, side);
    return m;
}
