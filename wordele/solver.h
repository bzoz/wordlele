#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "guesses.h"

template <typename GetNextGuess>
Guesses solve(std::vector<const char*> all_words,
          std::vector<const char*> possible_solutions,
          bool hard_mode,
          GetNextGuess get_next_guess) {
  Guesses guesses;
  bool first = true;
  while (!guesses.found_solution() && guesses.count() <= 6) {
    guesses.add_guess(get_next_guess(guesses, all_words, possible_solutions, first));
    first = false;
    if (hard_mode) {
      all_words.erase(std::remove_if(begin(all_words), end(all_words), [&](const char* word) {
        return !guesses.matches(word);
        }), end(all_words));
    }
    possible_solutions.erase(std::remove_if(begin(possible_solutions), end(possible_solutions), [&](const char* word) {
      return !guesses.matches(word);
      }), end(possible_solutions));
  }
  return guesses;
};

template <typename BestWordSelector>
struct BatchGuesser {
  BatchGuesser(const char* solution) : solution(solution) {}
  Guess operator()(const Guesses& guesses,
                   const std::vector<const char*>& all_words,
                   const std::vector<const char*>& possible_solutions,
                   bool first_word) {
    return Guess::from_words(best_word_selector(guesses, all_words, possible_solutions, first_word), solution);
  }
  const char* solution;
  BestWordSelector best_word_selector;
};

template <typename BestWordSelector>
struct InteractiveGuesser {
  Guess operator()(const Guesses& guesses,
    const std::vector<const char*>& all_words,
    const std::vector<const char*>& possible_solutions,
    bool first_word) {
    auto guess = best_word_selector(guesses, all_words, possible_solutions, first_word);
    std::cout << "Guess: " << guess << "\nResult? ";
    std::string result;
    std::cin >> result;
    return Guess::from_result(guess, result.c_str());
  }
  const char* solution;
  BestWordSelector best_word_selector;
  bool first_run = true;
};
