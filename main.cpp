#include "grid.h"
#include "wordlist.h"

#include <unicode/ustream.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <math.h>
#include <vector>

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

std::pair<icu::UnicodeString, std::vector<bool>>
GetCharsAndStars(std::string input) {
  icu::UnicodeString utf(icu::UnicodeString::fromUTF8(input));
  icu::UnicodeString chars;
  std::vector<bool> stars(utf.length(), false);
  int i = 0;
  for (int j = 0; j < utf.length(); ++j)
    if (utf[j] == '.')
      stars[i] = true;
    else {
      chars += utf[j];
      ++i;
    }
  return std::make_pair(chars, stars);
}

bool IsStarred(std::vector<bool> already_visited, std::vector<bool> stars) {
  for (auto i = 0; i < already_visited.size(); ++i)
    if (already_visited[i] && stars[i])
      return true;
  return false;
}

enum class Printed { yes, no };

int main(int argc, char *argv[]) {
  WordList word_list = CreateWordList("freedict_utf8");
  if (argc != 2)
    throw std::runtime_error(std::string{"usage: "} + argv[0] + " characters");
  const auto chars_and_stars = GetCharsAndStars(argv[1]);
  const auto &chars = chars_and_stars.first;
  const auto &stars = chars_and_stars.second;
  const auto size = get_size(chars);

  std::cout << "size: " << size << std::endl;

  Grid grid(size);
  std::vector<State> state;
  state.reserve(size * size + 1);
  state.push_back(State{word_list.begin(), word_list.end(), grid.All().cbegin(),
                        grid.All().cend(), 0,
                        icu::UnicodeString::fromUTF8("")});

  std::vector<bool> already_visited(size * size, false);
  std::map<icu::UnicodeString, Printed> already_found;
  while (state.size() > 0) {
    State &current_state = state.back();

    while ((current_state.m_begin != current_state.m_end) &&
           (already_visited[*current_state.m_begin + current_state.position]))
      ++current_state.m_begin;
    if (current_state.m_begin == current_state.m_end) {
      // No more possible moves from there
      already_visited[current_state.position] = false;
      state.pop_back();
      if (state.size() > 0)
        ++state.back().m_begin;
      continue;
    }

    State new_state;
    new_state.position = current_state.position + *current_state.m_begin;
    already_visited[new_state.position] = true;
    new_state.word = current_state.word;
    new_state.word.append(chars[new_state.position]);
    new_state.w_begin = std::find_if(current_state.w_begin, current_state.w_end,
                                     [new_state](const icu::UnicodeString &s) {
                                       return s.startsWith(new_state.word);
                                     });
    new_state.w_end =
        std::find_if_not(new_state.w_begin, current_state.w_end,
                         [new_state](const icu::UnicodeString &s) {
                           return s.startsWith(new_state.word);
                         });
    new_state.m_begin = grid.PossibleMoves(new_state.position).cbegin();
    new_state.m_end = grid.PossibleMoves(new_state.position).cend();
    state.push_back(new_state);
    if (new_state.word == *new_state.w_begin) {
      if (already_found.find(new_state.word) == already_found.cend()) {
        const auto starred = IsStarred(already_visited, stars);
        already_found.insert(
            {new_state.word, starred ? Printed::yes : Printed::no});
        if (starred)
          std::cout << "* " << state[1].position << " " << new_state.word
                    << std::endl;
      }
    }
    if (new_state.w_begin == new_state.w_end) {
      already_visited[new_state.position] = false;
      state.pop_back();
      if (state.size() > 0)
        ++state.back().m_begin;
      continue;
    }
  }

  for (auto result : already_found)
    if (result.second == Printed::no)
      std::cout << result.first << std::endl;
  return 0;
}
