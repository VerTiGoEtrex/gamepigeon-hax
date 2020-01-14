#include "GameState.hpp"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::tolower;

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

  cout << "And here are your next possible moves." << endl << endl;
  auto moves = state.genMoves();
  for (auto move : moves) {
    cout << "Color " << colorToChar(move.first) << endl;
    cout << move.second << endl;
  }
  return 0;
}