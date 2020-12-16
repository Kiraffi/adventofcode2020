
#include <algorithm>
#include <cassert>
#include <fstream>
#include <math.h>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <string.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>

struct MinMax
{
	int minValue;
	int maxValue;
};

struct NameType
{
	std::string name;
	std::vector<MinMax> minMaxes;
};

std::string parseStringUntil(const char **ptr, char parseChar)
{
	const char *&p = ptr[0];

	std::string name;
	while(*p && *p != parseChar)
	{
		name += *p++;
	}
	if(*p == '\0')
		return name;
	p++;

	return name;
}

std::string parseName(const char **ptr)
{
	const char *&p = ptr[0];

	std::string name;
	while(*p && *p != ' ')
	{
		name += *p++;
	}
	if(*p == '\0')
		return name;
	name += *p++;
	while(*p != ' ')
	{
		name += *p++;
	}
	if(*p == ' ')
		p++;
	
	return name;
}

int64_t parseNumber(const char **ptr)
{
	const char *&p = ptr[0];
	int64_t number = 0;
	bool neg = false;
	if(*p == '-')
	{
		neg = true;
		p++;
	}

	while(isdigit(*p))
		number = number * 10 + (*p++) - '0';
	if(*p != '\0')
		p++;
	return neg ? -number : number;
}

std::vector<int> parseNumbers(const std::string &s)
{
	std::vector<int> result;
	const char *ptr = s.data();
	bool valid = true;
	while(*ptr != '\0')
	{
		result.push_back(parseNumber(&ptr));
	}
	return result;
}
void findSpace(const char **ptr)
{
	const char *&p = ptr[0];
	while(!(*p == '\0' || *p == ' '))
		p++;
	if(*p == ' ')
		p++;
}

int getFirstSetBit(int64_t v)
{
	int bitCount = 0;
	assert(v != 0);
	while((v & 1) == 0)
	{
		++bitCount;
		v = v >> 1;
	}
	return bitCount;
}

int64_t removeInvalidNumbers(const std::string &s, const std::vector<NameType> &nameTypes, 
	std::vector<int> &validNumbers)
{
	int64_t sum = 0;
	int numberIndex = 0;
	const char *ptr = s.data();
	while(*ptr != '\0')
	{
		int removes = 0;
		int number = parseNumber(&ptr);
		bool foundAny = false;
		int i = 0;
		for(; i < nameTypes.size(); ++i)
		{
			for(const auto &m : nameTypes[i].minMaxes)
			{
				if(number >= m.minValue && number <= m.maxValue)
				{
					foundAny = true;
					goto foundAnyOne;
				}
			}
			// add the bits to remove if any is found
			removes |= (1 << i);
		}
		if(!foundAny)
		{
			sum += number;
			++numberIndex;
			continue;
		}
		foundAnyOne:
		int &p = validNumbers[numberIndex];
		p &= ~removes;
		for(; i < 31 && p; ++i)
		{
			if(((p >> i) & 1) == 0)
				continue;

			const NameType &a = nameTypes[i];
			bool found = false;
			for(const auto &m : a.minMaxes)
			{
				if(number >= m.minValue && number <= m.maxValue)
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				p &= ~(1 << i);
				// Check that we dont remove last bit
				assert(p != 0);
			}
		}
		++numberIndex;
	}
	return sum;
}

int readValues(const char *filename)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return -1;
	}

	std::vector<NameType> nameTypes;


	std::string s;
	while(getline(f, s))
	{
		const char *ptr = s.data();
		if(*ptr == '\0')
			break;
		NameType n;
		n.name = parseStringUntil(&ptr, ':');
		++ptr;
		int minV = parseNumber(&ptr);
		int maxV = parseNumber(&ptr);
		n.minMaxes.push_back({minV, maxV});
		ptr += 3;
		minV = parseNumber(&ptr);
		maxV = parseNumber(&ptr);
		n.minMaxes.push_back({minV, maxV});
		nameTypes.push_back(n);
	}
	getline(f, s);
	getline(f, s);
	std::vector<int> validNumbers(nameTypes.size(), (1 << nameTypes.size()) - 1);
	int64_t sum = 0;
	std::vector<int> myTicketNumbers = parseNumbers(s);
	removeInvalidNumbers(s, nameTypes, validNumbers);
	getline(f, s);
	getline(f, s);

	

	while(getline(f, s))
	{
		sum += removeInvalidNumbers(s, nameTypes, validNumbers); 
	}

	f.close();


	printf("Part a: Invalid sum: %" PRIi64 "\n", sum);

	// just keep removing indices that have multiple hits, until we have 1 hit for
	// everything
	bool stupidDelete = true;
	while(stupidDelete)
	{
		stupidDelete = false;
		for(int i = 0; i < validNumbers.size(); ++i)
		{
			int deleteValue = getFirstSetBit(validNumbers[i]);
			// Check that only one bit is set.
			if((validNumbers[i] - (1 << deleteValue)) != 0)
				continue;
			for(int j = 0; j < validNumbers.size(); ++j)
			{
				if(i == j)
					continue;
				int &a = validNumbers[j];
				stupidDelete |= (a >> deleteValue) & 1 == 1;
				a &= ~(1 << deleteValue);
			}
		}
	}

	int64_t multi = 1;

	for(int i = 0; i < validNumbers.size(); ++i)
	{
		int j = getFirstSetBit(validNumbers[i]);
		if(nameTypes[j].name.compare(0, 9, "departure") == 0)
			multi *= myTicketNumbers[i];
	}
	printf("Part b: Departure multiples: %" PRIi64 "\n", multi);
	return 0;
}

int main(int argc, char** argv)
{
	if(readValues("data.txt") == -1)
	{
		printf("Failed to load map!");
		return 0;
	};
	

	return 0;
}