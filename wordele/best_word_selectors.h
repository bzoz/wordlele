#pragma once
#include <vector>
#include <mutex>
#include <ppl.h>
#include "guesses.h"

struct MinimizeAveragePossibleSolutions {
  const char* operator()(const Guesses& guesses,
    const std::vector<const char*>& all_words,
    const std::vector<const char*>& possible_solutions) {
    std::mutex mutex;
    unsigned cnt = 0;
    unsigned min_word_cnt = 0xffffffff;
    const char* min_word = "(none)";

    concurrency::parallel_for_each(begin(all_words), end(all_words), [&](const char* tested_word) {
      unsigned int this_cnt = 0;
      for (auto tested_solution : possible_solutions) {
        Guesses guess;
        guess.add_guess(Guess::from_words(tested_word, tested_solution));
        for (auto word : possible_solutions) {
          if (guess.matches(word))
            ++this_cnt;
        }
      }
      mutex.lock();
      ++cnt;
      if (this_cnt < min_word_cnt) {
        min_word_cnt = this_cnt;
        min_word = tested_word;
      }
      mutex.unlock();
      });

    return min_word;
  }

};
