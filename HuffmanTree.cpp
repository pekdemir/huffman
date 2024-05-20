#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "HuffmanTree.h"

HuffmanTree::AsciiList HuffmanTree::countChars(const std::string& input)
{
	std::array<Ascii, 256> ascii = {0};
	for(const char& c : input) {
		ascii[(int)c].c = c;
		ascii[(int)c].count++;
	}
	std::cout << "Chars in string are counted.\n";
	return ascii;
}

HuffmanTree::NodeVector HuffmanTree::createTreeList(const AsciiList ascii)
{
	NodeVector nodes;
	for(int i = 0; i < 256; i++){
		Ascii a = ascii[i];
		if(a.count > 0){
			Node* n{new Node};
			n->ec.c = a.c;
			n->ec.count = a.count;
			n->ec.enc = 0;
			n->ec.bitSize = 0;
			n->left = NULL;
			n->right = NULL;
			nodes.push_back(n);
		}
	}
	std::cout << "Tree list is created.\n";
	return nodes;
}

void HuffmanTree::generateHuffmanTree(NodeVector& nodes)
{
	while(nodes.size() > 1){
		sort(nodes.begin(), nodes.end(), [](Node* n1, Node* n2){return ((int)n1->ec.count > (int)n2->ec.count);});

		Node* n1 = nodes.back();
		nodes.pop_back();

		Node* n2 = nodes.back();
		nodes.pop_back();

		//cout << n1->ec.c << " " << n2->ec.c << endl;
		Node* n{new Node};
		n->ec.c = NULLCHAR;
		n->ec.count = n1->ec.count + n2->ec.count;
		n->ec.enc = 0;
		n->ec.bitSize = 0;
		n->left = n1;
		n->right = n2;

		nodes.push_back(n);
	}
	std::cout << "Huffman Tree generated from list.\n";
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

void HuffmanTree::traverse(NodeVector& node_list, Node* root, unsigned short encoding, unsigned int bitcount)
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
		traverse(node_list, root->left, encleft, bitcleft);
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
		traverse(node_list, root->right, encright, bitcright);
	}
}

void HuffmanTree::saveHuffmanTreeList(std::ostream& output, const NodeVector& nodes)
{
	//write tree size first
	unsigned int treesize = nodes.size();
	output.write(reinterpret_cast<char*>(&treesize), sizeof(treesize));

	std::cout << "Tree size: " << treesize << "\n";

	for(unsigned int i = 0; i < treesize; i++){
		Node* n = nodes[i];
		output.write(reinterpret_cast<char*>(&n->ec), sizeof(EncChar));
	}
	std::cout << "Huffman Tree list is written in file.\n";
}

HuffmanTree::NodeVector HuffmanTree::loadHuffmanTreeList(std::istream& input)
{
	unsigned int treesize;
	input.read(reinterpret_cast<char*>(&treesize), sizeof(treesize));
	
	NodeVector nodes;

	std::cout << "Tree size: " << treesize << "\n";

	for(unsigned int i = 0; i < treesize; i++){
		Node* n{new Node};
		input.read(reinterpret_cast<char*>(&n->ec), sizeof(EncChar));
		n->left = NULL;
		n->right = NULL;
		nodes.push_back(n);
	}

	std::cout << "Huffman Tree list is loaded." << "\n";
	return nodes;
}

void HuffmanTree::compress(std::filesystem::path& input, std::filesystem::path& output)
{
	std::ifstream input_stream(input);
	if(!input_stream) {
		std::cout << "Cant open input file: " << input << " \n";
		return;
	}

	std::stringstream input_buffer;
	input_buffer << input_stream.rdbuf();

	std::ofstream output_stream(output, std::ios::binary);
	if(!output_stream){
		std::cout << "Cant open compressed file: " << output << "\n";
		return;
	}

	std::string plaintext{input_buffer.str()};
	const auto ascii_list{countChars(plaintext)};
	auto nodes{createTreeList(ascii_list)};
	//save tree list
	saveHuffmanTreeList(output_stream, nodes);

	generateHuffmanTree(nodes);

	NodeVector node_list;
	Node* root{nodes.front()};
	traverse(node_list, root, 0, 0);
	std::cout << "Tree traverse is completed.\n";

	unsigned int text_size{plaintext.size()};
	plaintext += ENDCHAR;
	unsigned int encode;
	unsigned char bitcount;
	unsigned int ibuff = 0;
	int shift = 32;
	int remain = 0;



	//write character count
	output_stream.write(reinterpret_cast<char*>(&text_size), sizeof(text_size));
	std::cout << "Char count: " << text_size << "\n";

	for(unsigned int i = 0; i <= text_size; i++){
		char c = plaintext[i];
		for(auto it = node_list.begin(); it != node_list.end(); it++)
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

			output_stream.write(reinterpret_cast<char*>(&ibuff), sizeof(ibuff));

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

			output_stream.write(reinterpret_cast<char*>(&ibuff), sizeof(ibuff));


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
	output_stream.write(reinterpret_cast<char*>(&ibuff), sizeof(ibuff));
	std::cout << "Compression is completed!.\n";
}

void HuffmanTree::decompress(std::filesystem::path& input, std::filesystem::path& output)
{
	std::ifstream input_stream(input, std::ios::binary);
	if(!input_stream){
		std::cout << "Cant open input file: " << input << " \n";
		return;
	}

	std::ofstream output_stream(output);
	if(!output_stream){
		std::cout << "Cant open decompressed file: " << output << "\n";
		return;
	}

	int ibuff = 0;
	int temp = 0;
	int index = 0;
	int count = 32;
	unsigned int data_size;

	//load tree list from file
	auto nodes{loadHuffmanTreeList(input_stream)};
	//generate tree from list
	generateHuffmanTree(nodes);

	Node* root = nodes.front();
	Node* curr = root;

	//read character count
	input_stream.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));
	std::cout << "Char count: " << data_size << "\n";

	std::string str('\0', data_size);

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
			input_stream.read(reinterpret_cast<char*>(&ibuff), sizeof(ibuff));
			if(input_stream.eof())
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

	
	output_stream.write(str.c_str(), data_size);
	std::cout << "Decompression is completed!.\n";
}
