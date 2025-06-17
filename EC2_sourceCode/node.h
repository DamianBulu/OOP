#ifndef NODE_H
#define NODE_H

#include <cstdint>

class Node {
public:
    Node* left;
    Node* right;
    uint8_t symbol;
    uint64_t frequency;

    Node(uint8_t symbol, uint64_t frequency);
    ~Node();

    bool is_leaf() const;  // Ad?ugat
};

// Func?ia global? (nu metod? a clasei)
Node* node_join(Node* left, Node* right);

#endif