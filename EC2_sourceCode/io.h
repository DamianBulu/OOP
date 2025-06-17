#ifndef IO_H
#define IO_H

#include "defines.h"
#include <cstdint>
#include <string>
#include <fstream>

class BitReader {
private:
    std::ifstream infile;
    uint8_t buffer[BLOCK];
    uint64_t buffer_index;
    uint64_t buffer_size;
    uint8_t bit_position;

public:
    BitReader(const std::string& filename);
    ~BitReader();

    // Read one bit from the file
    bool read_bit(uint8_t& bit);

    // Read a byte from the file
    bool read_byte(uint8_t& byte);

    // Read multiple bytes
    uint64_t read_bytes(uint8_t* dest, uint64_t size);

    // Check if there are more bits to read
    bool has_more_bits() const;
};

class BitWriter {
private:
    std::ofstream outfile;
    uint8_t buffer[BLOCK];
    uint64_t buffer_index;
    uint8_t bit_position;

public:
    BitWriter(const std::string& filename);
    ~BitWriter();

    // Write one bit to the file
    void write_bit(uint8_t bit);

    // Write a byte to the file
    void write_byte(uint8_t byte);

    // Write multiple bytes
    void write_bytes(const uint8_t* src, uint64_t size);

    // Flush remaining bits in buffer to file
    void flush();
};

#endif // IO_H