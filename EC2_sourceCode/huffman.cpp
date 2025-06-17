#include "huffman.h"
#include <queue>
#include <vector>
#include <stack>
#include <functional>
#include <algorithm>

using namespace std;

// Comparator for priority queue
struct NodeCompare {
    bool operator()(Node* a, Node* b) {
        return a->frequency > b->frequency; // Min-heap based on frequency
    }
};

Node* Huffman::build_tree(uint64_t histogram[ALPHABET]) {
    priority_queue<Node*, vector<Node*>, NodeCompare> pq;

    // Create leaf nodes for all symbols with non-zero frequency
    for (int i = 0; i < ALPHABET; i++) {
        if (histogram[i] > 0) {
            pq.push(new Node(static_cast<uint8_t>(i), histogram[i]));
        }
    }

    // Special case: if only one symbol, add a dummy symbol
    if (pq.size() == 1) {
        uint8_t dummy_symbol = pq.top()->symbol == 0 ? 255 : 0;
        pq.push(new Node(dummy_symbol, 1));
    }

    // Build the Huffman tree
    while (pq.size() > 1) {
        // Extract the two nodes with lowest frequency
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        // Create new internal node and push to queue
        Node* parent = node_join(left, right);
        pq.push(parent);
    }

    return pq.empty() ? nullptr : pq.top();
}

void Huffman::build_codes(Node* root, Code* codes) {
    if (root == nullptr) return;

    stack<pair<Node*, Code>> s;
    s.push({ root, Code() });

    while (!s.empty()) {
        auto [node, code] = s.top();
        s.pop();

        if (node->is_leaf()) {
            codes[node->symbol] = code;
            continue;
        }

        if (node->right) {
            Code right_code = code;
            right_code.push_bit(1);
            s.push({ node->right, right_code });
        }

        if (node->left) {
            Code left_code = code;
            left_code.push_bit(0);
            s.push({ node->left, left_code });
        }
    }
}

Node* Huffman::reconstruct_tree(uint8_t* tree_dump, uint32_t tree_size) {
    stack<Node*> node_stack;

    for (uint32_t i = 0; i < tree_size; ) {
        if (tree_dump[i] == 'L') {
            // Leaf node
            if (i + 1 >= tree_size) return nullptr; // Invalid format
            uint8_t symbol = tree_dump[i + 1];
            node_stack.push(new Node(symbol, 0));
            i += 2;
        }
        else if (tree_dump[i] == 'I') {
            // Internal node
            if (node_stack.size() < 2) return nullptr; // Invalid format

            Node* right = node_stack.top();
            node_stack.pop();
            Node* left = node_stack.top();
            node_stack.pop();

            Node* parent = node_join(left, right);
            node_stack.push(parent);
            i += 1;
        }
        else {
            // Invalid character in tree dump
            return nullptr;
        }
    }

    if (node_stack.size() != 1) return nullptr;
    return node_stack.top();
}

uint64_t Huffman::get_file_size(const std::string& filename) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesExA(filename.c_str(), GetFileExInfoStandard, &fileInfo)) {
        ULARGE_INTEGER size;
        size.LowPart = fileInfo.nFileSizeLow;
        size.HighPart = fileInfo.nFileSizeHigh;
        return size.QuadPart;
    }
    return 0;
}