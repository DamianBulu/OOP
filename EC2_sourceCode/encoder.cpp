#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include "huffman.h"
#include "defines.h"
#include <stack>
#include "node.h"

using namespace std;

void print_help() {
    cout << "Usage: encoder [-h] [-i infile] [-o outfile] [-s]\n"
        << "Options:\n"
        << "  -h         Print this help message\n"
        << "  -i infile  Input file to encode (default: stdin)\n"
        << "  -o outfile Output file for compressed data (default: stdout)\n"
        << "  -s         Print compression statistics\n";
}

int main(int argc, char* argv[]) {
    string input_file;
    string output_file;
    bool show_stats = false;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            input_file = argv[++i];
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        }
        else if (strcmp(argv[i], "-s") == 0) {
            show_stats = true;
        }
    }

    // Open input file or stdin
    istream* in = &cin;
    ifstream infile;
    uint64_t file_size = 0;

    if (!input_file.empty()) {
        infile.open(input_file, ios::binary);
        if (!infile) {
            cerr << "Error: Could not open input file " << input_file << endl;
            return 1;
        }
        in = &infile;
        file_size = Huffman::get_file_size(input_file);
    }

    // Compute histogram
    uint64_t histogram[ALPHABET] = { 0 };
    char buffer[BLOCK];

    if (input_file.empty()) {
        // Reading from stdin - need to count bytes
        while (in->read(buffer, BLOCK) || in->gcount()) {
            file_size += in->gcount();
            for (int i = 0; i < in->gcount(); i++) {
                histogram[(uint8_t)buffer[i]]++;
            }
        }
    }
    else {
        // Reading from file - already have size
        while (in->read(buffer, BLOCK) || in->gcount()) {
            for (int i = 0; i < in->gcount(); i++) {
                histogram[(uint8_t)buffer[i]]++;
            }
        }
    }

    // Ensure at least two symbols
    histogram[0]++;
    histogram[255]++;

    // Build Huffman tree
    Node* root = Huffman::build_tree(histogram);
    if (!root) {
        cerr << "Error: Failed to build Huffman tree\n";
        return 1;
    }

    // Build code table
    Code codes[ALPHABET];
    Huffman::build_codes(root, codes);

    // Calculate tree dump size
    uint32_t tree_size = 0;
    stack<Node*> s;
    s.push(root);

    while (!s.empty()) {
        Node* node = s.top();
        s.pop();

        if (node->left && node->right) {
            tree_size += 1; // 'I'
            s.push(node->right);
            s.push(node->left);
        }
        else {
            tree_size += 2; // 'L' + symbol
        }
    }

    // Open output file or stdout
    ostream* out = &cout;
    ofstream outfile;
    if (!output_file.empty()) {
        outfile.open(output_file, ios::binary);
        if (!outfile) {
            cerr << "Error: Could not open output file " << output_file << endl;
            delete root;
            return 1;
        }
        out = &outfile;
    }

    // Write header
    Header header;
    header.signature = MAGIC;
    header.tree_size = tree_size;
    header.file_size = file_size;
    out->write(reinterpret_cast<char*>(&header), sizeof(header));

    // Write tree dump (post-order traversal)
    stack<Node*> traversal_stack;
    Node* current = root;
    vector<uint8_t> tree_dump;

    while (current || !traversal_stack.empty()) {
        while (current) {
            traversal_stack.push(current);
            traversal_stack.push(nullptr); // Mark for right child
            current = current->left;
        }

        if (traversal_stack.empty()) break;

        current = traversal_stack.top();
        traversal_stack.pop();

        if (current == nullptr) {
            current = traversal_stack.top();
            traversal_stack.pop();

            if (current->left && current->right) {
                tree_dump.push_back('I');
            }
            else {
                tree_dump.push_back('L');
                tree_dump.push_back(current->symbol);
            }

            current = nullptr;
        }
        else {
            traversal_stack.push(current);
            traversal_stack.push(nullptr);
            current = current->right;
        }
    }

    out->write(reinterpret_cast<char*>(tree_dump.data()), tree_dump.size());

    // Encode the file
    in->clear();
    if (input_file.empty()) {
        // Can't seek on stdin, need to reopen
        cerr << "Error: Cannot re-read from stdin\n";
        delete root;
        return 1;
    }
    in->seekg(0);

    uint8_t output_byte = 0;
    uint8_t bit_pos = 0;

    while (in->read(buffer, BLOCK) || in->gcount()) {
        for (int i = 0; i < in->gcount(); i++) {
            uint8_t symbol = buffer[i];
            Code& code = codes[symbol];

            for (uint32_t j = 0; j < code.size(); j++) {
                uint8_t bit;
                if (!code.pop_bit(bit)) break;

                if (bit) {
                    output_byte |= (1 << bit_pos);
                }

                bit_pos++;
                if (bit_pos == 8) {
                    out->put(output_byte);
                    output_byte = 0;
                    bit_pos = 0;
                }
            }
        }
    }

    // Flush remaining bits
    if (bit_pos > 0) {
        out->put(output_byte);
    }

    // Calculate compressed size
    uint64_t compressed_size = sizeof(header) + tree_dump.size();
    compressed_size += (file_size * 8 + bit_pos) / 8;

    // Print statistics if requested
    if (show_stats) {
        cerr << "Uncompressed size: " << file_size << " bytes\n";
        cerr << "Compressed size: " << compressed_size << " bytes\n";
        double saving = 100.0 * (1.0 - (double)compressed_size / file_size);
        cerr << "Space saving: " << saving << "%\n";
    }

    // Clean up
    delete root;
    if (!input_file.empty()) infile.close();
    if (!output_file.empty()) outfile.close();

    return 0;
}