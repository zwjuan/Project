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
		//1.���ļ���ͳ���ļ��ַ�����
		LongType count = 0;
		FILE* fout = fopen(filename,"rb");
		assert(fout);

		char ch = fgetc(fout);
		while(!feof(fout))  //feof()����Ƿ��ѵ��ļ�β���Ƿ����棬���򷵻�0
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}

		//2.���ɶ�Ӧ��huffman����
		GenerateHuffmanCode();

		//3.ѹ���ļ�
		string CompressFile = filename;
		CompressFile += ".huffman";

		FILE* fCompress = fopen(CompressFile.c_str(),"wb");
		assert(fCompress);

		//д���ַ����ֵ���Ϣ,Ŀ�Ľ�ѹ�����ѹ���ֿ�ִ��
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

		//��дһ��-1����Ϊ������־
		TmpInfo flag;
		flag._count = -1;
		fwrite(&flag,sizeof(TmpInfo),1,fCompress);

		fseek(fout,0,SEEK_SET);
		ch = fgetc(fout);

		int inch = 0; //д����ַ�
		int index = 0; //��¼�����˼�λ

		while(!feof(fout))  // EOF����
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

		//����8���ַ��Ĵ���������ѹʱ����¼���ж����ַ�����
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

	//��ѹ��
	bool UnCompress(const char* filename)
	{
		string UnCompressFile = filename; //��ѹ����ļ���UnCompressFile
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

		//�ؽ�Huffman��
		FileInfo invalid;
		HuffmanTree<FileInfo> hft(_infos,256,invalid); 

		HuffmanTreeNode<FileInfo>* root = hft.GetRootNode();

		LongType charcount = root->_weight ._count;//��ֵΪѹ���ļ��ַ��ĸ���
		
		//��һ���ַ��Ĵ���
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

			if(cur->_left == NULL && cur->_right == NULL)//curΪ���ڵ�
			{
				fputc(cur->_weight._ch ,fin);
				cur = root;
				if(--charcount == 0) //�ַ��Ѷ�ȡ�꣬������λ��0���ٶ�ȡ
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

	void _GenerateHuffmanCode(HuffmanTreeNode<FileInfo>* root, string code)//Ӧ��ֵ���ݣ����ֵ�øı䣬���´���Ӱ��
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
