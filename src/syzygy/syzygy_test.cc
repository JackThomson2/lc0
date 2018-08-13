/*
  This file is part of Leela Chess Zero.
  Copyright (C) 2018 The LCZero Authors

  Leela Chess is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Leela Chess is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Leela Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include <iostream>
#include "src/syzygy/syzygy.h"

namespace lczero {

void TestValidExpectation(SyzygyTablebase* tablebase, const std::string& fen,
                          WDLScore expected, int expected_dtz) {
  ChessBoard board;
  PositionHistory history;
  board.SetFromFen(fen);
  history.Reset(board, 0, 1);
  ProbeState result;
  WDLScore score = tablebase->probe_wdl(history.Last(), &result);
  EXPECT_NE(result, FAIL);
  EXPECT_EQ(score, expected);
  int moves = tablebase->probe_dtz(history.Last(), &result);
  EXPECT_NE(result, FAIL);
  EXPECT_EQ(moves, expected_dtz);
}

TEST(Syzygy, Simple3PieceProbes) {
  SyzygyTablebase tablebase;
  // Try to find syzygy relative to current working directory.
  tablebase.init("syzygy");
  if (tablebase.max_cardinality() < 3) {
    // These probes require 3 piece tablebase.
    return;
  }
  // Longest 3 piece position.
  TestValidExpectation(&tablebase, "8/8/8/8/8/8/2Rk4/1K6 b - - 0 1", WDL_LOSS,
                       -31);
  // Invert color of above, no change.
  TestValidExpectation(&tablebase, "8/8/8/8/8/8/2rK4/1k6 w - - 0 1", WDL_LOSS,
                       -31);
  // Horizontal mirror.
  TestValidExpectation(&tablebase, "8/8/8/8/8/8/4kR2/6K1 b - - 0 1", WDL_LOSS,
                       -31);
  // Vertical mirror.
  TestValidExpectation(&tablebase, "6K1/4kR2/8/8/8/8/8/8 b - - 0 1", WDL_LOSS,
                       -31);
  // Horizontal mirror again.
  TestValidExpectation(&tablebase, "1K6/2Rk4/8/8/8/8/8/8 b - - 0 1", WDL_LOSS,
                       -31);

  // A draw by capture position, leaving KvK.
  TestValidExpectation(&tablebase, "5Qk1/8/8/8/8/8/8/4K3 b - - 0 1", WDL_DRAW,
                       0);

  // A position with a pawn which is to move and win from there.
  TestValidExpectation(&tablebase, "6k1/8/8/8/8/5p2/8/2K5 b - - 0 1", WDL_WIN,
                       1);

  // A position with a pawn that needs a king move first to win.
  TestValidExpectation(&tablebase, "8/8/8/8/8/k1p5/8/3K4 b - - 0 1", WDL_WIN,
                       2);

  // A position with a pawn that needs a few king moves before its a loss.
  TestValidExpectation(&tablebase, "8/2p5/8/8/8/5k2/8/2K5 w - - 0 1", WDL_LOSS,
                       -7);
}

TEST(Syzygy, Simple4PieceProbes) {
  SyzygyTablebase tablebase;
  // Try to find syzygy relative to current working directory.
  tablebase.init("syzygy");
  if (tablebase.max_cardinality() < 4) {
    // These probes require 4 piece tablebase.
    return;
  }

  // Longest 4 piece position.
  TestValidExpectation(&tablebase, "8/8/8/6B1/8/8/4k3/1K5N b - - 0 1", WDL_LOSS,
                       -64);

  // Some random checkmate position.
  TestValidExpectation(&tablebase, "8/8/8/8/8/2p5/3q2k1/4K3 w - - 0 1",
                       WDL_LOSS, -1);
}

TEST(Syzygy, Simple5PieceProbes) {
  SyzygyTablebase tablebase;
  // Try to find syzygy relative to current working directory.
  tablebase.init("syzygy");
  if (tablebase.max_cardinality() < 5) {
    // These probes require 5 piece tablebase.
    return;
  }

  // Longest 5 piece position.
  TestValidExpectation(&tablebase, "8/8/8/8/1p2P3/4P3/1k6/3K4 w - - 0 1",
                       WDL_CURSED_WIN, 101);

  // A blessed loss position.
  TestValidExpectation(&tablebase, "8/6B1/8/8/B7/8/K1pk4/8 b - - 0 1",
                       WDL_BLESSED_LOSS, -101);

  // Philidor draw position.
  TestValidExpectation(&tablebase, "8/8/8/8/4pk2/R7/7r/4K3 b - - 0 1", WDL_DRAW,
                       0);
  // Double mirrored and color swapped.
  TestValidExpectation(&tablebase, "3k4/R7/7r/2KP4/8/8/8/8 w - - 0 1", WDL_DRAW,
                       0);
}

}  // namespace lczero

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
