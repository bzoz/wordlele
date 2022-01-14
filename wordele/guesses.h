#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

/*
 *  Store a signle guess and its result.
 */
struct Guess {
	char word[5] = {0,0,0,0,0};
	std::uint16_t result = 0;
	
	/* Sets result of a guess at position.
	 *  'm' - green square
	 *  'i' - yellow square
	 *  'n' - gray square
	 */
	void set_result(int position, char guess_result) {
		switch (guess_result) {
		case 'm': result |= 0b11 << (position * 2); break;
		case 'i': result |= 0b10 << (position * 2); break;
		case 'n': result |= 0b01 << (position * 2); break;
		}
	}

	/* Takes two strings : tested word, e.g. "abbot" and result "minnng"
	 *  'm' - green square
	 *  'i' - yellow square
	 *  'n' - gray square
	 */
	static Guess from_result(const char* guess, const char* results) {
		Guess r;
		for (int i = 0; i < 5; ++i) {
			r.word[i] = guess[i];
			r.set_result(i, results[i]);
		}
		return r;
	}

	// Creates an instace from a result of testing a give guess with give solution
	static Guess from_words(const char* guess, const char* solution) {
		Guess r;
		std::int8_t counts[256] = {};
		for (int i = 0; i < 5; ++i) {
			++counts[solution[i]];
		}
		for (int i = 0; i < 5; ++i) {
			r.word[i] = guess[i];
			if (guess[i] == solution[i]) {
				--counts[guess[i]];
				r.set_result(i, 'm');
			}
			else if (counts[guess[i]] > 0) {
				--counts[guess[i]];
				r.set_result(i, 'i');
			}
			else {
				r.set_result(i, 'n');
			}
		}
		return r;
	}
};

/*
* Store a sequence of guesses and their results.
*/
class Guesses {
public:
	// Add new guess
	void add_guess(Guess guess) {
		guesses.push_back(guess);
	}
	/* Test if tried_solution will produce the same guess results as previous guesses
	 *  - proper solution should always return true here.
	 */
	bool matches(const char* tried_solution) const {
		for (const auto& guess : guesses) {
			if (strncmp(tried_solution, guess.word, 5) == 0) {
				return false;
			}
			auto tried_guess = Guess::from_words(guess.word, tried_solution);
			if (guess.result != tried_guess.result) {
				return false;
			}
		}
		return true;
	}
private:
	std::vector<Guess> guesses;
};
