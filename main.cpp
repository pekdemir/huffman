#include <cstring>
#include <iostream>
#include "HuffmanTree.h"

int main(int argc,char *argv[])
{
    char *command,*filename,*compressfilename;
    if(argc>1)
    {
        command = argv[1];
        filename = argv[2];
        compressfilename = argv[3];
    }else
    {
    	cout << "Usage: " << endl;
    	cout << "For compressing:\n$huffmancoding -c fromfilename compressedfilename\n" << endl;
    	cout << "For decompressing:\n$huffmancoding -d tofilename compressedfilename \n" << endl;
    }

    HuffmanTree ht = HuffmanTree();
    if(!strcmp(command,"-c"))
        ht.compress(filename,compressfilename);

    if(!strcmp(command,"-d"))
        ht.decompress(compressfilename,filename);
    return 0;
}

