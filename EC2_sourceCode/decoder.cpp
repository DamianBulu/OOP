//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <string>
//#include <cstring>
//#include "huffman.h"
//#include "defines.h"
//
//using namespace std;
//
//void print_help() {
//    cout << "Usage: decoder [-h] [-i infile] [-o outfile] [-s]\n"
//        << "Options:\n"
//        << "  -h         Print this help message\n"
//        << "  -i infile  Input file to decode (default: stdin)\n"
//        << "  -o outfile Output file for decompressed data (default: stdout)\n"
//        << "  -s         Print decompression statistics\n";
//}
//
//int main(int argc, char* argv[]) {
//    string input_file;
//    string output_file;
//    bool show_stats = false;
//
//    // Parse command line arguments
//    for (int i = 1; i < argc; i++) {
//        if (strcmp(argv[i], "-h") == 0) {
//            print_help();
//            return 0;
//        }
//        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
//            input_file = argv[++i];
//        }
//        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
//            output_file = argv[++i];
//        }
//        else if (strcmp(argv[i], "-s") == 0) {
//            show_stats = true;
//        }
//    }
//
//    // Open input file or stdin
//    istream* in = &cin;
//    ifstream infile;
//    if (!input_file.empty()) {
//        infile.open(input_file, ios::binary);
//        if (!infile) {
//            cerr << "Error: Could not open input file " << input_file << endl;
//            return 1;
//        }
//        in = &infile;
//    }
//
//    // Read header
//    Header header;
//    in->read(reinterpret_cast<char*>(&header), sizeof(header));
//
//    if (header.signature != MAGIC) {
//        cerr << "Error: Invalid file format (bad magic number)\n";
//        return 1;
//    }
//
//    // Read tree dump
//    vector<uint8_t> tree_dump(header.tree_size);
//    in->read(reinterpret_cast<char*>(tree_dump.data()), header.tree_size);
//
//    // Reconstruct Huffman tree
//    Node* root = Huffman::reconstruct_tree(tree_dump.data(), header.tree_size);
//    if (!root) {
//        cerr << "Error: Failed to reconstruct Huffman tree\n";
//        return 1;
//    }
//
//    // Open output file or stdout
//    ostream* out = &cout;
//    ofstream outfile;
//    if (!output_file.empty()) {
//        outfile.open(output_file, ios::binary);
//        if (!outfile) {
//            cerr << "Error: Could not open output file " << output_file << endl;
//            delete root;
//            return 1;
//        }
//        out = &outfile;
//    }
//
//    // Decode the file
//    uint64_t bytes_decoded = 0;
//    Node* current = root;
//    char buffer[BLOCK];
//    uint8_t byte;
//    uint8_t bit_pos = 0;
//
//    while (bytes_decoded < header.file_size) {
//        if (bit_pos == 0) {
//            if (!in->read(reinterpret_cast<char*>(&byte), 1)) {
//                break;
//            }
//            bit_pos = 8;
//        }
//
//        uint8_t bit = (byte >> (bit_pos - 1)) & 1;
//        bit_pos--;
//
//        current = bit ? current->right : current->left;
//
//        if (!current->left && !current->right) {
//            out->put(current->symbol);
//            bytes_decoded++;
//            current = root;
//        }
//    }
//
//    // Calculate compressed size
//    uint64_t compressed_size = sizeof(header) + header.tree_size;
//    compressed_size += in->tellg();
//
//    // Print statistics if requested
//    if (show_stats) {
//        cerr << "Compressed size: " << compressed_size << " bytes\n";
//        cerr << "Decompressed size: " << bytes_decoded << " bytes\n";
//        double saving = 100.0 * (1.0 - (double)compressed_size / bytes_decoded);
//        cerr << "Space saving: " << saving << "%\n";
//    }
//
//    // Clean up
//    delete root;
//    if (!input_file.empty()) infile.close();
//    if (!output_file.empty()) outfile.close();
//
//    return 0;
//}