#ifndef BRAIN_H
#define BRAIN_H

#include <string>
#include "../Game/game.h"
#include <cstdlib>
#include <ctime>
#include <queue>
//#include "player.h"
//#include "game.h"
//#include "enemy.h"

struct Position {
    int row{};
    int col{};
};

class Brain
{
public:
    Brain();                               // Constructor
    int getNextMove(GameState &gamestate); // Returns the next move for the AI

    bool isWalkable(char tile);
    bool isObstacle(char tile);
    bool isTarget(char tile);
    std::vector<int> closestGoal(GameState &gamestate, int h, int w);
};


#endif // BRAIN_H2