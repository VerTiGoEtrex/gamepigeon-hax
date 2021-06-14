#include "colors.hpp"

Color charToColor(char c) {
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

rang::bg colorToTermBg(Color c) {
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
    throw "invalid color!";
  }
}

ostream &operator<<(ostream &os, const Color &c) {
  os << colorToTermBg(c);
  return os;
}