#include "GameState.hpp"
#include "negamax.hpp"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::tolower;
using std::tuple;

// Functor to compare by the Mth element
template <int M, template <typename> class F = std::less> struct TupleCompare {
  template <typename T> bool operator()(T const &t1, T const &t2) {
    return F<typename std::tuple_element<M, T>::type>()(std::get<M>(t1),
                                                        std::get<M>(t2));
  }
};

void dfs(int row, int col, board_c &board, board_b &blob, Color c) {
  if (row < 0 || row >= BOARD_HEIGHT || col < 0 || col >= BOARD_WIDTH)
    return;
  auto resolvedCoord = GameState::resolveCoord(row, col);
  if (board[resolvedCoord] != c)
    return;
  if (blob[resolvedCoord])
    return;
  blob[resolvedCoord] = true;
  dfs(row + 1, col, board, blob, c);
  dfs(row - 1, col, board, blob, c);
  dfs(row, col + 1, board, blob, c);
  dfs(row, col - 1, board, blob, c);
}

int main() {
  cout << "*********************" << endl;
  cout << "** GAME PIGEON HAX **" << endl;
  cout << "*********************" << endl;
  cout << "please enter the board row by row (Red Green Yellow Blue Purple "
          "bLack), top to bottom:"
       << endl;

  board_c colors;
  for (int row = 0; row < BOARD_HEIGHT; ++row) {
    string rowStr;
    cin >> rowStr;
    if (rowStr.size() != BOARD_WIDTH) {
      throw "must type exactly " + to_string(BOARD_WIDTH) + " characters";
    }
    for (int col = 0; col < BOARD_WIDTH; ++col) {
      colors[GameState::resolveCoord(row, col)] =
          charToColor(toupper(rowStr[col]));
    }
  }

  char inChar;
  cout << "is it your turn? ";
  cin >> inChar;
  bool playersTurn = tolower(inChar) == 'y';

  // assume player is in bottom left corner and opponent is in top right corner
  Color playersColor = colors[GameState::resolveCoord(BOARD_HEIGHT - 1, 0)];
  Color computerColor = colors[GameState::resolveCoord(0, BOARD_WIDTH - 1)];

  board_b playerBlob, computerBlob;
  playerBlob.fill(false);
  computerBlob.fill(false);
  dfs(BOARD_HEIGHT - 1, 0, colors, playerBlob, playersColor);
  dfs(0, BOARD_WIDTH - 1, colors, computerBlob, computerColor);

  GameState state(colors, playersTurn, playersColor, computerColor, playerBlob,
                  computerBlob);

  cout << "Thanks for entering the board configuration. Here's what we think "
          "you entered."
       << endl;
  cout << state << endl;

  // cout << "Please enter a maximum depth to search: ";
  // int maxDepth;
  // cin >> maxDepth;

  for (int depth = 1;; ++depth) {
    // Begin negamax lolercopters
    cout << "Searching depth " << depth << "..." << endl;
    auto [score, move, newState] = negamax(state, depth);
    cout << "Color " << move << " yields score of " << score
         << " and a board of " << state << endl;
  }
  return 0;
}