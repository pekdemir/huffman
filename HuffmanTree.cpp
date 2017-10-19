/*
* HuffmanTree.cpp
*
*  Created on: 08.Haz.2015
*      Author: asus
*/

#include "HuffmanTree.h"


bool comperator(Node* n1, Node* n2)
{
    return ((int)n1->ec.count > (int)n2->ec.count);
}

HuffmanTree::HuffmanTree()
{
	memset(ascii,0,sizeof(Ascii)*256);
}

void HuffmanTree::countChars()
{
	for(unsigned int i=0;i<strlen(plaintext);i++)
	{
		char c = plaintext[i];
		ascii[(int)c].c = c;
		ascii[(int)c].count++;
	}
	cout << "Chars in string counted." << endl;
}

void HuffmanTree::createTreeList()
{
	for(int i=0;i<256;i++)
	{
		Ascii a = ascii[i];
		if(a.count > 0){
			Node* n;
			n = (Node*)malloc(sizeof(Node));
			n->ec.c = a.c;
			n->ec.count = a.count;
			n->ec.enc = 0;
			n->ec.bitSize = 0;
			n->left = NULL;
			n->right = NULL;
			nodes.push_back(n);
		}
	}
	treelistsize = nodes.size();
	cout << "Tree list created." << endl;
}

void HuffmanTree::generateHuffmanTree()
{
	while(nodes.size() > 1)
	{
		sort(nodes.begin(),nodes.end(),comperator);

		Node* n1 = nodes.back();
		nodes.pop_back();

		Node* n2 = nodes.back();
		nodes.pop_back();

		//cout << n1->ec.c << " " << n2->ec.c << endl;
		Node* n;
		n = (Node*)malloc(sizeof(Node));
		n->ec.c = NULLCHAR;
		n->ec.count = n1->ec.count + n2->ec.count;
		n->ec.enc = 0;
		n->ec.bitSize = 0;
		n->left = n1;
		n->right = n2;

		nodes.push_back(n);
	}
	cout << "Huffman Tree generated from list." << endl;
}
void HuffmanTree::printbin(unsigned int n, unsigned int s)
{
	unsigned int num = 1 << (s-1);
	while(num)
	{
		if(n & num)
			printf("1");
		else
			printf("0");
		num >>= 1;
	}
}

void HuffmanTree::traverse(Node* root,unsigned short encoding,unsigned int bitcount)
{
	if(root->ec.c != NULLCHAR)
	{
		node_list.push_back(root);
	}
	if(root->left == NULL && root->right == NULL)
	{
		//leaf
		root->ec.enc = encoding;
		root->ec.bitSize = bitcount;

		//printf("Leaf %c,%x,%d\n",root->ec.c,encoding,bitcount);

		return;
	}
	int encleft = encoding;
	int bitcleft = bitcount;

	int encright = encoding;
	int bitcright = bitcount;
	if(root->left != NULL)
	{
		/*if(bitcount>=8)
		{
			root->ec.enc = root->ec.c;
			root->ec.bitSize = 8;
			return;
		}*/

		encleft <<= 1;
		//left 0
		//encoding |= 1;
		bitcleft++;
		traverse(root->left,encleft,bitcleft);
	}
	if(root->right != NULL)
	{
		/*if(bitcount>=8)
		{
			root->ec.enc = root->ec.c;
			root->ec.bitSize = 8;
			return;
		}*/
		encright <<= 1;
		//right 1
		encright |= 1;
		bitcright++;
		traverse(root->right,encright,bitcright);
	}
}

void HuffmanTree::saveHuffmanTreeList(FILE *fp)
{
	//write tree size first
	unsigned int treesize = nodes.size();
	fwrite(&treesize,sizeof(unsigned int),1,fp);

	cout << "Treesize: " << treesize << endl;

	for(unsigned int i=0;i<treesize;i++)
	{
		Node* n = nodes[i];
		fwrite(&n->ec,sizeof(EncChar),1,fp);
	}
	cout << "Huffman Tree list wrote in file." << endl;
}

void HuffmanTree::loadHuffmanTreeList(FILE *fp)
{
	unsigned int treesize;
	fread(&treesize,sizeof(unsigned int),1,fp);
	nodes.clear();

	cout << "Treesize: " << treesize << endl;

	for(unsigned int i=0;i<treesize;i++)
	{
		Node* n;
		n = (Node*)malloc(sizeof(Node));
		fread(&n->ec,sizeof(EncChar),1,fp);
		n->left = NULL;
		n->right = NULL;
		nodes.push_back(n);
	}

	cout << "Huffman Tree list loaded." << endl;
}

void HuffmanTree::compress(char* filename,char* compress_file_name)
{
	loadString(filename);

	FILE *fp;
	fp = fopen(compress_file_name,"wb");
	if(!fp)
	{
		cout << "Cant open compressed file." << endl;
		return;
	}

	countChars();
	createTreeList();
	//save tree list
	saveHuffmanTreeList(fp);

	generateHuffmanTree();

	root = nodes.front();
	traverse(root,0,0);
	cout << "Tree traverse completed." << endl;

	unsigned int size = strlen(plaintext);
	plaintext[size] = ENDCHAR;
	unsigned int encode;
	unsigned char bitcount;
	unsigned int ibuff = 0;
	int shift = 32;
	int remain = 0;



	//write character count
	fwrite(&size,sizeof(unsigned int),1,fp);
	cout << "Char count: " << size << endl;

	for(unsigned int i=0;i<=size;i++)
	{
		char c = plaintext[i];
		vector<Node*>::iterator it;
		for(it=node_list.begin();it!=node_list.end();it++)
		{
			//printf("%c %d\n",(*it)->ec.c,(*it)->ec.count);
			if((*it)->ec.c == c)
			{
				encode = (*it)->ec.enc;
				bitcount = (*it)->ec.bitSize;
			}
		}
		if(shift == 0)
		{

			fwrite(&ibuff,sizeof(unsigned int),1,fp);

			ibuff = 0;
			shift = 32 - bitcount;
			ibuff |= encode;
			ibuff <<= shift;

			/*printf("tam %c",c,ibuff);
			printbin(ibuff,32);
			printf("\n");*/
		}
		else if(shift<bitcount)
		{
			remain = bitcount - shift;
			ibuff |= encode >> remain;

			/*printf("son %c",c,ibuff);
			printbin(ibuff,32);
			printf("\n");*/

			fwrite(&ibuff,sizeof(unsigned int),1,fp);


			ibuff = 0;
			shift = 32 - remain;
			ibuff |= encode;
			ibuff <<= shift;

			/*printf("yeni %c,",c,ibuff);
			printbin(ibuff,32);
			printf("\n");*/
		}else{
			shift -=bitcount;
			ibuff |= encode << shift;

			/*printf("%c,",c,ibuff);
			printbin(ibuff,32);
			printf("\n");*/
		}

	}
	fwrite(&ibuff,sizeof(unsigned int),1,fp);
	fclose(fp);
	cout << "Compression completed!." << endl;
}

void HuffmanTree::decompress(char* compress_file_name,char* filename)
{
	FILE* fp;
	fp = fopen(compress_file_name,"rb");
	if(!fp)
	{
		cout << "Cant open compressed file." << endl;
		return;
	}
	//bool devam = true;

	int ibuff = 0;
	int temp = 0;
	int index = 0;
	int count = 32;
	unsigned int size;

	//load tree list from file
	loadHuffmanTreeList(fp);
	//generate tree from list
	generateHuffmanTree();

	root = nodes.front();
	Node* curr = root;

	//read character count
	fread(&size,sizeof(unsigned int),1,fp);
	cout << "Char count: " << size << endl;

	char* str;
	str = (char*)malloc(sizeof(char)*size);

	memset(str,0,sizeof(char)*(size));

	while(true)
	{
		if(curr->left == NULL && curr->right == NULL)
		{
			if(curr->ec.c == ENDCHAR)
				break;
			str[index++] = curr->ec.c;
			curr = root;
		}

		if(count == 32)
		{
			int ret = fread(&ibuff,sizeof(unsigned int),1,fp);
			if(!ret)
				break;
			/*printf("ibuff:");
			printbin(ibuff,32);
			printf("\n");*/

			temp = ibuff;
			count = 0;
		}

		if(temp & (1<<31))
			curr = curr->right;
		else
			curr = curr->left;

		temp <<= 1;
		count++;
	}
	fclose(fp);
	FILE *fpout;
	fpout = fopen(filename,"wb");
	if(!fpout)
	{
		cout << "Cant open output file." << endl;
		return;
	}
	fwrite(str,size,1,fp);
	fclose(fpout);
	cout << "Decompression completed!." << endl;
}
void HuffmanTree::loadString(char* filename)
{
	FILE *fp;
	fp = fopen(filename,"rb");
	if(!fp)
	{
		cout << "Cant open input file." << endl;
		return;
	}
	fseek(fp,0,SEEK_END);
	unsigned int len = ftell(fp);
	rewind(fp);
	plaintext = (char*)malloc(sizeof(char)*len);
	fread(plaintext,len,1,fp);
	fclose(fp);
	//cout << plaintext <<endl;
	cout << "File loaded!." << endl;
}

