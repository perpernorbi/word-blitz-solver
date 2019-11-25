#pragma once

#include <experimental/filesystem>
#include <set>
#include <unicode/unistr.h>

using WordList = std::set<icu::UnicodeString>;
WordList CreateWordList(std::experimental::filesystem::path path);
