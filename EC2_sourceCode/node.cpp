#include "node.h"

Node::Node(uint8_t symbol, uint64_t frequency)
    : left(nullptr), right(nullptr), symbol(symbol), frequency(frequency) {
}

Node::~Node() {
    delete left;
    delete right;
}

// Implementare funcție globală (nu metodă a clasei)
Node* node_join(Node* left, Node* right) {
    Node* parent = new Node('$', left->frequency + right->frequency);
    parent->left = left;
    parent->right = right;
    return parent;
}

bool Node::is_leaf() const {
    return (left == nullptr && right == nullptr);
}