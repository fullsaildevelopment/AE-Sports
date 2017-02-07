#pragma once
#include <map>
#include <string>

class HashString
{
private:
	std::map<std::string, unsigned int> hashTable;
	int index = 0;

public:
	int GetKey(std::string key);
	void Insert(std::string key) { hashTable[key] = index++; }
	//void Insert(std::string key, unsigned int theirIndex) { hashTable[key] = theirIndex; }
};