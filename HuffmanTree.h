#pragma once

#include <vector>
#include <filesystem>
#include "Types.h"

class HuffmanTree
{
    using AsciiList = std::array<Ascii, 256>;
    using NodeVector = std::vector<Node*>;

    AsciiList countChars(const std::string& input);
    NodeVector createTreeList(const AsciiList ascii);
    void generateHuffmanTree(NodeVector& nodes);
    void traverse(NodeVector& node_list, Node* root,unsigned short encoding,unsigned int bitcount);
    void saveHuffmanTreeList(std::ostream& output, const NodeVector& nodes);
    NodeVector loadHuffmanTreeList(std::istream& input);
    void printbin(unsigned int n, unsigned int s);

public:
    void compress(std::filesystem::path& input, std::filesystem::path& output);
    void decompress(std::filesystem::path& input, std::filesystem::path& output);
};

