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

template<class GetNextGuess>
void do_histogram(const std::vector<const char*>& all_words, const std::vector<const char*>& possible_solutions) {
  std::vector<int> histogram;
  histogram.resize(7, 0);
  int cnt = 0;
  for (auto solution : possible_solutions) {
    GetNextGuess guesser(solution);
    auto result = solve(all_words, possible_solutions, true, guesser);
    ++cnt;
    if (cnt % 100 == 0 || result.count() > 6) {
      std::cout << cnt << "/" << possible_solutions.size() << ": " << solution << " - " << result.count() << "\n";
    }
    if (result.count() > 6) {
      histogram[0]++;
    }
    else {
      histogram[result.count()]++;
    }
  }
  for (int g = 1; g <= 6; ++g) {
    std::cout << g << " guesses: " << histogram[g] << "\n";
  }
  std::cout << "\nFailed: " << histogram[0] << "\n";
}

int main() {
  
  // Load words into vector for easier manipulation
  std::vector<const char*> all_words(words, words + sizeof(words) / sizeof(*words));
  std::vector<const char*> possible_solutions(solutions, solutions + sizeof(solutions) / sizeof(*solutions));
  all_words.insert(all_words.end(), possible_solutions.begin(), possible_solutions.end());
  
  //do_histogram<BatchGuesser<MinimizeAveragePossibleSolutions>>(all_words, possible_solutions);
  solve(all_words, possible_solutions, true, InteractiveGuesser<MinimizeAveragePossibleSolutions>{});
}