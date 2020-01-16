#pragma once

#include "colors.hpp"
#include <array>
#include <iostream>
#include <random>

using std::array;
using std::default_random_engine;
using std::endl;
using std::uniform_int_distribution;

// Bitboard representation. Board is stored in row-major order.
// lsb is col 0 row 0 (top left), lsb+1 is col 1 row 0 (one to right of top
// left)
typedef unsigned long board_t;

const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 7;
const int BOARD_SIZE = BOARD_WIDTH * BOARD_HEIGHT;
const long PLAYER_START = 1l << (BOARD_WIDTH * (BOARD_HEIGHT - 1));
const long COMPUTER_START = 1l << (BOARD_WIDTH - 1);

class board {
  static constexpr board_t LEFT_MASK_BLANKS();
  static constexpr board_t RIGHT_MASK_BLANKS();

public:
  static array<board_t, NUM_COLORS> makeRandomBoard(default_random_engine &);
  static bool isValidBoard(array<board_t, NUM_COLORS> board);
  static board_t left(board_t);
  static board_t right(board_t);
  static board_t up(board_t);
  static board_t down(board_t);
  static board_t expandBlob(board_t blob, board_t mask);
  static Color getColor(array<board_t, NUM_COLORS> colors, board_t mask);

  static constexpr board_t VALID_MASK() {
    return (~0ul) >> (sizeof(board_t) * 8 - BOARD_SIZE);
  }
  static constexpr board_t CROSS_MASK() {
    return 0b00011000'00011000'00011000'11111111'00011000'00011000'00011000ul;
  }
  // TODO fix this somehow if possible
  // static_assert(__builtin_popcountl(VALID_MASK()) == BOARD_SIZE);
};

struct print_mask {
  board_t const &ref;
  bool full;

  print_mask(board_t const &in, bool full = false) : ref(in), full(full) {}
};

ostream &operator<<(ostream &os, print_mask o);