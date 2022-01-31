#pragma once
#include <vector>
#include <mutex>
#include <ppl.h>
#include "guesses.h"

struct MinimizeAveragePossibleSolutions {
  const char* best_first = { "roate" };
  const char* operator()(const Guesses& guesses,
                         const std::vector<const char*>& all_words,
                         const std::vector<const char*>& possible_solutions,
                         bool first_word) const {
    if (first_word) {
      return best_first;
    }
    std::mutex mutex;
    unsigned cnt = 0;
    unsigned min_word_cnt = 0xffffffff;
    const char* min_word = "?????";

    const std::vector<const char*>& dictionary = possible_solutions.size() < 5 ? possible_solutions : all_words;
    concurrency::parallel_for_each(begin(dictionary), end(dictionary), [&](const char* tested_word) {
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

struct MinimizeMaxPossibleSolutions {
  const char* best_first = { "aesir" };
  const char* operator()(const Guesses& guesses,
                         const std::vector<const char*>& all_words,
                         const std::vector<const char*>& possible_solutions,
                         bool first_word) const {
    if (first_word) {
      return best_first;
    }
    std::mutex mutex;
    unsigned cnt = 0;
    unsigned min_word_cnt = 0xffffffff;
    const char* min_word = "?????";

    const std::vector<const char*>& dictionary = possible_solutions.size() < 5 ? possible_solutions : all_words;
    concurrency::parallel_for_each(begin(dictionary), end(dictionary), [&](const char* tested_word) {
      unsigned int this_max = 0;
      for (auto tested_solution : possible_solutions) {
        unsigned this_cnt = 0;
        Guesses guess;
        guess.add_guess(Guess::from_words(tested_word, tested_solution));
        for (auto word : possible_solutions) {
          if (guess.matches(word))
            ++this_cnt;
        }
        if (this_cnt > this_max) {
          this_max = this_cnt;
        }
      }
      mutex.lock();
      ++cnt;
      if (this_max < min_word_cnt) {
        min_word_cnt = this_max;
        min_word = tested_word;
      }
      mutex.unlock();
      });

    return min_word;
  }
};


struct MaximizeScore {
  const char* best_first = { "soare" };
  const char* operator()(const Guesses& guesses,
                         const std::vector<const char*>& all_words,
                         const std::vector<const char*>& possible_solutions,
                         bool first_word) const {
    if (first_word) {
      return best_first;
    }
    std::mutex mutex;
    double max_score = 0;
    const char* max_word = "?????";

    const std::vector<const char*>& dictionary = possible_solutions.size() < 5 ? possible_solutions : all_words;
    concurrency::parallel_for_each(begin(dictionary), end(dictionary), [&](const char* tested_word) {
      double total_score = 0;
      for (auto tested_solution : possible_solutions) {
        total_score += Guess::from_words(tested_word, tested_solution).score();
      }
      double score = total_score / possible_solutions.size();
      mutex.lock();
      if (max_score < score) {
        max_score = score;
        max_word = tested_word;
      }
      mutex.unlock();
      });

    return max_word;
  }
};