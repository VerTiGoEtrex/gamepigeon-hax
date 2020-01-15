#include "GameState.hpp"
#include "negamax.hpp"
#include "timeout.hpp"
#include <chrono>
#include <future>
#include <iostream>
#include <random>

using std::cin;
using std::cout;
using std::default_random_engine;
using std::endl;
using std::string;
using std::to_string;
using std::tolower;
using std::tuple;
using std::uniform_int_distribution;

void dfs(int row, int col, board_c &board, board_b &blob, Color c) {
  if (row < 0 || row >= BOARD_HEIGHT || col < 0 || col >= BOARD_WIDTH)
    return;
  auto resolvedCoord = GameState::resolveCoord(row, col);
  if (board[resolvedCoord] != c)
    return;
  if (blob[resolvedCoord])
    return;
  blob[resolvedCoord] = true;
  dfs(row + 1, col, board, blob, c);
  dfs(row - 1, col, board, blob, c);
  dfs(row, col + 1, board, blob, c);
  dfs(row, col - 1, board, blob, c);
}

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
    return SENTINAL;
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

int main() {
  try {
    uniform_int_distribution colorDist(0, Color::SENTINAL - 1);
    char inChar;
    board_c colors;
    cout << "*********************" << endl;
    cout << "** GAME PIGEON HAX **" << endl;
    cout << "*********************" << endl;
    cout << "init with random game? ";
    cin >> inChar;
    if (tolower(inChar) == 'y') {
      cout << "input a random seed: ";
      int randomSeed;
      cin >> randomSeed;
      default_random_engine rand(randomSeed);

      for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
          bool eqAdjLeft, eqAdjTop, isPlayerStartAndEqComputer;
          Color randomColor;
          do {
            randomColor = static_cast<Color>(colorDist(rand));
            eqAdjLeft = col > 0
                            ? randomColor ==
                                  colors[GameState::resolveCoord(row, col - 1)]
                            : false;
            eqAdjTop = row > 0
                           ? randomColor ==
                                 colors[GameState::resolveCoord(row - 1, col)]
                           : false;
            isPlayerStartAndEqComputer =
                (row == BOARD_HEIGHT - 1 && col == 0)
                    ? randomColor ==
                          colors[GameState::resolveCoord(0, BOARD_WIDTH - 1)]
                    : false;
          } while (eqAdjLeft || eqAdjTop || isPlayerStartAndEqComputer);
          colors[GameState::resolveCoord(row, col)] = randomColor;
        }
      }
    } else {
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
          colors[GameState::resolveCoord(row, col)] =
              charToColor(toupper(rowStr[col]));
        }
      }
    }

    cout << "is it your turn? ";
    cin >> inChar;
    bool playersTurn = tolower(inChar) == 'y';

    // assume player is in bottom left corner and opponent is in top right
    // corner
    Color playersColor = colors[GameState::resolveCoord(BOARD_HEIGHT - 1, 0)];
    Color computerColor = colors[GameState::resolveCoord(0, BOARD_WIDTH - 1)];

    board_b playerBlob, computerBlob;
    playerBlob.fill(false);
    computerBlob.fill(false);
    dfs(BOARD_HEIGHT - 1, 0, colors, playerBlob, playersColor);
    dfs(0, BOARD_WIDTH - 1, colors, computerBlob, computerColor);

    GameState state(colors, playersTurn, playersColor, computerColor,
                    playerBlob, computerBlob);

    cout << "Thanks for entering the board configuration. Here's what we think "
            "you entered."
         << endl;
    cout << state << endl;

    cout << "Want to play against the computer? ";
    cin >> inChar;
    bool playVsComputer = tolower(inChar) == 'y';

    // cout << "Sanity check: next possible moves from provided state" << endl;
    // for (auto move : state.genMoves()) {
    //   cout << "Color: " << move.first << '.' << rang::bg::reset
    //        << "board: " << endl
    //        << move.second << endl;
    // }

    const int MAX_DEPTH = 12;

    while (!state.isTerminal()) {
      if (!playVsComputer || !state.isPlayersTurn()) {
        Color bestMove = SENTINAL;
        for (int depth = 1;; ++depth) {
          auto thisMove =
              computeBestMove(state, depth, TimeOut(std::chrono::seconds(3)));
          if (thisMove == SENTINAL)
            break;
          bestMove = thisMove;
        }
        if (bestMove == SENTINAL)
          throw "could not calculate best move in time!";

        // auto bestMove = computeBestMove(state, MAX_DEPTH);
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
          std::cerr << "Invalid color? Try one of (RGBYPL)" << endl;
          continue;
        }
        if (std::find(possibleColors.begin(), possibleColors.end(),
                      colorChosen) != possibleColors.end())
          break;
        std::cerr << "Cannot select that color. Are you or your opponent "
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
    std::cerr << s << endl;
    return 1;
  } catch (const char *s) {
    std::cerr << s << endl;
    return 1;
  }
}