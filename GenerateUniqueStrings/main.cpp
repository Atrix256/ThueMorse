#include <stdio.h>
#include <string>
#include <bit>
#include <unordered_set>
#include <vector>
#include <unordered_map>

using uint = unsigned int;

static const uint c_numBits = 5;
static const uint c_ARTSequenceLetters = 32;

// The Thue-Morse bit at index i is the sum of the 1 bts in the binary number i, modulo 2
std::string ThueMorse(uint n)
{
	if (n == 0)
		return "";
	std::string ret;
	ret.resize(n, ' ');

	for (uint i = 0; i < n; ++i)
		ret[i] = ((std::popcount(i) % 2) == 1) ? '1' : '0';

	return ret;
}

// Equation 3 from https://www.cs.jhu.edu/~misha/ReadingSeminar/Papers/Ahmed17.pdf
// 0 -> 01
// 1 -> 10
// The Thue-Morse sequence is a fixed point of this mapping - This mapping will
// make the sequence longer, but any existing digits will remain the same.
// That is not strictly true when taking substrings from the middle of the sequence.
//
// Example: "10010" does have this property and becomes "1001011001, but removing the
// first digit from that example to make "0010" does not have this property and ends up
// being transformed into "01011001".
std::string ThueMorseHomomorphism(const std::string& src)
{
	std::string ret;
	for (char c : src)
	{
		switch (c)
		{
			case '0': ret += "01"; break;
			case '1': ret += "10"; break;
		}
	}
	return ret;
}

std::string BitInverse(const std::string& s)
{
	std::string ret;
	uint n = (uint)s.length();
	ret.resize(n, ' ');

	for (uint i = 0; i < n; ++i)
		ret[i] = (s[i] == '1') ? '0' : '1';

	return ret;
}

uint Pow2LTE(uint n)
{
	uint ret = 1;
	while (ret * 2 <= n)
		ret *= 2;
	return ret;
}

void ScanSubstrings(const std::string& s, unsigned int length, std::unordered_set<std::string>& uniques, std::vector<std::string>& orderedUniques)
{
	uint maxOffset = (uint)s.length() - length + 1;
	for (uint offset = 0; offset < maxOffset; ++offset)
	{
		std::string newString = s.substr(offset, length);
		if (uniques.count(newString) == 0)
		{
			orderedUniques.push_back(newString);
			uniques.insert(newString);
		}
	}
}

struct SymbolInfo
{
	// Self
	std::string bits;
	char letter = '-';

	// Possible sibblings (for iteration spatially)
	std::string on0Bits;
	char on0Letter = '-';

	std::string on1Bits;
	char on1Letter = '-';

	// Children (for recursion)
	std::string morphedIDBits;

	std::string child0Bits;
	char child0Letter = '-';

	std::string child1Bits;
	char child1Letter = '-';
};

char GetLetterFromLetterIndex(uint letterIndex)
{
	if (letterIndex < 26)
		return 'A' + char(letterIndex);
	letterIndex -= 26;

	if (letterIndex < 10)
		return '0' + char(letterIndex);
	letterIndex -= 10;

	if (letterIndex < 26)
		return 'a' + char(letterIndex);
	letterIndex -= 26;

	return '?';
}

int main(int argc, char** argv)
{
	std::unordered_set<std::string> uniques;
	std::vector<std::string> orderedUniques;

	{
#if 0
		// Make A and B
		uint M = Pow2LTE(c_numBits);
		std::string A = ThueMorse(M);
		std::string B = BitInverse(A);

		// Scan for unique symbols
		ScanSubstrings(A + B, c_numBits, uniques, orderedUniques);
		ScanSubstrings(B + B, c_numBits, uniques, orderedUniques);
		ScanSubstrings(B + A, c_numBits, uniques, orderedUniques);
		ScanSubstrings(A + A, c_numBits, uniques, orderedUniques);
#else
		// Scan the full set: ABBABAAB
		uint M = Pow2LTE(c_numBits) * 8;
		std::string A = ThueMorse(M);
		ScanSubstrings(A, c_numBits, uniques, orderedUniques);
#endif
	}

	// make the symbol info
	std::unordered_map<std::string, SymbolInfo> symbolInfo;
	{
		// make the bits and letter for each
		uint letterIndex = 0;
		for (const std::string& s : orderedUniques)
		{
			SymbolInfo newSymbol;
			newSymbol.bits = s;
			newSymbol.letter = GetLetterFromLetterIndex(letterIndex);
			symbolInfo[s] = newSymbol;
			letterIndex++;
		}

		// find the on0 and on1 siblings
		for (auto& symbolPair : symbolInfo)
		{
			SymbolInfo& info = symbolPair.second;

			// on0
			{
				std::string on0 = std::string(&info.bits[1]) + "0";
				auto it = symbolInfo.find(on0);
				if (it != symbolInfo.end())
				{
					info.on0Bits = on0;
					info.on0Letter = it->second.letter;
				}
			}

			// on1
			{
				std::string on1 = std::string(&info.bits[1]) + "1";
				auto it = symbolInfo.find(on1);
				if (it != symbolInfo.end())
				{
					info.on1Bits = on1;
					info.on1Letter = it->second.letter;
				}
			}
		}

		// Find the child ids
		for (auto& symbolPair : symbolInfo)
		{
			SymbolInfo& info = symbolPair.second;
			info.morphedIDBits = ThueMorseHomomorphism(info.bits);

			info.child0Bits = info.morphedIDBits.substr(c_numBits * 2 - c_numBits - 1, c_numBits);
			if (symbolInfo.find(info.child0Bits) != symbolInfo.end())
				info.child0Letter = symbolInfo[info.child0Bits].letter;

			info.child1Bits = info.morphedIDBits.substr(c_numBits * 2 - c_numBits, c_numBits);
			if (symbolInfo.find(info.child1Bits) != symbolInfo.end())
				info.child1Letter = symbolInfo[info.child1Bits].letter;
		}
	}

	// Print what we found
	printf("Unique %u bit strings in the Thue-Morse sequence:\n", c_numBits);
	for (const std::string& s : orderedUniques)
	{
		const SymbolInfo& info = symbolInfo[s];
		printf("    %s | %c | %c %c | %s | %c %c\n", s.c_str(), info.letter, info.on0Letter, info.on1Letter, info.morphedIDBits.c_str(), info.child0Letter, info.child1Letter);
	}
	printf("%u strings total\n", (uint)uniques.size());

	// Print out the ART sequence
	{
		printf("\nThe first %u symbols of the Thue-Morse sequence:\n", c_ARTSequenceLetters);
		std::string bits = ThueMorse(c_numBits + c_ARTSequenceLetters - 1);
		printf("bits   : %s\nsymbols: ", bits.c_str());

		for (uint i = 0; i < c_ARTSequenceLetters; ++i)
		{
			std::string s = bits.substr(i, c_numBits);

			auto it = symbolInfo.find(s);
			printf("%c", (it == symbolInfo.end()) ? '?' : it->second.letter);
		}
		printf("\n");
	}

	return 0;
}

/*
To generate the unique set of N bit strings found in the Thue-Morse sequence...
1) Find a pow of 2 M <= N. Generate the sequence of that size, call it A.
2) Bit reverse it to generate the next M items in the sequence, call it B.

We could scan the string ABBABAAB and keep unique N bit strings.
That sequence is the Thue-Morse sequence and contains all four combinations of A and B: AA, AB, BA, BB.

Construction rules:
A -> AB
B -> BA

Doing that 3 times:
A -> AB -> ABBA -> ABBABAAB

That sequence is 8*M long though.  We could instead just scan these 4:
AA
AB
BA
BB

That's a little more efficient because there are no duplicates of letter combinations being scanned.

To make them be in the order they are found in the actual sequence, we can instead scan them in this order:
AB
BB
BA
AA

TODO: 7 bits didn't work with the logic as I implemented it so i went back to just looking at M*8. investigate why.
I think this is because with M = 4, it can span 3 symbols and there are more combinations.
AAB
ABA
ABB
BAA
BAB
BBA
Not these though since the sequence is cube free:
AAA
BBB

Thanks to ryg for working through some of this with me!
*/