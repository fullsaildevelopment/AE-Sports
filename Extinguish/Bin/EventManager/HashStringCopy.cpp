#include "HashStringCopy.h"

//HashString* HashString::singleton = 0;

int HashStringCopy::GetKey(std::string key)
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