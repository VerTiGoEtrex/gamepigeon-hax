#include "GameState.hpp"
#include "Colors.hpp"
#include "Constants.hpp"
#include "rang.hpp"
#include <algorithm>

GameState::GameState(board_c board, bool playersTurn, Color playerColor,
                     Color computerColor, board_b playerBlob,
                     board_b computerBlob)
    : board(board), playersTurn(playersTurn), playerColor(playerColor),
      computerColor(computerColor), playerBlob(playerBlob),
      computerBlob(computerBlob) {}
GameState::~GameState() {}

void dfs(int row, int col, board_b &blob, Color newColor, board_c &board) {
  if (row < 0 || row >= BOARD_HEIGHT || col < 0 || col >= BOARD_WIDTH)
    return;
  // This is a valid board coordinate, now let's check if it's the new color
  auto resolvedCoord = GameState::resolveCoord(row, col);
  if (board[resolvedCoord] == newColor != blob[resolvedCoord]) {
    // coordinate is either the new color and not part of the blob yet, or it's
    // part of the blob, but not painted yet. else, coordinate is both new color
    // and blobbed (already painted) or coordinate is not the new color, and is
    // also not blobbed (not to be touched)
    blob[resolvedCoord] = true;
    board[resolvedCoord] = newColor;
    dfs(row + 1, col, blob, newColor, board);
    dfs(row - 1, col, blob, newColor, board);
    dfs(row, col + 1, blob, newColor, board);
    dfs(row, col - 1, blob, newColor, board);
  }
}

const vector<pair<Color, GameState>> GameState::genMoves() const {
  // These are all of the possible resulting game states from this point.
  vector<pair<Color, GameState>> ret;

  // Determine possible colors for either player to change to
  vector<Color> possibleColors = getPossibleColors();

  // Create a new gamestate based on taking these moves
  for (Color c : possibleColors) {
    ret.push_back({c, applyMove(c)});
  }
  return ret;
}

vector<Color> GameState::getPossibleColors() const {
  vector<Color> possibleColors;
  for (int i = 0; i < Color::SENTINAL; ++i) {
    Color c = static_cast<Color>(i);
    if (c != playerColor && c != computerColor) {
      possibleColors.push_back(c);
    }
  }
  return possibleColors;
}

GameState GameState::applyMove(Color c) const {
  // get a copy of the current gamestate
  GameState newState = *this;
  auto &currColor = playersTurn ? newState.playerColor : newState.computerColor;
  auto &currBlob = playersTurn ? newState.playerBlob : newState.computerBlob;

  // update next state based on this "move"
  newState.playersTurn = !newState.playersTurn;
  currColor = c;

  for (int i = 0; i < BOARD_SIZE; ++i) {
    // find something already part of the blob
    if (currBlob[i]) {
      // dfs from that point on the blob and quit
      auto [row, col] = unresolveCoord(i);
      dfs(row, col, currBlob, c, newState.board);
      break;
    }
  }
  return newState;
}

const int GameState::getHeuristicValue() const {
  // TODO include relative "value" of colors. Cells closest to enemy are worth
  // more than further. Cells compeltely contained by our blob are the same as
  // "captured."
  auto balance = getPlayerSize() - getComputerSize();
  if (isTerminal()) {
    if (balance > 0)
      balance += 10000;
    else if (balance < 0)
      balance -= 10000;
  }
  return balance;
}

const bool GameState::isTerminal() const {
  return getPlayerSize() + getComputerSize() == BOARD_WIDTH * BOARD_HEIGHT;
}

const int GameState::getPlayerSize() const {
  int size = 0;
  for (int i = 0; i < BOARD_SIZE; ++i) {
    if (playerBlob[i])
      ++size;
  }
  return size;
}

const int GameState::getComputerSize() const {
  int size = 0;
  for (int i = 0; i < BOARD_SIZE; ++i) {
    if (computerBlob[i])
      ++size;
  }
  return size;
}

const bool GameState::isPlayersTurn() const { return playersTurn; }

ostream &operator<<(ostream &os, const GameState &state) {
  os << "Player to move? " << (state.playersTurn ? 'A' : 'B') << endl;
  os << "Current player value " << state.getHeuristicValue() << "("
     << state.getPlayerSize() << "-" << state.getComputerSize() << ")" << endl;
  for (auto row = 0; row < BOARD_HEIGHT; ++row) {
    for (auto col = 0; col < BOARD_WIDTH; ++col) {
      auto resolved = GameState::resolveCoord(row, col);
      os << state.board[resolved]
         << (state.playerBlob[resolved]
                 ? 'X'
                 : state.computerBlob[resolved] ? 'O' : '.')
         << rang::bg::reset;
    }
    os << endl;
  }
  return os;
}