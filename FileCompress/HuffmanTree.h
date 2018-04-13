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

		/*���ﴫ�ݵ�����ӦΪNode* ,���Խ�������������������������ʱ��
		����֪�������������Һ��ӷֱ���˭*/
		Heap<Node* ,NodeCompare<T>> minHeap;//����С��

		//����
		for(size_t i=0; i<size; i++)
		{
			if(arr[i] != invalid)  //256���ַ�������ȫ����Ч��ȥ����Ч��
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
			minHeap.Push(parent);//�����ڵ��������У��ٴӶ�����ѡ����С�Ľ��

		}
		_root = minHeap.Top();
	}

	 Node* GetRootNode() //��const��
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