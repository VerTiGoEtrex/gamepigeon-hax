#pragma once

#include "GameState.hpp"
#include <numeric>

using std::max;
using std::numeric_limits;
using std::tuple;

vector<tuple<int, Color, GameState>> negamax(GameState &state, int depth,
                                             int maxDepth);