#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include "board.hpp"
using namespace std;

struct TreeNode {
	Move base = Move(-1, -1);
	int score;
};

class Player {

private:
	Side side;
	Side opponentSide;
	Board *board;
public:
    Player(Side side1);
    ~Player();

    TreeNode minmax_tree(Board *b, int depth, bool finding_max, Move prev);
    Move *doMove(Move *opponentsMove, int msLeft);

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
};

#endif
