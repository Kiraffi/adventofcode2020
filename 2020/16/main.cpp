
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

struct NameType
{
	std::string name;
	std::vector<int> mins;
	std::vector<int> maxes;
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

// zeros don't add sum but are invalid...
bool checkValidNumbers(const std::string &s, const std::vector<NameType> &nameTypes, int64_t &outv)
{
	outv = 0;
	const char *ptr = s.data();
	bool valid = true;
	while(*ptr != '\0')
	{
		int number = parseNumber(&ptr);
		bool found = false;
		for(const auto &a : nameTypes)
		{
			for(int i = 0; i < a.mins.size(); ++i)
			{
				if(number >= a.mins[i] && number <= a.maxes[i])
				{
					found = true;
					break;
				}
			}
			if(found)
				break;
		}
		if(!found)
		{
			outv += number;
			valid = false;
		}
	}
	return valid;
}

void removeInvalidNumbers(const std::string &s, const std::vector<NameType> &nameTypes, 
	std::vector<std::vector<int>> &validNumbers)
{
	int numberIndex = 0;
	const char *ptr = s.data();
	while(*ptr != '\0')
	{
		int number = parseNumber(&ptr);
		bool found = false;
		std::vector<int> &p = validNumbers[numberIndex];
		for(int j = p.size() - 1; j >= 0; --j)
		{
			const NameType &a = nameTypes[p[j]];
			bool found = false;
			for(int i = 0; i < a.mins.size(); ++i)
			{
				if(number >= a.mins[i] && number <= a.maxes[i])
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				assert(p.size() > 1);
				p.erase(p.begin() + j);
			}
		}
		++numberIndex;
	}
}




std::vector<int> generateNumbers(int upTo)
{
	std::vector<int> v(upTo, 0);
	for(int i = 0; i < upTo; ++i)
		v[i] = i;
	return v;
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
		n.mins.push_back(parseNumber(&ptr));
		n.maxes.push_back(parseNumber(&ptr));
		ptr += 3;
		n.mins.push_back(parseNumber(&ptr));
		n.maxes.push_back(parseNumber(&ptr));
		nameTypes.push_back(n);
	}
	getline(f, s);
	getline(f, s);
	std::vector<std::vector<int>> validNumbers(nameTypes.size(), generateNumbers(nameTypes.size()));
	int64_t sum = 0;
	std::vector<int> myTicketNumbers = parseNumbers(s);
	checkValidNumbers(s, nameTypes, sum);
	removeInvalidNumbers(s, nameTypes, validNumbers);
	getline(f, s);
	getline(f, s);

	

	while(getline(f, s))
	{
		int64_t v = 0; 
		if(checkValidNumbers(s, nameTypes, v))
		{
			removeInvalidNumbers(s, nameTypes, validNumbers);
		}
		sum += v;
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
			if(validNumbers[i].size() != 1)
				continue;
			int deleteValue = validNumbers[i][0];
			for(int j = 0; j < validNumbers.size(); ++j)
			{
				if(i == j)
					continue;
				std::vector<int> &a = validNumbers[j]; 
				for(int k = a.size() - 1; k >= 0; --k)
				{
					if(a[k] == deleteValue)
					{
						stupidDelete = true;
						a.erase(a.begin() + k);
					}
				}
			}
		}
	}

	int64_t multi = 1;

	for(int i = 0; i < validNumbers.size(); ++i)
	{
		int j = validNumbers[i][0];
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