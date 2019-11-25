#include "wordlist.h"

#include <fstream>

WordList CreateWordList(std::experimental::filesystem::path path) {
  WordList word_list;
  std::ifstream f(path, std::ios::in);
  if (!f.is_open())
    throw std::runtime_error(std::string{"Failed to open file "} +
                             path.string());

  auto it = word_list.cbegin();
  std::string line;
  while (std::getline(f, line))
    it = word_list.emplace_hint(it, icu::UnicodeString::fromUTF8(line));
  return word_list;
}
