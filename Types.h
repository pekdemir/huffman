#ifndef TYPES_H_
#define TYPES_H_

#define ENDCHAR 0x03 //end of text
#define NULLCHAR 0x00

typedef struct _ascii
{
    char c;
    unsigned int count;
}Ascii;

typedef struct _encChar
{
    char c;
    unsigned int count;
    unsigned short enc;
    unsigned char bitSize;
}EncChar;

typedef struct _node
{
    EncChar ec;
    _node* left;
    _node* right;
}Node;

#endif /* TYPES_H_ */
