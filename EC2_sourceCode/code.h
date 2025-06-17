#ifndef CODE_H
#define CODE_H

#include <cstdint>
#include "defines.h"

class Code {
public:
    uint32_t top;
    uint8_t bits[MAX_CODE_SIZE];

    Code();
    bool is_empty() const;
    bool is_full() const;
    uint32_t size() const;
    bool push_bit(uint8_t bit);
    bool pop_bit(uint8_t& bit);  // Schimbat de la uint8_t* la uint8_t&
    uint8_t get_bit(uint32_t index) const;  // Adăugat
    void clear();  // Adăugat
};

#endif