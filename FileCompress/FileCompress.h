#pragma once

#include"HuffmanTree.h"
#include<string>
typedef long long LongType;

struct FileInfo
{
	unsigned char _ch;
	LongType _count;
	string _code;


	FileInfo(const char& ch = 0)
		:_ch(ch)
		,_count(0)
	{}

	bool operator != (const FileInfo&  fi) const
	{
		return this->_count != fi._count;  
	}

	FileInfo operator+(FileInfo& fi)
	{
		LongType count = _count + fi._count ;
		FileInfo tmp;
		tmp._count = count;
		return tmp;
	}
};

class FileCompress
{
	struct TmpInfo
	{
		unsigned char _ch;
		LongType _count;
	};
public:
	FileCompress()
	{
		for(int i=0; i<256; i++)
		{
			_infos[i]._ch = i;  
		}
	}

	bool Compress(const char* filename)
	{
		assert("filename");
		//1.打开文件，统计文件字符个数
		LongType count = 0;
		FILE* fout = fopen(filename,"rb");
		assert(fout);

		char ch = fgetc(fout);
		while(!feof(fout))  //feof()检测是否已到文件尾，是返回真，否则返回0
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}

		//2.生成对应的huffman编码
		GenerateHuffmanCode();

		//3.压缩文件
		string CompressFile = filename;
		CompressFile += ".huffman";

		FILE* fCompress = fopen(CompressFile.c_str(),"wb");
		assert(fCompress);

		//写入字符出现的信息,目的将压缩与解压缩分开执行
		for(int i=0; i<256; i++)
		{
			if(_infos[i]._count > 0)
			{
				TmpInfo info;
				info._ch = _infos[i]._ch;
				info._count = _infos[i]._count;
				fwrite(&info,sizeof(TmpInfo),1,fCompress);
			}
		}

		//多写一个-1，作为结束标志
		TmpInfo flag;
		flag._count = -1;
		fwrite(&flag,sizeof(TmpInfo),1,fCompress);

		fseek(fout,0,SEEK_SET);
		ch = fgetc(fout);

		int inch = 0; //写入的字符
		int index = 0; //记录读到了几位

		while(!feof(fout))  // EOF区别
		{
			string& code = _infos[(unsigned char)ch]._code ; 
			for(size_t i=0; i<code.size(); i++)
			{
				inch = inch << 1;
				if(code[i] == '1')
				{
					inch |= 1;
				}

				if(++index == 8)
				{
					fputc(inch,fCompress);
					inch = 0;
					index = 0;
				}
			}
			ch = fgetc(fout);
		}

		//不够8个字符的处理，后处理（解压时，记录共有多少字符串）
		if(index != 0)
		{
			inch = inch <<(8 - index);
			fputc(inch ,fCompress );
		}

		fclose(fCompress);
		fclose(fout);

		fclose(fCompress);
		fclose(fout);

		return true;
	}

	//解压缩
	bool UnCompress(const char* filename)
	{
		string UnCompressFile = filename; //解压后的文件名UnCompressFile
		size_t pos = UnCompressFile.rfind('.');
		assert(pos != string::npos); 
		UnCompressFile.erase(pos);
		UnCompressFile +=".uncompress";

		FILE* fin = fopen(UnCompressFile.c_str() ,"wb");
		assert(fin);

		FILE* fout = fopen(filename ,"rb");
		assert(fout);

		TmpInfo info;
		fread(&info,sizeof(TmpInfo),1,fout);
		while(info._count != -1)
		{
			_infos[(unsigned char)info._ch]._ch = info._ch;
			_infos[(unsigned char)info._ch]._count = info._count;
			fread(&info,sizeof(TmpInfo),1,fout);
		}

		//重建Huffman树
		FileInfo invalid;
		HuffmanTree<FileInfo> hft(_infos,256,invalid); 

		HuffmanTreeNode<FileInfo>* root = hft.GetRootNode();

		LongType charcount = root->_weight ._count;//此值为压缩文件字符的个数
		
		//对一个字符的处理
		if((root->_left == NULL)&& (root->_right == NULL))
		{
			while(charcount-- != 0)
			{
				fputc(root->_weight._ch ,fin);
			}
			return true;
		}

		HuffmanTreeNode<FileInfo>* cur = root;
		char ch = fgetc(fout);
		pos = 8;

		while(!feof(fout)) //!feof(fout)
		{
			--pos;
			if(((unsigned char)ch & (1<<pos)) == 0) 
			{
				cur = cur->_left ;
			}
			else
			{
				cur = cur->_right ;
			}

			if(cur->_left == NULL && cur->_right == NULL)//cur为根节点
			{
				fputc(cur->_weight._ch ,fin);
				cur = root;
				if(--charcount == 0) //字符已读取完，遇到补位的0不再读取
				{
					break;
				}
			}

			if(pos == 0)
			{
				ch = fgetc(fout);
				pos = 8;
			}		
		}

		fclose(fin);
		fclose(fout);

		return true;

	}


protected :
	void GenerateHuffmanCode()
	{
		FileInfo invalid;
		HuffmanTree<FileInfo> hft(_infos,256,invalid);
		string code;
		_GenerateHuffmanCode(hft.GetRootNode(),code);
	}

	void _GenerateHuffmanCode(HuffmanTreeNode<FileInfo>* root, string code)//应该值传递，这次值得改变，对下次无影响
	{ 
		if(root == NULL)
		{
			return ;
		} 
		
		if(root->_left == NULL && root->_right == NULL)
		{
			_infos[(unsigned char)root->_weight._ch ]._code  = code;
		}

		_GenerateHuffmanCode(root->_left,code+'0');
		_GenerateHuffmanCode(root->_right,code+'1');
	}

protected: 
	FileInfo _infos[256];
};

void TestFileCompress()
{
	FileCompress fc;

	fc.Compress("1.jpg");

	fc.UnCompress ("1.jpg.huffman");
}
