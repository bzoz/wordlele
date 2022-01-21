#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_set>
#include <ppl.h>
#include "wordlist.h"
#include "guesses.h"
#include "solver.h"
#include "best_word_selectors.h"

// Finds a word that minimizes the average number of possible solutions
void minimize_avgerage_possible_solutions(const std::vector<const char*>& all_words, const std::vector<const char*>& possible_solutions) {
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
      std::cout << cnt << "/" << all_words.size() << " " << min_word << " - " << min_word_cnt << "\n";
    }
    mutex.unlock();
  });
}

int main() {
  // Load words into vector for easier manipulation
  std::vector<const char*> all_words(words, words + sizeof(words) / sizeof(*words));
  std::vector<const char*> possible_solutions(solutions, solutions + sizeof(solutions) / sizeof(*solutions));
  std::vector<const char*> all_possible_solutions(solutions, solutions + sizeof(solutions) / sizeof(*solutions));
  /*MaximizeScore first_guess;
  auto guess = first_guess({}, all_words, possible_solutions, false);
  std::cout << guess << "\n";
  return 0;/**/
  /*std::vector<int> histogram;
  histogram.resize(7, 0);
  int cnt = 0;
  for (auto solution : possible_solutions) {
    auto result = solve(all_words, possible_solutions, false, BatchGuesser<MaximizeScore>(solution));
    ++cnt;
    if (cnt % 100 == 0 || result.count() > 6) {
      std::cout << cnt  <<"/" << possible_solutions.size()<<": " <<  solution << " - " << result.count() << "\n";
    }
    if (result.count() > 6) {
      histogram[0]++;
    }
    else {
      histogram[result.count()]++;
    }
  }
  std::cout << "\nFailed: " << histogram[0] << "\n";
  for (int g = 1; g <= 6; ++g) {
    std::cout << g << " guesses: " << histogram[g] << "\n";
  }*/
  
  Guesses guesses;
  guesses.add_guess(Guess::from_result("roate", "innnn"));
  guesses.add_guess(Guess::from_result("sculk", "ninnm"));
  guesses.add_guess(Guess::from_result("upbow", "ninnn"));
  //guesses.add_guess(Guess::from_result("pugil", "mnnnn"));
  /*all_words.erase(std::remove_if(begin(all_words), end(all_words), [&](const char* word) {
    return !guesses.matches(word);
    }), end(all_words));*/
  possible_solutions.erase(std::remove_if(begin(possible_solutions), end(possible_solutions), [&](const char* word) {
    return !guesses.matches(word);
    }), end(possible_solutions));
  std::cout << possible_solutions.size() << "\n";
  if (possible_solutions.size() < 10) {
    for (auto possible_solution : possible_solutions) {
      std::cout << "  " << possible_solution << "\n";
    }
  }
  minimize_avgerage_possible_solutions(all_words, possible_solutions);
  minimize_avgerage_possible_solutions(all_possible_solutions, possible_solutions);
  minimize_avgerage_possible_solutions(possible_solutions, possible_solutions);
  /**/
}