#pragma once

#include<iostream>
#include<vector>
#include<assert.h>

using namespace std;

template<class T>
struct Less
{
	bool operator()(const T& l, const T& r)
	{
		return l < r; //重载operator<
	}
};

template<class T>
struct Great
{
	bool operator()(const T& l, const T& r)
	{
		return l > r;
	}
};

template<class T, class Compare=Less<T>>
class Heap
{
public:
	Heap()
	{}

	Heap(const T* arr, size_t size)
	{
		for(size_t i=0; i<size; i++)
		{
			_array.push_back (arr[i]);
		}

		//建堆
		for(size_t i=(_array.size()-2)/2; (int)i>=0; i--)
		{
			AdjustDown(i);
		}
	}

	void AdjustDown(size_t root)
	{
		size_t child = root*2+1;
		Compare com;
		while(child < _array.size())
		{
			/*if(child+1 <_array.size()&& _array[child] < _array[child+1])*/
			if(child+1 < _array.size() && com(_array[child+1],_array[child]))
			{
				child++;  //若child<child+1,直接++child,下一步直接比较child与parent大小
			}

			/*if(_array[child] > _array[root])*/
			if(com(_array[child],_array[root]))
			{
				swap(_array[child], _array[root]);
				root = child;
				child = root*2+1;
			}
			else
			{
				break;
			}
		}
	}

	void Push(const  T& x) //在尾上插入
	{
		_array.push_back (x);
		AdjustUp(_array.size()-1);
	}

	void AdjustUp(size_t child)
	{
		size_t parent = (child-1)/2;
		Compare com;
		while(child > 0)
		{
			if(com(_array[child],_array[parent]) && parent>=0)
			{
				swap(_array[parent],_array[child]);
				child = parent;
				parent = (child-1)/2;
			}
			else
			{
				break;
			}
		}
	}
	
	void Pop() 
	{
		assert(_array.size()>0);
		swap(_array[0],_array[_array.size()-1]);
		_array.pop_back ();

		AdjustDown(0);
	}

	T& Top()
	{
		assert(_array.size()>0);

		return _array[0];
	}

	bool Empty()
	{
		return _array.empty ();
	}

	size_t Size()
	{
		return _array.size();
	}

	void Print()
	{
		for(size_t i=0; i<_array.size(); i++)
		{
			cout<<_array[i]<<" ";
		}
		cout<<endl;
	}

private:
	vector<T> _array;
};
