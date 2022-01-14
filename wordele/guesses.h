#pragma once
#include <vector>
#include <cstdint>

struct Guess {
	char word[5] = {0,0,0,0,0};
	std::uint16_t result = 0;
	
	void set_result(int position, char guess_result) {
		switch (guess_result) {
		case 'm':  result |= 0b11 << (position * 2); break;
		case 'i': result |= 0b10 << (position * 2); break;
		case 'n':   result |= 0b01 << (position * 2); break;
		}
	}

	static Guess from_result(const char* guess, const char* results) {
		Guess r;
		for (int i = 0; i < 5; ++i) {
			r.word[i] = guess[i];
			r.set_result(i, results[i]);
		}
		return r;
	}
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

class Guesses {
public:
	void add_guess(Guess guess) {
		guesses.push_back(guess);
	}
	bool matches(const char* tried_solution) const {
		for (const auto& guess : guesses) {
			auto tried_guess = Guess::from_words(guess.word, tried_solution);
			if (guess.result != tried_guess.result)
				return false;
		}
		return true;
	}
private:
	std::vector<Guess> guesses;
};