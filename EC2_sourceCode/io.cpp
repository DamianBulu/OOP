#include "io.h"
#include <iostream>

BitReader::BitReader(const std::string& filename) : buffer_index(0), buffer_size(0), bit_position(0) {
    infile.open(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error: Cannot open file " << filename << " for reading." << std::endl;
    }

    // Fill the buffer initially
    buffer_size = infile.read(reinterpret_cast<char*>(buffer), BLOCK).gcount();
}

BitReader::~BitReader() {
    if (infile.is_open()) {
        infile.close();
    }
}

bool BitReader::read_bit(uint8_t& bit) {
    // If the buffer is empty or we've read all bits in the current byte
    if (buffer_index >= buffer_size || (bit_position == 0 && buffer_index == buffer_size - 1)) {
        // Try to refill the buffer
        buffer_index = 0;
        buffer_size = infile.read(reinterpret_cast<char*>(buffer), BLOCK).gcount();

        // If we couldn't read any more data
        if (buffer_size == 0) {
            return false;
        }
    }

    // Extract the current bit
    bit = (buffer[buffer_index] >> bit_position) & 1;

    // Move to the next bit
    bit_position++;

    // If we've read all bits in the current byte
    if (bit_position >= 8) {
        bit_position = 0;
        buffer_index++;
    }

    return true;
}

bool BitReader::read_byte(uint8_t& byte) {
    byte = 0;

    // If we're at a byte boundary, we can read a full byte directly
    if (bit_position == 0) {
        if (buffer_index >= buffer_size) {
            // Try to refill the buffer
            buffer_index = 0;
            buffer_size = infile.read(reinterpret_cast<char*>(buffer), BLOCK).gcount();

            // If we couldn't read any more data
            if (buffer_size == 0) {
                return false;
            }
        }

        byte = buffer[buffer_index++];
        return true;
    }
    else {
        // We need to read 8 bits
        for (int i = 0; i < 8; i++) {
            uint8_t bit;
            if (!read_bit(bit)) {
                return false;
            }
            byte |= (bit << i);
        }
        return true;
    }
}

uint64_t BitReader::read_bytes(uint8_t* dest, uint64_t size) {
    uint64_t bytes_read = 0;

    for (uint64_t i = 0; i < size; i++) {
        if (!read_byte(dest[i])) {
            break;
        }
        bytes_read++;
    }

    return bytes_read;
}

bool BitReader::has_more_bits() const {
    return buffer_index < buffer_size || infile.good();
}

BitWriter::BitWriter(const std::string& filename) : buffer_index(0), bit_position(0) {
    outfile.open(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error: Cannot open file " << filename << " for writing." << std::endl;
    }

    // Initialize buffer
    memset(buffer, 0, BLOCK);
}

BitWriter::~BitWriter() {
    flush();
    if (outfile.is_open()) {
        outfile.close();
    }
}

void BitWriter::write_bit(uint8_t bit) {
    // Set or clear the bit
    if (bit) {
        buffer[buffer_index] |= (1 << bit_position);
    }
    else {
        buffer[buffer_index] &= ~(1 << bit_position);
    }

    // Move to the next bit
    bit_position++;

    // If we've filled a byte
    if (bit_position >= 8) {
        bit_position = 0;
        buffer_index++;

        // If the buffer is full, flush it
        if (buffer_index >= BLOCK) {
            outfile.write(reinterpret_cast<char*>(buffer), BLOCK);
            buffer_index = 0;
            memset(buffer, 0, BLOCK);
        }
    }
}

void BitWriter::write_byte(uint8_t byte) {
    // If we're at a byte boundary, we can write the full byte directly
    if (bit_position == 0) {
        buffer[buffer_index++] = byte;

        // If the buffer is full, flush it
        if (buffer_index >= BLOCK) {
            outfile.write(reinterpret_cast<char*>(buffer), BLOCK);
            buffer_index = 0;
            memset(buffer, 0, BLOCK);
        }
    }
    else {
        // We need to write 8 bits
        for (int i = 0; i < 8; i++) {
            write_bit((byte >> i) & 1);
        }
    }
}

void BitWriter::write_bytes(const uint8_t* src, uint64_t size) {
    for (uint64_t i = 0; i < size; i++) {
        write_byte(src[i]);
    }
}

void BitWriter::flush() {
    // If we have bits pending in the current byte, write the byte
    if (bit_position > 0) {
        buffer_index++;
    }

    // Write any remaining bytes in the buffer
    if (buffer_index > 0) {
        outfile.write(reinterpret_cast<char*>(buffer), buffer_index);
        buffer_index = 0;
        bit_position = 0;
        memset(buffer, 0, BLOCK);
    }

    outfile.flush();
}