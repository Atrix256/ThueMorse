#include <stdio.h>
#include <string>
#include <bit>
#include <unordered_set>
#include <vector>
#include <unordered_map>

using uint = unsigned int;

static const uint c_numBits = 5;

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

// Double the size of the Thue-Morse sequence given, using string substitution.
// The subsequence doesn't have to start at the beginning of the sequence.
// If src is length N, this will give the next N entries in the Thue-Morse sequence.
// 0 -> 01
// 1 -> 10
std::string ThueMorseSequenceDouble(const std::string& src)
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

int main(int argc, char** argv)
{
	// Make A and B
	uint M = Pow2LTE(c_numBits);
	std::string A = ThueMorse(M);
	std::string B = BitInverse(A);

	// Scan for unique symbols
	std::unordered_set<std::string> uniques;
	std::vector<std::string> orderedUniques;
	ScanSubstrings(A + B, c_numBits, uniques, orderedUniques);
	ScanSubstrings(B + B, c_numBits, uniques, orderedUniques);
	ScanSubstrings(B + A, c_numBits, uniques, orderedUniques);
	ScanSubstrings(A + A, c_numBits, uniques, orderedUniques);

	// make the symbol info
	std::unordered_map<std::string, SymbolInfo> symbolInfo;
	{
		// make the bits and letter for each
		char letter = 'A';
		for (const std::string& s : orderedUniques)
		{
			SymbolInfo newSymbol;
			newSymbol.bits = s;
			newSymbol.letter = letter;
			symbolInfo[s] = newSymbol;
			letter++;
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
			info.morphedIDBits = ThueMorseSequenceDouble(info.bits);

			info.child0Bits = info.morphedIDBits.substr(c_numBits * 2 - c_numBits - 1, c_numBits);
			info.child0Letter = symbolInfo[info.child0Bits].letter;

			info.child1Bits = info.morphedIDBits.substr(c_numBits * 2 - c_numBits, c_numBits);
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

	// TODO: Print out the art sequence

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

Thanks to ryg for working through some of this with me!
*/