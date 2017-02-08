#include "HashString.h"

//returns -1 if the result is neglible
int HashString::GetKey(std::string key)
{ 
	int result = -1;

	//key += '\0';

	auto stuff = hashTable.find(key);

	if (stuff != hashTable.end())
	{ 
		result =  hashTable[key];
	}

	return result;
}