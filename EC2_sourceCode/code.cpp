#include "code.h"
#include <cstring>

Code::Code() : top(0) {
    memset(bits, 0, sizeof(bits));  // Folosim sizeof(bits) în loc de MAX_CODE_SIZE
}

bool Code::is_empty() const {
    return top == 0;
}

bool Code::is_full() const {
    return top >= MAX_CODE_SIZE * 8;
}

uint32_t Code::size() const {
    return top;
}

bool Code::push_bit(uint8_t bit) {
    if (is_full()) {
        return false;
    }

    uint32_t byte_index = top / 8;
    uint32_t bit_index = top % 8;

    if (bit) {
        bits[byte_index] |= (1 << bit_index);
    }
    else {
        bits[byte_index] &= ~(1 << bit_index);
    }

    top++;
    return true;
}

bool Code::pop_bit(uint8_t& bit) {
    if (is_empty()) return false;
    top--;
    uint32_t byte_index = top / 8;
    uint32_t bit_index = top % 8;
    bit = (bits[byte_index] >> bit_index) & 1;
    return true;
}

uint8_t Code::get_bit(uint32_t index) const {
    if (index >= top) {
        return 0;
    }
    uint32_t byte_index = index / 8;
    uint32_t bit_index = index % 8;
    return (bits[byte_index] >> bit_index) & 1;
}

void Code::clear() {
    top = 0;
    memset(bits, 0, sizeof(bits));
}