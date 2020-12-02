
#include <algorithm>
#include <fstream>
#include <inttypes.h> // prix64
#include <stdio.h>
#include <vector>
#include <assert.h>

bool readValues(const char *filename, std::vector<int> &outValues);


struct MachineState
{
	std::vector<int> memory;
	int *functionParameterPointers[4];
	int instructionPointer;
	int input;
};


using InstructionFunction = bool(*)(MachineState &state);

struct Instruction
{
	int opcode;
	int parameters;
	InstructionFunction func;
};



bool opcodeAdd(MachineState &state)
{
	*state.functionParameterPointers[2] = *state.functionParameterPointers[0] + *state.functionParameterPointers[1];
	return true;
}

bool opcodeMul(MachineState &state)
{
	*state.functionParameterPointers[2] = *state.functionParameterPointers[0] * *state.functionParameterPointers[1];
	return true;
}

bool opcodeInput(MachineState &state)
{
	printf("input: %i\n", *state.functionParameterPointers[0]);
	*state.functionParameterPointers[0] = state.input;
	return true;
}

bool opcodeOutput(MachineState &state)
{
	printf("output: %i\n", *state.functionParameterPointers[0]);
	return true;
}

bool opcodeJumpIfTrue(MachineState &state)
{
	if(*state.functionParameterPointers[0] != 0)
	{
		state.instructionPointer = *state.functionParameterPointers[1];
	}
	return true;
}
bool opcodeJumpIfFalse(MachineState &state)
{
	if(*state.functionParameterPointers[0] == 0)
	{
		state.instructionPointer = *state.functionParameterPointers[1];
	}
	return true;
}
bool opcodeLessThan(MachineState &state)
{
	int storeValue = *state.functionParameterPointers[0] < *state.functionParameterPointers[1] ? 1 : 0;  
	*state.functionParameterPointers[2] = storeValue;

	return true;
}
bool opcodeEquals(MachineState &state)
{
	int storeValue = *state.functionParameterPointers[0] == *state.functionParameterPointers[1] ? 1 : 0;  
	*state.functionParameterPointers[2] = storeValue;

	return true;
}


bool opcodeHalt(MachineState &state)
{
	printf("Halted\n");
	printf("at 0: %i\n", state.memory[0]);
	if(state.memory[0] == 19690720)
	{
		printf("1: %i, 2: %i\n", state.memory[1], state.memory[2]);
		return true;
	}
	return false;
}




Instruction instructions[] = 
{
	Instruction{1, 3, opcodeAdd},
	Instruction{2, 3, opcodeMul},
	Instruction{3, 1, opcodeInput},
	Instruction{4, 1, opcodeOutput},
	Instruction{5, 2, opcodeJumpIfTrue},
	Instruction{6, 2, opcodeJumpIfFalse},
	Instruction{7, 3, opcodeLessThan},
	Instruction{8, 3, opcodeEquals},
	Instruction{99, 0, opcodeHalt},
};



bool parseValue(const std::vector<int> &memory, int pointer, bool immediateMode, int &outValue)
{
	if(pointer >= memory.size())
	{
		printf("Pointer out of range for parsing values. \n");
		return false;
	}
	int v = memory[pointer];
	if(!immediateMode)
	{
		if(v >= memory.size())
		{
			printf("Pointer out of range for parsing values. \n");
			return false;
		}
		v = memory[v];
	}
	outValue = v;
	return true;
}



void printValues(const std::vector<int> &memory)
{
	for(int i = 0; i < memory.size(); ++i)
	{
		if( i + 1 < memory.size())
			printf("%i,", memory[i]);
		else 
			printf("%i", memory[i]);
	}
	printf("\n");

}

// pass by copy
bool program(MachineState state, int replaceFirst, int replaceSecond)
{
	//state.memory[1] = replaceFirst;
	//state.memory[2] = replaceSecond;

	std::vector<bool> immediates;
	state.instructionPointer = 0;
	int &pointer = state.instructionPointer;
	while(pointer < state.memory.size())
	{
		int v = state.memory[pointer];
		++pointer;
		int opcode = v % 100;

		bool foundOpCode = false;

		for(int i = 0; i < sizeof(instructions) / sizeof(Instruction); ++i)
		{
			if(opcode == instructions[i].opcode)
			{
				if(pointer + instructions[i].parameters > state.memory.size())
				{
					printf("Pointer will be out of range for parsing memory. \n");
					return false;
				}
				int v2 = v / 100;
				for(int param = 0; param <  instructions[i].parameters; ++param)
				{
					bool immediate = (v2 % 10) == 1;
					int *tmp = &state.memory[pointer + param];
					state.functionParameterPointers[param] = immediate ? tmp : &state.memory[*tmp];
					v2 /= 10;
				}
				//printf("Pointer: %i \n", pointer - 1);
				pointer += instructions[i].parameters;
				bool found = instructions[i].func(state);

				if(opcode == 99)
					return found;

				foundOpCode = true;
			}
		}
		if(!foundOpCode)
		{
			assert(false && "Unknown op code!");
		}
	}

	printf("Trying to read op codes out of array: pointer: %i vs size: %i\n",
		pointer, int(state.memory.size()));
	return false;
}

int main(int argc, char** argv)
{
	std::vector<int> values;
	if(!readValues("data.txt", values))
	{
		return 0;
	}

	if(values.size() < 1)
	{
		printf("Data should have at least 1 number!\n");
		return 0;
	}

	program(MachineState{ values, {}, 0, 5} , 225, 1);
/*
	program(values, 12, 2);

	for(int i = 0; i < 100; ++i)
	{
		for(int j = 0; j < 100; ++j)
		{
			if(program(values, i, j))
			{
				return 0;	
			}
		}

	}
*/

	return 0;
}







bool readValues(const char *filename, std::vector<int> &outValues)
{
	std::ifstream f (filename);
	if (!f.is_open())
	{
		printf("Failed to open file: %s\n", filename);
		return false;
	}
	std::string s;
	while(getline(f, s))
	{
		int v = 0;
		bool neg = false;
		for(int i = 0; i < s.length(); ++i)
		{
			char c = s[i];
			if(c >= '0' && c <= '9')
			{
				v = v * 10 + (c - '0');
			}
			if(c == '-')
			{
				neg = true;
			}
			if(c == ',' || i == s.length() - 1)
			{
				if(neg)
					v = -v;
				outValues.emplace_back(v);
				v = 0;
				neg = false;
			}
			
		}
	}
	f.close();
	
	return true;
}

