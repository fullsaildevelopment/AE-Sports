#pragma once
#include <map>
#include <string>

class HashStringCopy
{
private:
	//static HashString* singleton;
	std::map<std::string, unsigned int> hashTable;
	int index = 0;

public:
	//~HashString()
	//{
	//	delete singleton;
	//	singleton = nullptr;
	//}

	//static HashString* GetSingleton() { if (!singleton) { singleton = new HashString(); } return singleton; }
	int GetKey(std::string key);
	void Insert(std::string key) { hashTable[key] = index++; }
	//void Insert(std::string key, unsigned int theirIndex) { hashTable[key] = theirIndex; }
	//void CleanUp() { delete singleton; singleton = nullptr; }
};