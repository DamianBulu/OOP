#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "node.h"
#include "code.h"
#include <windows.h>  // For Windows file operations
#include <string>
 
struct Header {
    uint32_t signature;
    uint32_t tree_size;
    uint64_t file_size;
};

class Huffman {
public:
    static Node* build_tree(uint64_t histogram[ALPHABET]);
    static void build_codes(Node* root, Code* codes);
    static Node* reconstruct_tree(uint8_t* tree_dump, uint32_t tree_size);

    // Windows-specific file size function
    static uint64_t get_file_size(const std::string& filename);
};

#endif