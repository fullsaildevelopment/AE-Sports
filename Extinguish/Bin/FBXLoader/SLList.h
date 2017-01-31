#pragma once

template<typename Type = int> 
class SLLIter;
template<typename Type = int>
class SLList
{
	friend class SLLIter<Type>;

private:
	struct node 
	{
		Type data;
		node* next = nullptr;

		node() {}
		node(Type _data, node* _next) : data(_data), next(_next) {}
	};
	node* head;
	int Size;
public:
	SLList() { head = nullptr; Size = 0; }
	~SLList() { clear(); Size = 0; }
	SLList<Type>& operator=(const SLList<Type>& that);
	SLList(const SLList<Type>& that);
	void addHead(const Type& _data);
	void clear();
	void insert(SLLIter<Type>& index, const Type& v);
	void remove(SLLIter<Type>& index);
	int size() const { return Size; }
};

template<typename Type>
SLList<Type>& SLList<Type>::operator=(const SLList<Type>& that)
{
	if (this != &that && that.head != nullptr)
	{
		clear();
		node* tmp2 = that.head;
		addHead(tmp2->data);
		node* tmp1 = head;
		while (tmp2->next)
		{
			tmp2 = tmp2->next;
			tmp1->next = new node(tmp2->data, nullptr);
			tmp1 = tmp1->next;
		}
		Size = that.Size;
	}
	else if (that.head == nullptr)
		clear();
	return *this;
}

template<typename Type>
SLList<Type>::SLList(const SLList<Type>& that)
{
	node* tmp2 = that.head;
	addHead(tmp2->data);
	node* tmp1 = head;
	
	while (tmp2->next)
	{
		tmp1->next = new node(tmp2->data, nullptr);
		tmp2 = tmp2->next;
		tmp1 = tmp1->next;
	}
	Size = that.Size;

}


template<typename Type>
void SLList<Type>::addHead(const Type& _data)
{
	//head = new node(_data, head);
	node* tmp = new node;
	tmp->data = _data;
	tmp->next = head;
	head = tmp;
	++Size;
}
template<typename Type>
void SLList<Type>::clear()
{
	node* tmp = head;
	while (head)
	{
		head = head->next;
		delete tmp;
		tmp = head;
	}
	Size = 0;
}

template<typename Type>
void SLList<Type>::insert(SLLIter<Type>& index, const Type& v)
{
	if (Size == 0)
		return;
	node* tmp = head;
	node* tmp2 = head;
	if (index.looker == head)
	{
		addHead(v);
		index.headList = head;
		index.looker = head;
	}
	else
	{
		for (int i = 0; i < Size; i++)
		{
			if (tmp == index.looker)
				break;
			else
			{
				tmp2 = tmp;
				tmp = tmp->next;
			}
		}
		tmp2->next = new node(v, tmp2->next);
		index.looker = tmp2->next;

		++Size;
	}
}

template<typename Type>
void SLList<Type>::remove(SLLIter<Type>& index)
{
	node* tmp = head;
	node* tmp2 = head;
	if (index.looker == nullptr)
		return;
	if (index.looker == head)
	{
		head = head->next;
		index.looker = head;
		index.headList = head;
		delete tmp;
		--Size;
	}
	else
	{
		for (int i = 0; i < Size - 1; i++)
		{
			if (tmp == index.looker)
				break;
			else
			{
				tmp2 = tmp;
				tmp = tmp->next;
			}
		}
		tmp2->next = tmp->next;
		delete tmp;
		--Size;
	}
}


template<typename Type>
class SLLIter
{
	friend class SLList<Type>;
private:
	typename SLList<Type>::node* looker;
	typename SLList<Type>::node* headList;
	SLList<Type>* list;
public:
	SLLIter(SLList<Type>& listToIterate);
	void begin();
	bool end() const;
	SLLIter<Type>& operator++();
	Type& current() const;
};

template<typename Type>
SLLIter<Type>::SLLIter(SLList<Type>& listToIterate)
{
	list = &listToIterate;
	looker = listToIterate.head;
	headList = listToIterate.head;
}

template<typename Type>
void SLLIter<Type>::begin()
{
	looker = list->head;
}

template<typename Type>
bool SLLIter<Type>::end() const 
{
	bool isEnd = true;
	if(looker == nullptr)
		return isEnd;
	else return !isEnd;
}

template<typename Type>
SLLIter<Type>& SLLIter<Type>::operator++()
{
	if(looker != nullptr)
		looker = looker->next;
	return *this;
}

template<typename Type>
Type& SLLIter<Type>::current() const
{
	return looker->data;
}