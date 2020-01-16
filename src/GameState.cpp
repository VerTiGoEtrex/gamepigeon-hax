#include "gamestate.hpp"

GameState::GameState(array<board_t, NUM_COLORS> board, bool playersTurn,
                     Color playerColor, Color computerColor, board_t playerBlob,
                     board_t computerBlob)
    : board(board), playersTurn(playersTurn), playerColor(playerColor),
      computerColor(computerColor), playerBlob(playerBlob),
      computerBlob(computerBlob) {}
GameState::~GameState() {}

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
  for (int i = 0; i < NUM_COLORS; ++i) {
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
  auto &otherBlob = playersTurn ? newState.computerBlob : newState.playerBlob;

  // update next state based on this "move"
  newState.playersTurn = !newState.playersTurn;
  currColor = c;
  currBlob = board::expandBlob(currBlob, newState.board[c] & ~otherBlob);
  return newState;
}

int GameState::getHeuristicValue() const {
  // TODO include relative "value" of colors. Cells closest to enemy are worth
  // more than further. Cells completely contained by our blob are the same as
  // "captured."
  auto balance = getPlayerSize() - getComputerSize();
  if (isTerminal() || getPlayerSize() + getComputerSize() >= BOARD_SIZE / 2) {
    if (balance > 0)
      balance += 10000;
    else if (balance < 0)
      balance -= 10000;
  }
  return balance;
}

bool GameState::isTerminal() const {
  return getPlayerSize() + getComputerSize() == BOARD_WIDTH * BOARD_HEIGHT;
}

int GameState::getPlayerSize() const { return __builtin_popcountl(playerBlob); }

int GameState::getComputerSize() const {
  return __builtin_popcountl(computerBlob);
}

bool GameState::isPlayersTurn() const { return playersTurn; }

ostream &operator<<(ostream &os, const GameState &state) {
  // Print the scores
  if (state.playersTurn)
    os << rang::style::bold;
  os << "A" << std::setw(2) << std::right << state.getPlayerSize()
     << rang::style::reset;
  if (!state.playersTurn)
    os << rang::style::bold;
  os << " B" << std::setw(2) << std::right << state.getComputerSize()
     << rang::style::reset;
  os << endl;
  os << "H " << state.getHeuristicValue() << endl;
  for (auto row = 0; row < BOARD_HEIGHT; ++row) {
    for (auto col = 0; col < BOARD_WIDTH; ++col) {
      board_t mask = 1ul << (row * BOARD_WIDTH + col);
      static_assert(sizeof(mask) == 8);
      Color color = Color::NONE;
      char c = '#';
      if (state.playerBlob & mask) {
        color = state.playerColor;
        c = 'X';
      } else if (state.computerBlob & mask) {
        color = state.computerColor;
        c = 'O';
      } else {
        color = board::getColor(state.board, mask);
        c = '.';
      }
      os << colorToTermBg(color) << c << rang::bg::reset;
    }
    os << endl;
  }
  return os;
}