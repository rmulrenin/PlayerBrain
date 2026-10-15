#include "brain.h"

Brain::Brain()
{
}

bool Brain::isWalkable(char tile) {
    if (tile == ' ' || tile == '0' || tile == 'T' || tile == 'w' || tile == 'A' || tile == 'B') {
        return true;
    } else {
        return false;
    }
}

bool Brain::isObstacle(char tile) {
    if (tile == '+' || tile == 'X' || tile == 'D') {
        return true;
    } else {
        return false;
    }
}

bool Brain::isTarget(char tile) {
    if (tile == '0' || tile == 'A' || tile == 'B' || tile == 'T' || tile == 'w') {
        return true;
    } else {
        return false;
    }
}

// find closest goal using bfs algorithm
std::vector<int> Brain::closestGoal(GameState &gamestate, int h, int w) {
    int start_row = h;
    int start_col = w;

    int rows = gamestate.vision.size();
    int cols = gamestate.vision.at(0).size();

    // vector of visited booleans
    std::vector<std::vector<bool>> visited;
    for (size_t i{0}; i < gamestate.vision.size(); i++) {
        std::vector<bool> row;
        for (size_t j{0}; j < gamestate.vision.at(i).size(); j++) {
            row.push_back(false);
        }
        visited.push_back(row);
    }

    std::queue<Position> q;
    q.push(Position{start_row, start_col});
    visited.at(start_row).at(start_col) = true;

    while (!q.empty()) {
        Position current{q.front()};
        int row = current.row;
        int col = current.col;
        q.pop();

        if (isTarget(gamestate.vision.at(row).at(col))) {
            return {row, col};
        }
        std::vector<std::vector<int>> neighbors;
        neighbors.push_back({row - 1, col}); // up
        neighbors.push_back({row + 1, col}); // down
        neighbors.push_back({row, col - 1}); // left
        neighbors.push_back({row, col + 1}); // right

        for (auto &neighbor : neighbors) {
            int new_row = neighbor.at(0);
            int new_col = neighbor.at(1);
            if (new_row >= rows || new_col >= cols || new_row < 0 || new_col < 0) {
                continue;
            }
            else if (isObstacle(gamestate.vision.at(new_row).at(new_col))) {
                continue; // issue here, vision is only obstacles for stage 4
            }
            if (!visited.at(new_row).at(new_col)) {
                visited.at(new_row).at(new_col) = true;
                q.push(Position{new_row, new_col});
            }
        }
    }
    return {0, 0};
}

int Brain::getNextMove(GameState &gamestate)
{
    int nothing{0};
    int up{1};
    int left{2};
    int down{3};
    int right{4};
    char wall = '+';
    int h{};
    int w{};

    // facing
    char north = '^';
    char east = '>';
    char west = '<';
    char south = 'v';

    // player position in vision
    for (int i{0}; i < static_cast<int>(gamestate.vision.size()); i++) {
        for (int j{0}; j < static_cast<int>(gamestate.vision.at(i).size()); j++) {
            if (gamestate.vision.at(i).at(j) == north || gamestate.vision.at(i).at(j) == south
                || gamestate.vision.at(i).at(j) == east || gamestate.vision.at(i).at(j) == west) {
                    h = i;
                    w = j;
                }
        }
    }

    // visited positions
    std::vector<std::vector<bool>> visited;
    for (size_t i{0}; i < gamestate.vision.size(); i++) {
        std::vector<bool> row;
        for (size_t j{0}; j < gamestate.vision.at(i).size(); j++) {
            row.push_back(false);
        }
        visited.push_back(row);
    }

    char direction = gamestate.vision.at(h).at(w);

    // AI logic to determine the next move based on the gamestate
    // STAGE 0
    if (gamestate.stage == 0) {
        // hug the left wall
        if (direction == east && gamestate.vision.at(h - 1).at(w) == wall) {    // east & wall above
            if (gamestate.vision.at(h).at(w + 1) != wall) { // wall not in front
                return right;
            } else {    
                return down;
            }
        }
        if (direction == south && gamestate.vision.at(h).at(w + 1) == wall) {   // south & wall right
            if (gamestate.vision.at(h + 1).at(w) != wall) {
                return down;
            } else {
                return right;
            }
        }
        if (direction == west && gamestate.vision.at(h).at(w + 1) == wall) {    // west & wall below
            if (gamestate.vision.at(h).at(w - 1) != wall) {
                return left;
            }
        }
        if (direction == north && gamestate.vision.at(h).at(w - 1) == wall) {   // north & wall left
            if (gamestate.vision.at(h - 1).at(w) != wall) {
                return up;
            } else {
                return right;
            }
        }

        // diagonal cases
        if (direction == south && gamestate.vision.at(h - 1).at(w + 1) == wall) {
            if (gamestate.vision.at(h).at(w + 1) != wall) {
                return right;
            }
        }
        if (direction == east && gamestate.vision.at(h - 1).at(w - 1) == wall) {
            if (gamestate.vision.at(h - 1).at(w ) != wall && visited.at(h - 1).at(w) == false) {
                return up;
            } else {
                return right;
            }
        }
    }
    // END STAGE 0

    // STAGE 01
    if (gamestate.stage == 1) {
        std::vector<int> target = closestGoal(gamestate, h, w);
        int target_h = target.at(0);
        int target_w = target.at(1);

        int distance_h{0};
        int distance_w{0};

        if (target_h != 0 || target_w != 0) {
            distance_h = h - target_h;
            distance_w = target_w - w;
        }

        if (isTarget(gamestate.vision.at(h - 1).at(w))) { // target directly above
            return up;
        }
        else if (isTarget(gamestate.vision.at(h + 1).at(w))) {  // target directly below
            return down;
        }
        else if (isTarget(gamestate.vision.at(h).at(w + 1))) {  // target directly right
            return right;
        }
        else if (isTarget(gamestate.vision.at(h).at(w - 1))) {  // target directly left
            return left;
        }

        // prioritize moving up / down
        if (distance_h != 0) {
            if (distance_h > 0) {
                if (isWalkable(gamestate.vision.at(h - 1).at(w))) {
                    return up;
                }
                else if (isWalkable(gamestate.vision.at(h).at(w + 1))){
                    return right;
                }
            } 
            else if (distance_h < 0) {
                if (isWalkable(gamestate.vision.at(h + 1).at(w))) {
                    return down;
                }
                else if (isWalkable(gamestate.vision.at(h).at(w + 1))){
                    return right;
                }
            }
        }
        // then move left / right
        if (distance_w != 0) {
            if (distance_w > 0 && isWalkable(gamestate.vision.at(h).at(w + 1))) {
                return right;

            } else if (distance_w < 0 && isWalkable(gamestate.vision.at(h).at(w - 1))) {
                return left;
            }
        }
        if (distance_h == 0 && distance_w == 0) {
            if (isWalkable(gamestate.vision.at(h).at(w + 1))) {
                return right;
            } else {
                return down;
            }
        }
        return down;
    }
    // END STAGE 01

    // STAGE 02
    if (gamestate.stage == 2) {
        // hug right wall
        if (direction == north) {
            if (isObstacle(gamestate.vision.at(h).at(w + 1)) && isWalkable(gamestate.vision.at(h - 1).at(w))) { // wall east, clear north
                return up;
            }
            else if (isObstacle(gamestate.vision.at(h).at(w + 1)) && isObstacle(gamestate.vision.at(h - 1).at(w)) 
            && isWalkable(gamestate.vision.at(h).at(w - 1))) { // wall east & north, clear west
                return left;
            }
            else if (isObstacle(gamestate.vision.at(h + 1).at(w + 1)) && isWalkable(gamestate.vision.at(h).at(w + 1))) { // diagonal case, wall southeast, clear east
                return right;
            }
            else {
                return nothing;
            }
        }

        if (direction == south) {
            if (isObstacle(gamestate.vision.at(h).at(w - 1)) && isWalkable(gamestate.vision.at(h + 1).at(w))) { // wall west, clear south
                return down;
            }
            else if (isObstacle(gamestate.vision.at(h).at(w - 1)) && isObstacle(gamestate.vision.at(h + 1).at(w)) 
            && isWalkable(gamestate.vision.at(h).at(w + 1))) { // wall west & south, clear east
                return right;
            }
            else {
                return nothing;
            }
        }

        if (direction == east) {
            if (isObstacle(gamestate.vision.at(h + 1).at(w)) && isWalkable(gamestate.vision.at(h).at(w + 1))) { // wall south, clear east
                return right;
            }
            else if (isObstacle(gamestate.vision.at(h + 1).at(w)) && isObstacle(gamestate.vision.at(h).at(w + 1)) 
            && isWalkable(gamestate.vision.at(h - 1).at(w))) { // wall south & east, clear north
                return up;
            }
            else if (isObstacle(gamestate.vision.at(h + 1).at(w + 1)) && isObstacle(gamestate.vision.at(h).at(w + 1)) 
            && isWalkable(gamestate.vision.at(h + 1).at(w))) { // diagonal case, wall southwest & east, clear south
                return down;
            }
            else {
                return nothing;
            }
        }

        if (direction == west) {
            if (isObstacle(gamestate.vision.at(h - 1).at(w)) && isWalkable(gamestate.vision.at(h).at(w - 1))) { // wall north, clear west
                return left;
            }
            else if (isObstacle(gamestate.vision.at(h - 1).at(w + 1)) && isWalkable(gamestate.vision.at(h - 1).at(w))) { // diagonal case, wall northeast, clear north
                return up;
            }
            else {
                return nothing;
            }
        }
    }
    // END STAGE 02

    // STAGE 03
    if (gamestate.stage == 3) {
        if (gamestate.score < 80) {
            // hug right wall until A is found
            if (direction == east) {
                if (isObstacle(gamestate.vision.at(h + 1).at(w)) && isWalkable(gamestate.vision.at(h).at(w + 1))) { // wall south, clear east
                    return right;
                }
                else if (isObstacle(gamestate.vision.at(h + 1).at(w - 1)) && isWalkable(gamestate.vision.at(h + 1).at(w))) { // wall southwest, clear south
                    return down;
                }
                else {
                    return nothing;
                }
            }

            if (direction == west) {
                if (isObstacle(gamestate.vision.at(h - 1).at(w)) && isWalkable(gamestate.vision.at(h).at(w - 1))) { // wall north, clear west
                    return left;
                }
                else if (isObstacle(gamestate.vision.at(h - 1).at(w + 1)) && isWalkable(gamestate.vision.at(h - 1).at(w))) { // wall northeast, clear north
                    return up;
                }
                else {
                    return nothing;
                }
            }

            if (direction == north) {
                if (isObstacle(gamestate.vision.at(h).at(w + 1)) && isWalkable(gamestate.vision.at(h - 1).at(w))) { // wall east, clear north
                    return up;
                }
                else if (isObstacle(gamestate.vision.at(h - 1).at(w)) && isObstacle(gamestate.vision.at(h).at(w + 1)) && isWalkable(gamestate.vision.at(h + 1).at(w))) { // check after getting A, shouldn't work but tests AB
                    return down;
                }
            }

            if (direction == south) {
                if (isObstacle(gamestate.vision.at(h).at(w - 1)) && isWalkable(gamestate.vision.at(h + 1).at(w))) { // wall west, clear south
                    return down;
                }
                else if (isObstacle(gamestate.vision.at(h - 1).at(w - 1)) && isWalkable(gamestate.vision.at(h).at(w - 1))) { // wall northwest, clear west
                    return left;
                }
                else if (isObstacle(gamestate.vision.at(h + 1).at(w)) && isWalkable(gamestate.vision.at(h).at(w + 1))) {
                    return right;
                }
                else {
                    return nothing;
                }
            }
            
        }
        // once A is found
        if (gamestate.score > 80 && gamestate.score < 100) {
            std::vector<int> target = closestGoal(gamestate, h, w);
            int target_h = target.at(0);
            int target_w = target.at(1);

            int distance_h{0};
            int distance_w{0};

            bool targetReachable = !(target_h == 0 && target_w == 0);

            if (targetReachable) {
                distance_h = h - target_h;
                distance_w = target_w - w;
            }

            if (isTarget(gamestate.vision.at(h - 1).at(w))) { // target directly above
                return up;
            }
            else if (isTarget(gamestate.vision.at(h + 1).at(w))) {  // target directly below
                return down;
            }
            else if (isTarget(gamestate.vision.at(h).at(w + 1))) {  // target directly right
                return right;
            }
            else if (isTarget(gamestate.vision.at(h).at(w - 1))) {  // target directly left
                return left;
            }

            // prioritize moving up / down
            if (distance_h != 0) {
                if (distance_h > 0) {
                    if (isWalkable(gamestate.vision.at(h - 1).at(w))) {
                        return up;
                    }
                    else if (isWalkable(gamestate.vision.at(h).at(w + 1))){
                        return right;
                    }
                    else if (isWalkable(gamestate.vision.at(h + 1).at(w))){
                        return down;
                    }
                } 
                else if (distance_h < 0) {
                    if (isWalkable(gamestate.vision.at(h + 1).at(w))) {
                        return down;
                    }
                    else if (isWalkable(gamestate.vision.at(h).at(w + 1))){
                        return right;
                    }
                }
            }
            // then move left / right
            if (distance_w != 0) {
                if (distance_w > 0 && isWalkable(gamestate.vision.at(h).at(w + 1))) {
                    return right;

                } 
                else if (distance_w < 0 && isWalkable(gamestate.vision.at(h).at(w - 1))) {
                    return left;
                }
                else if (distance_w > 0 && isObstacle(gamestate.vision.at(h - 1).at(w)) && isObstacle(gamestate.vision.at(h).at(w + 1))) {
                    return down;
                }
            }
            if (!targetReachable) {
                if (isObstacle(gamestate.vision.at(h - 1).at(w)) && isWalkable(gamestate.vision.at(h).at(w + 1))){ // wall north, clear east
                    return right;
                }
                else if (isObstacle(gamestate.vision.at(h - 1).at(w + 1)) && isWalkable(gamestate.vision.at(h).at(w + 1))) { // wall northeast, clear east
                    return right;
                }
                else if (isObstacle(gamestate.vision.at(h - 1).at(w - 1)) && isWalkable(gamestate.vision.at(h - 1).at(w))) { // wall northwest, clear above
                    return up;
                }
                else if (isWalkable(gamestate.vision.at(h + 1).at(w))) { // walk down
                    return down;
                } 
            }
        }
        
        if (gamestate.score > 100) {
            if (isWalkable(gamestate.vision.at(h).at(w + 1))) {
                return right;
            }
            else if (isObstacle(gamestate.vision.at(h).at(w + 1)) && isWalkable(gamestate.vision.at(h + 1).at(w))) {
                return down;
            }
            else {
                return nothing;
            }
        }
    }
    // END STAGE 03
    return 0;
}
