#pragma once

#include <vector>
#include "Types.h"
using namespace std;

class HuffmanTree
{
    char *plaintext;
    vector<Node*> nodes,node_list;
    Ascii ascii[256];
    Node* root;
    unsigned int treelistsize;

public:
    HuffmanTree();

    void countChars();
    void createTreeList();
    void generateHuffmanTree();
    void printbin(unsigned int n, unsigned int s);
    void traverse(Node* root,unsigned short encoding,unsigned int bitcount);
    void saveHuffmanTreeList(FILE *fp);
    void loadHuffmanTreeList(FILE *fp);
    void compress(char* filename,char* compress_file_name);
    void decompress(char* compress_file_name,char* filename);
    void loadString(char* filename);
};

