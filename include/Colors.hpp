#pragma once

#include "rang.hpp"
#include <iostream>

using std::ostream;

enum Color { RED, GREEN, YELLOW, BLUE, PURPLE, BLACK, SENTINAL };

static char colorToChar(Color c) {
  switch (c) {
  case RED:
    return 'R';
  case GREEN:
    return 'G';
  case YELLOW:
    return 'Y';
  case BLUE:
    return 'B';
  case PURPLE:
    return 'P';
  case BLACK:
    return 'L';
  default:
    throw "invalid color";
  }
}

static Color charToColor(char c) {
  switch (c) {
  case 'R':
    return RED;
  case 'G':
    return GREEN;
  case 'Y':
    return YELLOW;
  case 'B':
    return BLUE;
  case 'P':
    return PURPLE;
  case 'L':
    return BLACK;
  default:
    throw "invalid color";
  }
}

static rang::bg colorToTermBg(Color c) {
  switch (c) {
  case RED:
    return rang::bg::red;
  case GREEN:
    return rang::bg::green;
  case YELLOW:
    return rang::bg::yellow;
  case BLUE:
    return rang::bg::blue;
  case PURPLE:
    return rang::bg::magenta;
  case BLACK:
    return rang::bg::black;
  default:
    throw "invalid color";
  }
}

ostream &operator<<(ostream &os, const Color &c);