#include <cstring>
#include <iostream>
#include <filesystem>
#include "HuffmanTree.h"

int main(int argc, char *argv[]) {
    if(argc > 1) {
        std::string command(argv[1]);
        std::filesystem::path filename(argv[2]);
        std::filesystem::path compressedfilename(argv[3]);

        HuffmanTree ht = HuffmanTree();
        if(command == "-c")
            ht.compress(filename, compressedfilename);

        else if(command == "-d")
            ht.decompress(compressedfilename, filename);
    } else {
        std::cout << "Usage:\n";
        std::cout << "For compressing:\n$./huffman -c fromfilename compressedfilename\n";
        std::cout << "For decompressing:\n$huffman -d tofilename compressedfilename \n";
    }


    return 0;
}

