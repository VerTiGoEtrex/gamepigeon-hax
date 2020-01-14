#pragma once

#include "Colors.hpp"
#include "Constants.hpp"
#include <array>
#include <iostream>
#include <vector>

using std::array;
using std::endl;
using std::ostream;
using std::pair;
using std::vector;

typedef array<bool, BOARD_SIZE> board_b;
typedef array<Color, BOARD_SIZE> board_c;

class GameState {
private:
  board_c board;
  bool playersTurn;
  Color playerColor;
  Color computerColor;
  board_b playerBlob;
  board_b computerBlob;

public:
  GameState(board_c board, bool playersTurn, Color playerColor,
            Color computerColor, board_b playerBlob, board_b computerBlob);
  ~GameState();

  const vector<pair<Color, GameState>> genMoves() const;
  const int getHeuristicValue() const;
  const bool isTerminal() const;
  const int getPlayerSize() const;
  const int getComputerSize() const;
  const bool isPlayersTurn() const;

  static int resolveCoord(int row, int col) { return row * BOARD_WIDTH + col; }
  static pair<int, int> unresolveCoord(int coord) {
    return {coord / BOARD_WIDTH, coord % BOARD_WIDTH};
  }

  friend ostream &operator<<(ostream &os, const GameState &state);
};