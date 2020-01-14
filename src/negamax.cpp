#include "negamax.hpp"

// function negamax(node, depth, color) is
//     if depth = 0 or node is a terminal node then
//         return color × the heuristic value of node
//     value := −∞
//     for each child of node do
//         value := max(value, −negamax(child, depth − 1, −color))
//     return value
// (* Initial call for Player A's root node *)
// negamax(rootNode, depth, 1)
// (* Initial call for Player B's root node *)
// negamax(rootNode, depth, −1)

tuple<int, Color, GameState> negamax(GameState &state, int depth) {
  if (depth == 0 || state.isTerminal()) {
    // if player's turn, this node is worth the heuristic, else it's worth
    // inverse (to the computer)
    return {state.isPlayersTurn() ? state.getHeuristicValue()
                                  : -state.getHeuristicValue(),
            SENTINAL, state};
  }
  int bestValue = numeric_limits<int>::min();
  auto bestMove = SENTINAL;
  auto bestState = state;
  for (auto move : state.genMoves()) {
    auto [resultingValue, moveTaken, resultingState] =
        negamax(move.second, depth - 1);
    // Our best move is whatever minimizes the other player's score
    auto ourValueForMove = -resultingValue;
    if (bestValue < resultingValue) {
      bestValue = ourValueForMove;
      bestMove = move.first;
      // This is the best endgame state we found in the tree.
      bestState = resultingState;
    }
  }
  return {bestValue, bestMove, bestState};
}