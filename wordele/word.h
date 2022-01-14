#pragma once
#include <vector>
#include <string>
#include <cstdint>

/*
	General idea:
		we can store all the info about a word inside a single uint64_t

	We need 5 bits for each letter. There are 5 of those, so 25 bits in total. This
	fits a std::uint32_t perfectly.

	We also need to keep track of what letters are used in a word. We need 26 bits,
	so - another std::uint32_t.

	With this, we can check if a word matches given constrains (used letter, specific
	letters at specific positions) using a bunch of bitwise AND and XOR operations.
*/

namespace {
	// 5 bits to store 26 different values ('a' - 'z')
	constexpr unsigned letter_bitwidth = 5;
	// 5 bits set to 1 mask
	constexpr unsigned set_5_bits = 0b11111;

	// 'a' = 0, 'b' = 1, etc...
	constexpr std::uint8_t letter_index(char letter) {
		return letter - 'a';
	}

	constexpr char letter_from_index(std::uint8_t index) {
		return 'a' + index;
	}

	// We need 5 bits to store each letter. Calculate letter position in  a bitfield
	constexpr std::int32_t letter_bit_offset(int position) {
		return position * letter_bitwidth;
	}

	// 0b1 for 'a', 0b10 for 'b', 0b100 for 'c', etc... 
	constexpr std::int32_t used_letter_bitmask(char letter) {
		return 1 << letter_index(letter);
	}
	constexpr std::int32_t used_letter_index_bitmask(std::int8_t letter_index) {
		return 1 << letter_index;
	}

	// Merged bitmask to string
	std::string used_letters_string(std::int32_t used_letters) {
		std::string letters;
		for (char c = 'a'; c <= 'z'; ++c) {
			if (used_letters & used_letter_bitmask(c))
				letters.push_back(c);
		}
		return letters;
	}
}

struct Word {
	// Word encoded as 5 sets of 5-bit values
	std::uint32_t word = 0;
	void set_letter(char letter, int position) {
		set_letter_index(letter_index(letter), position);
	}
	void set_letter_index(std::int8_t letter_index, int position) {
		word |= letter_index << letter_bit_offset(position);
	}
	char get_letter(int position) const {
		return letter_from_index(get_letter_index(position));
	}
	std::int8_t get_letter_index(int position) const {
		return (word >> letter_bit_offset(position)) & set_5_bits;
	}

	// Bitmask of letters used in a word
	std::uint32_t used_letters = 0;
	void set_used_letter(char c) {
		set_used_letter_index(letter_index(c));
	}
	void set_used_letter_index(std::int8_t letter_index) {
		used_letters |= used_letter_index_bitmask(letter_index);
	}
	bool uses_letter(char c) const {
		return used_letters & used_letter_bitmask(c);
	}
	bool uses_letter_index(std::uint8_t letter_index) const {
		return used_letters & used_letter_index_bitmask(letter_index);
	}

	static Word from_string(const char* letters) {
		Word word;
		for (int position = 0; *letters != 0; ++letters, ++position) {
			word.set_used_letter(*letters);
			word.set_letter(*letters, position);
		}
		return word;
	}

	std::string to_string() const {
		std::string word;
		for (int p = 0; p < 5; ++p)
			word.push_back(get_letter(p));
		return word;
	}
};

struct RedableConstrains {
	std::string invalid_letters;
	std::vector<std::string> invalid_letters_at_positions;
	std::string must_have_letters;
	std::string known_letters;
};

struct Constrains {
	// Letters that are not in the solution
	std::uint32_t invalid_letters = 0;
	// Letters that we known are not ok on given position. I.e. the yellow square
	std::uint32_t invalid_position_letters[5] = { 0, 0, 0, 0, 0 };
	// Letters that are in the solution
	std::uint32_t must_have_letters = 0;
	// Bitmask of known letters - 0b11111 in specific bit position if we know a letter
	// at that position
	std::uint32_t known_letters_mask = 0;
	// Specific letters that must match
	std::uint32_t known_letters = 0;

	void set_known_letter_at_position(char letter, int position) {
		must_have_letters |= used_letter_bitmask(letter);
		known_letters_mask |= set_5_bits << letter_bit_offset(position);
		known_letters |= letter_index(letter) << letter_bit_offset(position);
	}

	void set_known_letter(char letter) {
		must_have_letters |= used_letter_bitmask(letter);
	}

	void set_invalid_letter(char letter) {
		invalid_letters |= used_letter_bitmask(letter);
	}

	void set_invalid_letter_at_position(char letter, int position) {
		invalid_position_letters[position] |= used_letter_bitmask(letter);
	}

	bool meets_constrains(Word word) {
		auto invalid_letters_string = used_letters_string(invalid_letters);
		auto must_use_letters_string = used_letters_string(must_have_letters);
		auto word_letters = used_letters_string(word.used_letters);
		bool does_not_have_invalid_letters = (word.used_letters & invalid_letters) == 0;
		bool uses_all_must_have_letters = (word.used_letters & must_have_letters) == must_have_letters;
		bool known_letters_match = ((word.word ^ known_letters) & known_letters_mask) == 0;
		bool no_invalid_letters_at_position = true;
		for (int position = 0; position < 5; ++position) {
			auto letter_index = word.get_letter_index(position);
			no_invalid_letters_at_position = no_invalid_letters_at_position && !(used_letter_index_bitmask(letter_index) & invalid_position_letters[position]);
		}
		return does_not_have_invalid_letters && uses_all_must_have_letters && known_letters_match && no_invalid_letters_at_position;
	}

	static Constrains merge(Constrains a, Constrains b) {
		Constrains merged;
		merged.invalid_letters = a.invalid_letters | b.invalid_letters;
		merged.must_have_letters = a.must_have_letters | b.must_have_letters;
		merged.known_letters_mask = a.known_letters_mask | b.known_letters_mask;
		merged.known_letters = a.known_letters | b.known_letters;
		for (int i = 0; i < 5; ++i)
			merged.invalid_position_letters[i] = a.invalid_position_letters[i] | b.invalid_position_letters[i];
		return merged;
	}

	static Constrains from_words(Word tested, Word solution) {
		Constrains constrains;
		for (int position = 0; position < 5; ++position) {
			std::uint8_t tested_letter = (tested.word >> letter_bit_offset(position)) & set_5_bits;
			std::uint8_t solution_letter = (solution.word >> letter_bit_offset(position)) & set_5_bits;
			if (tested_letter == solution_letter) {
				constrains.set_known_letter_at_position(letter_from_index(tested_letter), position);
			}
			else {
				constrains.set_invalid_letter_at_position(letter_from_index(tested_letter), position);
			}
		}
		constrains.must_have_letters = tested.used_letters & solution.used_letters;
		constrains.invalid_letters = (~solution.used_letters) & tested.used_letters;
		return constrains;
	}
	
	static Constrains from_results(const char* tested, const char* result) {
		Constrains constrains;
		for (int position = 0; position < 5; ++position) {
			auto letter = tested[position];
			switch (result[position]) {
			case 'g':
				constrains.set_known_letter_at_position(letter, position);
				break;
			case 'y':
				constrains.set_invalid_letter_at_position(letter, position);
				constrains.set_known_letter(letter);
				break;
			case 'b':
				constrains.set_invalid_letter_at_position(letter, position);
				constrains.set_invalid_letter(letter);
				break;
			}
		}
		return constrains;
	}

	RedableConstrains readable() const {
		RedableConstrains redable;
		redable.invalid_letters_at_positions.resize(5);
		for (int p = 0; p < 5; ++p)
			redable.invalid_letters_at_positions[p] = used_letters_string(invalid_position_letters[p]);
		redable.invalid_letters = used_letters_string(invalid_letters);
		redable.must_have_letters = used_letters_string(must_have_letters);
		for (int p = 0; p < 5; ++p) {
			if ((known_letters_mask >> letter_bit_offset(p)) == 0) {
				redable.known_letters.push_back('?');
			}
			else {
				redable.known_letters.push_back(letter_from_index(known_letters >> letter_bit_offset(p)));
			}
		}
		return redable;
	}
};

