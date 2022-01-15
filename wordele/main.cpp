#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_set>
#include <ppl.h>
#include "wordlist.h"
#include "guesses.h"

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

  // tests...
  std::unordered_set<const char*> tried_words = {
    "roate", "hault", "tawny"
  };
  Guesses guesses;
  guesses.add_guess(Guess::from_result("roate", "nninn"));
  guesses.add_guess(Guess::from_result("lysin", "nnnmi"));
  //guesses.add_guess(Guess::from_result("nasal", "imnnn"));
  //guesses.add_guess(Guess::from_result("candy", "immnn"));
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
  minimize_avgerage_possible_solutions(possible_solutions, possible_solutions);

}