#pragma once

#include "GameState.hpp"
#include <numeric>

using std::max;
using std::numeric_limits;
using std::tuple;

tuple<int, Color, GameState> negamax(GameState &state, int depth);