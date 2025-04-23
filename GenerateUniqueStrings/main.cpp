#include <stdio.h>
#include <string>
#include <bit>
#include <unordered_set>
#include <vector>

using uint = unsigned int;

static const uint c_numBits = 6;

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

int main(int argc, char** argv)
{
	uint M = Pow2LTE(c_numBits);
	std::string A = ThueMorse(M);
	std::string B = BitInverse(A);

	std::unordered_set<std::string> uniques;
	std::vector<std::string> orderedUniques;

	ScanSubstrings(A + B, c_numBits, uniques, orderedUniques);
	ScanSubstrings(B + B, c_numBits, uniques, orderedUniques);
	ScanSubstrings(B + A, c_numBits, uniques, orderedUniques);
	ScanSubstrings(A + A, c_numBits, uniques, orderedUniques);

	printf("Unique %u bit strings in the Thue-Morse sequence:\n", c_numBits);

	for (const std::string& s : orderedUniques)
		printf("    %s\n", s.c_str());
	printf("%u strings total\n", (uint)uniques.size());

	return 0;
}

/*
To generate the unique set of N bit strings found in the Thue-Morse sequence...
1) Find a pow of 2 M < N. Generate the sequence of that size, call it A.
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