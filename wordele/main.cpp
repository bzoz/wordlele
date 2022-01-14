#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_set>
#include <ppl.h>
#include "wordlist.h"
#include "word.h"
#include "guesses.h"

static std::string starting_string = "roate";
static Word starting_word = Word::from_string(starting_string.c_str());

struct SolverResult {
	Word word;
	double score;
};

// The solvers:
SolverResult minimize_avgerage_possible_solutions(const std::vector<Word>& all_words, const std::vector<Word>& possible_solutions) {
	std::mutex mutex;
	unsigned cnt = 0;
	unsigned min_word_cnt = 0xffffffff;
	Word min_word;

	concurrency::parallel_for_each(begin(all_words), end(all_words), [&](Word tested_word) {
		unsigned int this_cnt = 0;
		for (auto tested_solution : possible_solutions) {
			auto constrains = Constrains::from_words(tested_word, tested_solution);
			for (auto word : possible_solutions) {
				if (constrains.meets_constrains(word))
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
	return { min_word, (double) min_word_cnt / possible_solutions.size() };
}

SolverResult minimize_maximum_possible_solutions(const std::vector<Word>& all_words, const std::vector<Word>& possible_solutions) {
	std::mutex mutex;
	unsigned cnt = 0;
	unsigned min_word_cnt = 0xffffffff;
	Word min_word;

	concurrency::parallel_for_each(begin(all_words), end(all_words), [&](Word tested_word) {
		unsigned int max_cnt = 0;
		for (auto tested_solution : possible_solutions) {
			auto constrains = Constrains::from_words(tested_word, tested_solution);
			int this_cnt = 0;
			for (auto word : possible_solutions) {
				if (constrains.meets_constrains(word))
					++this_cnt;
			}
			if (this_cnt > max_cnt)
				max_cnt = this_cnt;
		}
		mutex.lock();
		++cnt;
		if (max_cnt < min_word_cnt) {
			min_word_cnt = max_cnt;
			min_word = tested_word;
		}
		mutex.unlock();
	});
	return { min_word, (double)min_word_cnt };
}

/*SolverResult most_matching_word(const std::vector<Word>& all_words, const std::vector<Word>& possible_solutions) {
	std::mutex mutex;
	unsigned cnt = 0;
	double max_letter_match = 0;
	Word max_word;

	concurrency::parallel_for_each(begin(all_words), end(all_words), [&](Word tested_word) {
		double this_word_matches = 0;
		for (auto tested_solution : possible_solutions) {
			std::uint32_t letters_in_s
			for (int p = 0; p < 5; ++p) {
				auto tested_char_index = tested_word.get_letter_index(p);
				auto solution_char_index = tested_solution.get_letter_index(p);
				if (tested_char_index)
			}
		}
		mutex.lock();
		++cnt;
		if (this_word_matches > max_letter_match) {
			max_letter_match = this_word_matches;
			max_word = tested_word;
		}
		mutex.unlock();
		});
	return { max_word, max_letter_match };
}*/

int solve(std::vector<Word> all_words, std::vector<Word> possible_solutions, Word solution) {
	int iterations = 0;
	std::unordered_set<std::uint32_t> tried_words;
	Word next_word = starting_word;
	Constrains constrains;
	while (possible_solutions.size() > 1) {
		++iterations;
		tried_words.insert(next_word.word);
		constrains = Constrains::merge(constrains, Constrains::from_words(next_word, solution));
		all_words.erase(std::remove_if(begin(all_words), end(all_words), [&](Word word) {
			return (tried_words.find(word.word) != tried_words.end()) || !constrains.meets_constrains(word);
		}), end(all_words));
		possible_solutions.erase(std::remove_if(begin(possible_solutions), end(possible_solutions), [&](Word word) {
			return (tried_words.find(word.word) != tried_words.end()) || !constrains.meets_constrains(word);
		}), end(possible_solutions));
		next_word = minimize_avgerage_possible_solutions(all_words, possible_solutions).word;
		auto str = next_word.to_string();
	}
	if (possible_solutions.size() > 0) {
		auto str = possible_solutions[0].to_string();
	}
	return iterations;
}

void try_all(const std::vector<Word>& all_words, const std::vector<Word>& possible_solutions) {

}

int main3() {
	Constrains constrains;
	//constrains = Constrains::from_results("roate", "bbyby");
	//constrains = Constrains::merge(constrains, Constrains::from_results("genal", "bybyb"));
	/*auto redable = constrains.readable();
	std::cout << "Constrains:\n";
	std::cout << " invalid letters: " << redable.invalid_letters << "\n";
	for (int i = 0; i < 5; ++i) {
		std::cout << " invalid letter at " << i + 1 << ": " << redable.invalid_letters_at_positions[i] << "\n";
	}
	std::cout << " must have letters: " << redable.must_have_letters << "\n";
	std::cout << " know letters: " << redable.known_letters << "\n";*/
	std::vector<Word> all_words, possible_solutions;
	for (auto w : words) {
		auto word = Word::from_string(w);
		if (constrains.meets_constrains(word))
			all_words.push_back(word);
	}
	for (auto w : solutions) {
		auto word = Word::from_string(w);
		if (constrains.meets_constrains(word))
			possible_solutions.push_back(word);
	}
	std::cout << "All words: " << all_words.size() << ", possbile solutions: " << possible_solutions.size() << "\n";
	if (possible_solutions.size() < 10) {
		for (auto possible : possible_solutions)
			std::cout << "- " << possible.to_string() << "\n";
	}
	//solve(all_words, possible_solutions, Word::from_string("abbey"));
	/*
	std::cout << "Use all words: " << best_word(all_words, possible_solutions).to_string() << "\n"; */
	//std::cout << "Use only solutions: " << best_word(possible_solutions, possible_solutions).to_string() << "\n";
	std::cout << "Use all words: " << minimize_avgerage_possible_solutions(all_words, possible_solutions).word.to_string() << "\n";
	return 0;
}


void minimize_avgerage_possible_solutions2(const std::vector<const char*>& all_words, const std::vector<const char*>& possible_solutions) {
	std::mutex mutex;
	unsigned cnt = 0;
	unsigned min_word_cnt = 0xffffffff;
	const char* min_word;

	concurrency::parallel_for_each(begin(all_words), end(all_words), [&](const char* tested_word) {
		unsigned int this_cnt = 0;
		for (auto tested_solution : possible_solutions) {
			Guesses constrains;
			constrains.add_guess(Guess::from_words(tested_word, tested_solution));
			for (auto word : possible_solutions) {
				if (constrains.matches(word))
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
	std::vector<const char*> all_words(words, words + sizeof(words) / sizeof(*words));
	std::vector<const char*> possible_solutions(solutions, solutions + sizeof(solutions) / sizeof(*solutions));
	/*std::unordered_set<const char*> tried_words = {
		"roate", "hault", "tawny"
	};
	Guesses guesses;
	guesses.add_guess(Guess::from_result("roate", "nniin"));
	guesses.add_guess(Guess::from_result("hault", "nmnni"));
	guesses.add_guess(Guess::from_result("tawny", "mmnim"));
	all_words.erase(std::remove_if(begin(all_words), end(all_words), [&](const char* word) {
		return (tried_words.find(word) != tried_words.end()) || !guesses.matches(word);
		}), end(all_words));
	possible_solutions.erase(std::remove_if(begin(possible_solutions), end(possible_solutions), [&](const char* word) {
		return (tried_words.find(word) != tried_words.end()) || !guesses.matches(word);
		}), end(possible_solutions));
	std::cout << possible_solutions.size() << "\n";*/
	minimize_avgerage_possible_solutions2(all_words, possible_solutions);

}