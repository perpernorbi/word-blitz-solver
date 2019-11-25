#include "grid.h"
#include "wordlist.h"

#include <unicode/ustream.h>

#include <algorithm>
#include <iostream>
#include <math.h>
#include <set>

int get_size(icu::UnicodeString s) {
  int size = sqrt(s.length());
  if (size * size != s.length())
    throw std::runtime_error("Invalid number of characters.");
  return size;
}

struct State {
  WordList::const_iterator w_begin;
  WordList::const_iterator w_end;
  Grid::Moves::const_iterator m_begin;
  Grid::Moves::const_iterator m_end;
  int position;
  icu::UnicodeString word;
};

class Solver {
private:
  const Grid &grid_;
  const WordList &word_list_;
  const icu::UnicodeString chars_;

public:
  Solver(const Grid &grid, const WordList &word_list,
         const icu::UnicodeString chars)
      : grid_(grid), word_list_(word_list_), chars_(chars) {}
};

int main(int argc, char *argv[]) {
  WordList word_list = CreateWordList("freedict_utf8");
  if (argc != 2)
    throw std::runtime_error(std::string{"usage: "} + argv[0] + " characters");
  const auto chars = icu::UnicodeString::fromUTF8(argv[1]);
  const auto size = get_size(chars);
  std::cout << "size: " << size << std::endl;

  Grid grid(size);
  std::vector<State> state;
  state.reserve(size * size + 1);
  state.push_back(State{word_list.begin(), word_list.end(), grid.All().cbegin(),
                        grid.All().cend(), 0,
                        icu::UnicodeString::fromUTF8("")});

  std::vector<bool> already_visited(size * size, false);
  std::set<icu::UnicodeString> already_found;
  while (state.size() > 0) {
    State &current_state = state.back();

    while ((current_state.m_begin != current_state.m_end) &&
           (already_visited[*current_state.m_begin + current_state.position]))
      ++current_state.m_begin;
    if (current_state.m_begin == current_state.m_end) {
      // No more possible moves from there
      already_visited[current_state.position] = false;
      state.pop_back();
      ++state.back().m_begin;
      continue;
    }

    State new_state;
    new_state.position = current_state.position + *current_state.m_begin;
    already_visited[new_state.position] = true;
    new_state.word = current_state.word;
    new_state.word.append(chars[new_state.position]);
    // std::cout << "word " << new_state.word << std::endl;
    new_state.w_begin = std::find_if(current_state.w_begin, current_state.w_end,
                                     [new_state](const icu::UnicodeString &s) {
                                       return s.startsWith(new_state.word);
                                     });
    new_state.w_end =
        std::find_if_not(new_state.w_begin, current_state.w_end,
                         [new_state](const icu::UnicodeString &s) {
                           return s.startsWith(new_state.word);
                         });
    // std::cout << "begin " << *new_state.w_begin << std::endl;
    // std::cout << "end " << *new_state.w_end << std::endl;
    new_state.m_begin = grid.PossibleMoves(new_state.position).cbegin();
    new_state.m_end = grid.PossibleMoves(new_state.position).cend();
    state.push_back(new_state);
    if (new_state.word == *new_state.w_begin) {
      if (std::find(already_found.cbegin(), already_found.cend(),
                    new_state.word) == already_found.cend()) {
        already_found.insert(new_state.word);
        std::cout << new_state.word << std::endl;
      }
    }
    if (new_state.w_begin == new_state.w_end) {
      already_visited[new_state.position] = false;
      state.pop_back();
      ++state.back().m_begin;
      continue;
    }
  }

  return 0;
}
