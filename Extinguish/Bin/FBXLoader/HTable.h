#pragma once
#include "SLList.h"


template<typename Type>
class HTable
{
private:

	unsigned int (*HashFunk)(const Type&);
	unsigned int numOfB;
	SLList<Type>* Array;

public:
	/////////////////////////////////////////////////////////////////////////////
	// Function : Constructor 
	// Parameters : numOfBuckets - the number of buckets
	//              hFunction - pointer to the hash function for this table
	// Notes : constructs an empty hash table
	/////////////////////////////////////////////////////////////////////////////
	HTable(unsigned int numOfBuckets, unsigned int(*hFunction) (const Type &v))
	{
		numOfB = numOfBuckets;
		Array = new SLList<Type>[numOfBuckets];
		HashFunk = hFunction;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function : Destructor
	// Notes : destroys a hash table
	/////////////////////////////////////////////////////////////////////////////
	~HTable()
	{
		clear();
		delete[] Array;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function : Assignment Operator
	/////////////////////////////////////////////////////////////////////////////
	HTable<Type>& operator=(const HTable<Type>& that)
	{
		if (this != &that)
		{
			clear();
			delete[] Array;
			numOfB = that.numOfB;
			Array = new SLList<Type>[that.numOfB];
			for (unsigned int i = 0; i < numOfB; i++)
			{
				Array[i] = that.Array[i];
			}
			HashFunk = that.HashFunk;
		}
		return *this;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function : Copy Constructor
	/////////////////////////////////////////////////////////////////////////////
	HTable(const HTable<Type>& that)
	{
		numOfB = that.numOfB;
		Array = new SLList<Type>[that.numOfB];
		for (unsigned int i = 0; i < numOfB; i++)
		{
			Array[i] = that.Array[i];
		}
		HashFunk = &that.HashFunk;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function : insert
	// Parameters : v - the item to insert into the hash table
	/////////////////////////////////////////////////////////////////////////////
	void insert(const Type& v)
	{
		Array[HashFunk(v)].addHead(v);
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function : findAndRemove
	// Parameters : v - the item to remove(if it is found)
	/////////////////////////////////////////////////////////////////////////////
	bool findAndRemove(const Type& v)
	{
		int index = find(v);

		if (index == -1)
			return false;
		else
		{
			SLLIter<Type> iter(Array[index]);
			while (!iter.end())
			{
				if (iter.current() == v)
				{
					Array[index].remove(iter);
					return true;
				}
				++iter;
			}
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function : clear
	// Notes : clears the hash table
	/////////////////////////////////////////////////////////////////////////////
	void clear()
	{
		for (unsigned int i = 0; i < numOfB; ++i)
		{
			Array[i].clear();
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function : find
	// Parameters : v - the item to look for
	// Return : the bucket we found the item in or -1 if we didn’t find the item.
	/////////////////////////////////////////////////////////////////////////////
	int find(const Type& v) const
	{
		SLLIter<Type> iter(Array[HashFunk(v)]);
		for (; !iter.end(); ++iter)
		{
			if (iter.current() == v)
				return HashFunk(v);
		}
		return -1;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function : findReturn
	// Parameters : v - the item to look for
	// Return : if found is returned else null
	/////////////////////////////////////////////////////////////////////////////
	Type* findReturn(const Type& v) const
	{
		SLLIter<Type> iter(Array[HashFunk(v)]);
		for (; !iter.end(); ++iter)
		{
			if (iter.current() == v)
				return &iter.current();
		}
		return nullptr;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Function: retriveBucket
	// Parameters: b - bucket number to get
	// Return: the bucket single list
	/////////////////////////////////////////////////////////////////////////////
	const SLList<Type>* retriveBucket(unsigned int b) const
	{
		return &Array[b];
	}
};