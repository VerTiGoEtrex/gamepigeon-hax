#pragma once

#include "board.hpp"
#include "colors.hpp"
#include "rang.hpp"
#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <vector>

using std::array;
using std::endl;
using std::ostream;
using std::pair;
using std::vector;

class GameState {
private:
  array<board_t, NUM_COLORS> board;
  bool playersTurn;
  Color playerColor;
  Color computerColor;
  board_t playerBlob;
  board_t computerBlob;

public:
  GameState(array<board_t, NUM_COLORS> board, bool playersTurn,
            Color playerColor, Color computerColor, board_t playerBlob,
            board_t computerBlob);
  ~GameState();

  const vector<pair<Color, GameState>> genMoves() const;
  vector<Color> getPossibleColors() const;
  GameState applyMove(Color c) const;
  int getHeuristicValue() const;
  bool isTerminal() const;
  int getPlayerSize() const;
  int getComputerSize() const;
  bool isPlayersTurn() const;
  friend ostream &operator<<(ostream &os, const GameState &state);
};