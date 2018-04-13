#pragma once 

#include<iostream>
#include "Heap.h"

using namespace std;

template<class T>
struct HuffmanTreeNode
{
	T _weight;
	HuffmanTreeNode* _left;
	HuffmanTreeNode* _right;

	HuffmanTreeNode (const T& w)
		:_weight(w)
		,_left(NULL)
		,_right(NULL)
	{}
};

template<class T>
class HuffmanTree
{
public:
	typedef HuffmanTreeNode<T>  Node;
	HuffmanTree()
		:_root(NULL)
	{}

	template<class T>
	struct NodeCompare
	{
		bool operator()(Node* l, Node* r)
		{
			return l->_weight._count < r->_weight._count;
		}
	};

	HuffmanTree(const T* arr, size_t size, const T& invalid)
	{
		assert(arr);

		/*堆里传递的类型应为Node* ,可以将整棵树连接起来，若传整数时，
		不会知道它的左孩子与右孩子分别是谁*/
		Heap<Node* ,NodeCompare<T>> minHeap;//生成小堆

		//建堆
		for(size_t i=0; i<size; i++)
		{
			if(arr[i] != invalid)  //256个字符不可能全部有效，去掉无效的
			{
				Node* node = new Node(arr[i]);
				minHeap.Push (node);
			}
		}

		while(minHeap.Size() > 1)
		{
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node* right = minHeap.Top();
			minHeap.Pop();

			Node* parent = new Node(left->_weight + right->_weight); 
			parent->_left = left;
			parent->_right = right;
			minHeap.Push(parent);//将父节点插入堆列中，再从堆列中选择最小的结点

		}
		_root = minHeap.Top();
	}

	 Node* GetRootNode() //加const？
	{
		return _root;
	}

	~HuffmanTree()
	{
		cout<<"~HuffmanTree()"<<endl;
		Destory(GetRootNode ());
	}

	void Destory( Node* root)
	{
		if(root != NULL)
		{
			Destory(root->_left);
			Destory(root->_right);
			delete root;
			root = NULL;
		}
	}

private:
	Node* _root;
};

//void TestHuffmanTree()
//{
//	int arr[4] = {1,2,3,4};
//	HuffmanTree<int> ht(arr,4);
//}