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
	opponentSide = (side == BLACK) ? WHITE : BLACK;
}

/*
 * Destructor for the player.
 */
Player::~Player() {
	delete board;
}

TreeNode Player::minmax_tree(Board *b, int depth, Side playSide)
{
    TreeNode x;
    Side other = (playSide == BLACK) ? WHITE : BLACK;
    if (depth == 0) {
        x.score = b->getScore(playSide, other);
        return x;
    }
    else if (!(b->hasMoves(playSide))) {
        TreeNode v = minmax_tree(b, depth-1, other);
        return v;
    }
    /*
    else if (!finding_max && !(b->hasMoves(opponentSide))) {
        x.score = b->getScore(side, opponentSide);
        return x;
    }
    */
    
    if (side == playSide) {
        bitset<64> moves = b->getMoves(playSide);
        TreeNode best_move;
        best_move.score = -999999;
        int v;
        for (int i = 0; i < 64; i++) {
            if (moves[i]) {
                Board *b2 = board->copy();
                Move move(i % 8, i / 8);
                b2->doMove(&move, playSide);
                v = minmax_tree(b2, depth-1, other).score;
                if (v > best_move.score) {
                    best_move.score = v;
                    best_move.base = move;
                }
            }
        }
        return best_move;
    }
    else {
        bitset<64> moves = b->getMoves(opponentSide);
        TreeNode worst_move;
        worst_move.score = 999999;
        int v;
        for (int i = 0; i < 64; i++) {
            if (moves[i]) {
                Board *b2 = board->copy();
                Move move(i % 8, i / 8);
                b2->doMove(&move, opponentSide);
                v = minmax_tree(b2, depth-1, side).score;
                if (v < worst_move.score) {
                    worst_move.score = v;
                    worst_move.base = move;
                }
            }
        }
        return worst_move;
    }
    
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
    
    TreeNode mt;
    mt = minmax_tree(board, 6, side);
    Move best = mt.base;
    Move *m =  new Move(best.getX(), best.getY());
    board->doMove(m, side);
    return m;
    
    
    /*
    bitset<64> moves = board->getMoves(side);
    Move maxMove(-1,-1);
    int maxScore = -99999;
    for (int i = 0; i < 64; i++)
    {
    	if (moves[i])
     	{
     		Board *b = board->copy();
     		Move move(i % 8, i / 8);
     		b->doMove(&move, side);
     		int score = b->getScore(side, opponentSide);
     		if (score > maxScore)
     		{
     			maxMove = move;
     			maxScore = score;
     		}
     	}
    }
    Move *m =  new Move(maxMove.getX(), maxMove.getY());
    board->doMove(m, side);
    return m;
    */
}
