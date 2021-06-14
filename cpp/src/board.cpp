#include "board.hpp"

constexpr board_t board::LEFT_MASK_BLANKS() {
  board_t board = (~0ul);
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    board &= ~(1ul << ((i * BOARD_WIDTH) + (BOARD_WIDTH - 1)));
  }
  return (const board_t)board;
}

constexpr board_t board::RIGHT_MASK_BLANKS() {
  board_t board = (~0ul);
  for (int i = 0; i < BOARD_HEIGHT; ++i) {
    board &= ~(1ul << (i * BOARD_WIDTH));
  }
  return (const board_t)board;
}

array<board_t, NUM_COLORS>
board::makeRandomBoard(default_random_engine &engine) {
  array<board_t, NUM_COLORS> ret;
  ret.fill(0);
  uniform_int_distribution colorDist(0, NUM_COLORS - 1);
  // add from top left row-major
  for (int i = 0; i < BOARD_SIZE; ++i) {
    Color randomColor = Color::NONE;
    auto bitPos = 1ul << i;
    bool eqAdjLeft, eqAdjTop, isPlayerStartAndEqComputer;
    do {
      randomColor = static_cast<Color>(colorDist(engine));
      eqAdjLeft = (ret[randomColor] & left(bitPos)) != 0;
      eqAdjTop = (ret[randomColor] & up(bitPos)) != 0;
      isPlayerStartAndEqComputer = (bitPos == PLAYER_START) &&
                                   ((ret[randomColor] & COMPUTER_START) != 0);
    } while (eqAdjLeft || eqAdjTop || isPlayerStartAndEqComputer);
    std::cout << colorToTermBg(randomColor) << '.' << rang::bg::reset;
    if (i % 8 == 7)
      std::cout << std::endl;
    ret[randomColor] |= bitPos;
  }
  return ret;
}

// Checks every bit has exactly one color
bool board::isValidBoard(array<board_t, NUM_COLORS> board) {
  board_t allOn = board::VALID_MASK();
  for (int i = 0; i < NUM_COLORS; ++i) {
    // number of squares left before flipping
    auto onBits = __builtin_popcountl(allOn);
    allOn ^= board[i];
    if (__builtin_popcountl(allOn) != onBits - __builtin_popcountl(board[i])) {
      // we expected that |board[i]| bits would be turned off, but less were,
      // meaning something must have been turned back on (more than one color
      // for bit)
      return false;
    }
  }
  // every bit must have exactly one color
  return allOn == 0;
}

board_t board::left(board_t b) {
  return (b >> 1) & LEFT_MASK_BLANKS() & VALID_MASK();
}
board_t board::right(board_t b) {
  return (b << 1) & RIGHT_MASK_BLANKS() & VALID_MASK();
}
board_t board::up(board_t b) { return (b >> BOARD_WIDTH) & VALID_MASK(); }
board_t board::down(board_t b) { return (b << BOARD_WIDTH) & VALID_MASK(); }

board_t board::expandBlob(board_t blob, board_t mask) {
  board_t newBlob = blob;
  do {
    blob = newBlob;
    auto floodMask = board::left(blob) | board::right(blob) | board::up(blob) |
                     board::down(blob);
    newBlob |= floodMask & mask;
    newBlob &= board::VALID_MASK();
  } while (newBlob != blob);
  return newBlob;
}

Color board::getColor(array<board_t, NUM_COLORS> colors, board_t mask) {
  Color ret = NONE;
  for (int i = 0; i < NUM_COLORS; ++i) {
    if ((colors[i] & mask) != 0) {
      if (ret != NONE)
        throw "mask has multiple colors " + std::to_string(ret) + " " +
            std::to_string(static_cast<Color>(i));
      ret = static_cast<Color>(i);
    }
  }
  if (ret == NONE)
    throw "mask has no colors";
  return ret;
}

ostream &operator<<(ostream &os, print_mask o) {
  if (!o.full) {
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
      for (int col = 0; col < BOARD_WIDTH; ++col) {
        long mask = 1ul << (row * BOARD_WIDTH + col);
        os << (o.ref & mask ? 'X' : '.');
      }
      os << endl;
    }
  } else {
    for (unsigned long i = 0; i < sizeof(o.ref) * 8; ++i) {
      os << (o.ref & (1ul << i) ? 'X' : '.');
      if (i % 8 == 7)
        os << endl;
    }
  }
  return os;
}