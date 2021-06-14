#include "gamestate.hpp"
#include "negamax.hpp"
#include "timeout.hpp"
#include <chrono>
#include <future>
#include <iostream>
#include <random>

using std::cerr;
using std::cin;
using std::cout;
using std::default_random_engine;
using std::endl;
using std::string;
using std::to_string;
using std::tolower;
using std::tuple;
using std::uniform_int_distribution;

Color computeBestMove(GameState state, int maxDepth, TimeOut timeout) {
  cout << "------------------------\n";
  cout << rang::style::bold << "Searching depth " << maxDepth << "..."
       << rang::style::reset << endl;
  cout << "-- current board --" << endl;
  cout << state << endl;

  auto f = std::async(std::launch::async, negamax, std::ref(state), maxDepth,
                      maxDepth, numeric_limits<int>::min() + 1,
                      numeric_limits<int>::max(), std::ref(timeout));
  // Best chain of moves for current player in reverse order
  auto bestChain = f.get();
  if (bestChain.empty()) {
    cout << "no moves. Probably timed out." << endl;
    return Color::NONE;
  }
  for (auto it = bestChain.rbegin(); it != bestChain.rend() - 1; ++it) {
    auto [thisScore, thisMove, thisNewState] = *it;
    cout << "-- after move " << thisMove << distance(bestChain.rbegin(), it)
         << " by player " << (thisNewState.isPlayersTurn() ? "B" : "A")
         << rang::bg::reset << " yields negamax score of " << thisScore
         << " --\n"
         << thisNewState << endl;
  }
  auto [score, move, newState] = bestChain.back();

  cout << "Summary - Color " << move << '.' << rang::bg::reset
       << " yields best score of " << score << "\n\n\n\n";
  return move;
}

void selfCheck() {
  cout << "Selfcheck (VALID MASK):" << endl;
  cout << print_mask(board::VALID_MASK(), true) << endl;
  cout << print_mask(board::left(board::VALID_MASK()), true) << endl;
  cout << print_mask(board::right(board::VALID_MASK()), true) << endl;
  cout << print_mask(board::up(board::VALID_MASK()), true) << endl;
  cout << print_mask(board::down(board::VALID_MASK()), true) << endl;

  cout << "Selfcheck (CROSS):" << endl;
  cout << print_mask(board::CROSS_MASK(), true) << endl;
  cout << print_mask(board::left(board::CROSS_MASK()), true) << endl;
  cout << print_mask(board::right(board::CROSS_MASK()), true) << endl;
  cout << print_mask(board::up(board::CROSS_MASK()), true) << endl;
  cout << print_mask(board::down(board::CROSS_MASK()), true) << endl;

  cout << "Selfcheck (STARTS):" << endl;
  cout << print_mask(PLAYER_START, true) << endl;
  cout << print_mask(COMPUTER_START, true) << endl;

  cout << endl;
}

int main() {
  try {
    char inChar;
    array<board_t, NUM_COLORS> colors;
    cout << "*********************" << endl;
    cout << "** GAME PIGEON HAX **" << endl;
    cout << "*********************" << endl;

    // selfCheck();

    cout << "init with random game? ";
    cin >> inChar;
    if (tolower(inChar) == 'y') {
      cout << "input a random seed: ";
      int randomSeed;
      cin >> randomSeed;
      default_random_engine rand(randomSeed);
      colors = board::makeRandomBoard(rand);
    } else {
      colors.fill(0);
      cout << "please enter the board row by row (Red Green Yellow Blue Purple "
              "bLack), top to bottom:"
           << endl;

      for (int row = 0; row < BOARD_HEIGHT; ++row) {
        string rowStr;
        cin >> rowStr;
        if (rowStr.size() != BOARD_WIDTH) {
          throw "must type exactly " + to_string(BOARD_WIDTH) + " characters";
        }
        for (int col = 0; col < BOARD_WIDTH; ++col) {
          int pos = row * BOARD_WIDTH + col;
          colors[charToColor(toupper(rowStr[col]))] |= (1ul << pos);
        }
      }
    }
    if (!board::isValidBoard(colors)) {
      cerr << "Invalid board!\n" << endl;
      for (int i = 0; i < NUM_COLORS; ++i)
        cerr << colorToTermBg(static_cast<Color>(i)) << print_mask(colors[i])
             << rang::bg::reset << endl;
      throw "Invalid board!";
    }

    cout << "is it your turn? ";
    cin >> inChar;
    bool playersTurn = tolower(inChar) == 'y';

    // assume player is in bottom left corner and opponent is in top right
    // corner
    Color playersColor = board::getColor(colors, PLAYER_START);
    Color computerColor = board::getColor(colors, COMPUTER_START);

    board_t playerBlob = board::expandBlob(PLAYER_START, colors[playersColor]);
    board_t computerBlob =
        board::expandBlob(COMPUTER_START, colors[computerColor]);

    GameState state(colors, playersTurn, playersColor, computerColor,
                    playerBlob, computerBlob);

    cout << "Thanks for entering the board configuration. Here's what we think "
            "you entered."
         << endl;
    cout << state << endl;

    cout << "Want to play against the computer? ";
    cin >> inChar;
    bool playVsComputer = tolower(inChar) == 'y';

    cout << "Sanity check: next possible moves from provided state" << endl;
    for (auto move : state.genMoves()) {
      cout << "Color: " << move.first << '.' << rang::bg::reset
           << "board: " << endl
           << move.second << endl;
    }

    while (!state.isTerminal()) {
      if (!playVsComputer || !state.isPlayersTurn()) {
        Color bestMove = Color::NONE;
        for (int depth = 1;; ++depth) {
          auto thisMove =
              computeBestMove(state, depth, TimeOut(std::chrono::seconds(3)));
          if (thisMove == Color::NONE)
            break;
          bestMove = thisMove;
        }
        if (bestMove == Color::NONE)
          throw "could not calculate best move in time!";

        if (playVsComputer) {
          cout << "-- previous board --" << endl << state << endl;
          state = state.applyMove(bestMove);
          continue;
        }
      }

      cout << "-- current board --" << endl << state << endl;
      Color colorChosen;
      auto possibleColors = state.getPossibleColors();
      while (true) {
        cout << "Enter the move actually played: ";
        cin >> inChar;
        try {
          colorChosen = charToColor(toupper(inChar));
        } catch (...) {
          cerr << "Invalid color? Try one of (RGBYPL)" << endl;
          continue;
        }
        if (std::find(possibleColors.begin(), possibleColors.end(),
                      colorChosen) != possibleColors.end())
          break;
        cerr << "Cannot select that color. Are you or your opponent "
                "already that color?"
             << endl;
      }
      state = state.applyMove(colorChosen);
    }

    cout << state << endl;

    if (state.getHeuristicValue() == 0) {
      cout << "Draw! Thanks for playing!";
    } else if (state.getHeuristicValue() > 0) {
      cout << "Player A wins!";
    } else {
      cout << "Player B wins!";
    }
    return 0;
  } catch (string s) {
    cerr << s << endl;
    return 1;
  } catch (const char *s) {
    cerr << s << endl;
    return 1;
  }
}