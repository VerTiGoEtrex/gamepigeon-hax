#include "Colors.hpp"

ostream &operator<<(ostream &os, const Color &c) {
  os << colorToTermBg(c) << '.' << rang::bg::reset;
  return os;
}