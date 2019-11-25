#include "grid.h"
#include <cassert>
#include <numeric>

namespace {
// -size-1 -size -size+1
//      -1     0      +1
//  size-1  size  size+1

std::vector<Grid::Moves> generate_possible_moves(int size) {
  std::vector<Grid::Moves> ret;
  ret.reserve(size * size);
  for (auto i = 0; i < size; ++i)
    for (auto j = 0; j < size; ++j) {

      const bool top = i == 0;
      const bool bottom = i == size - 1;
      const bool left = j == 0;
      const bool right = j == size - 1;
      if (top) {
        if (left)
          ret.emplace_back(Grid::Moves{1, size, size + 1});
        else if (right)
          ret.emplace_back(Grid::Moves{-1, size - 1, size});
        else
          ret.emplace_back(Grid::Moves{-1, 1, size - 1, size, size + 1});
      } else if (bottom) {
        if (left)
          ret.emplace_back(Grid::Moves{-size, -size + 1, 1});
        else if (right)
          ret.emplace_back(Grid::Moves{-size - 1, -size, -1});
        else
          ret.emplace_back(Grid::Moves{-size - 1, -size, -size + 1, -1, +1});
      } else {
        if (left)
          ret.emplace_back(Grid::Moves{-size, -size + 1, +1, size, size + 1});
        else if (right)
          ret.emplace_back(Grid::Moves{-size - 1, -size, -1, size - 1, size});
        else
          ret.emplace_back(Grid::Moves{-size - 1, -size, -size + 1, -1, 1,
                                       size - 1, size, size + 1});
      }
    }
  return ret;
}

Grid::Moves generate_all(int size) {
  Grid::Moves ret(size * size);
  std::iota(ret.begin(), ret.end(), 0);
  return ret;
}
} // namespace

Grid::Grid(int size)
    : size_(size), possible_moves_(generate_possible_moves(size)),
      all_(generate_all(size)) {
  assert(0 < size);
}

const Grid::Moves &Grid::PossibleMoves(int position) {
  assert(position < size_ * size_);
  return possible_moves_[position];
}

const Grid::Moves &Grid::All() { return all_; }
