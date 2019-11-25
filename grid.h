#pragma once
#include <vector>

class Grid {
public:
  using Moves = std::vector<int>;
  Grid(int size);
  const Moves &PossibleMoves(int position);
  const Moves &All();

private:
  int size_;
  const std::vector<Moves> possible_moves_;
  const Moves all_;
};
