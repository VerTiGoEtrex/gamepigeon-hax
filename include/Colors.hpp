#pragma once

#include "rang.hpp"
#include <iostream>

using std::ostream;

enum Color {
  RED,
  GREEN,
  YELLOW,
  BLUE,
  PURPLE,
  BLACK,
  NONE = -1,
};
const int NUM_COLORS = Color::BLACK + 1;
static_assert(NUM_COLORS == 6);

Color charToColor(char c);
rang::bg colorToTermBg(Color c);
ostream &operator<<(ostream &os, const Color &c);