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

// returns a tuple representing the value of this node for whoever will be
// playing from it. If player A, the heuristic, if player B, the inverse of the
// heuristic.

// If depth > 0, then the value for this node is equal to the minimum of its
// value to the other player or in other words, the child with the maximum
// inverse "other player" value, or "-negamax"
vector<tuple<int, Color, GameState>> negamax(GameState &state, int depth,
                                             int maxDepth, int alpha, int beta,
                                             TimeOut &timeout) {
  if (!timeout.keepWorking())
    return {};
  auto thisHeuristic = state.getHeuristicValue();
  if (depth == 0 || state.isTerminal()) {
    // if player's turn, this node is worth the heuristic, else it's worth
    // inverse (to the computer)
    return {
        {(maxDepth * thisHeuristic * (state.isPlayersTurn() ? 1 : -1)) - depth,
         SENTINAL, state}};
  }
  int bestValue = numeric_limits<int>::min();
  auto bestMove = SENTINAL;
  auto bestState = state;
  vector<tuple<int, Color, GameState>> bestChain;
  auto moves = state.genMoves();

  if (state.isPlayersTurn()) {
    std::sort(moves.begin(), moves.end(), [](const auto lhs, const auto rhs) {
      return lhs.second.getHeuristicValue() > rhs.second.getHeuristicValue();
    });
  } else {
    std::sort(moves.begin(), moves.end(), [](const auto lhs, const auto rhs) {
      return lhs.second.getHeuristicValue() < rhs.second.getHeuristicValue();
    });
  }
  for (auto move : moves) {
    auto resultingChain =
        negamax(move.second, depth - 1, maxDepth, -beta, -alpha, timeout);
    if (resultingChain.empty()) {
      if (timeout.keepWorking())
        throw "wut?";
      return {};
    }
    auto [resultingValue, moveTaken, resultingState] = resultingChain.back();
    // Our best move is whatever minimizes the other player's score
    auto ourValueForMove = -resultingValue;
    if (bestValue < ourValueForMove) {
      bestValue = ourValueForMove;
      bestMove = move.first;
      bestState = move.second;
      bestChain = resultingChain;
    }
    alpha = max(alpha, bestValue);
    if (alpha >= beta)
      break;
  }
  bestChain.push_back({bestValue, bestMove, bestState});
  if (!timeout.keepWorking())
    return {};
  return bestChain;
}