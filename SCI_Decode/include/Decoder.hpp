#ifndef DECODER_H
#define DECODER_H

#include <iostream>
#include <cstddef>
#include <cassert>
#include <iomanip>
#include <stdint.h>

using namespace std;

class Decoder {
protected:
    template <class T>
    T decode_bit(const char* buffer, size_t begin, size_t end);
    template <class T>
    T decode_byte(const char* buffer, size_t begin, size_t end); 
};

template <class T>
T Decoder::decode_bit(const char* buffer, size_t begin, size_t end) {
    T sum = 0;
    int begin_byte = begin / 8;
    int begin_pos = begin % 8;
    int end_byte = end / 8;
    int end_pos = end % 8;
    switch (begin_pos) {
    case 0:
        sum = static_cast<uint8_t>(buffer[begin_byte]) & 0xFF;
        break;
    case 1:
        sum = static_cast<uint8_t>(buffer[begin_byte]) & 0x7F;
        break;
    case 2:
        sum = static_cast<uint8_t>(buffer[begin_byte]) & 0x3F;
        break;
    case 3:
        sum = static_cast<uint8_t>(buffer[begin_byte]) & 0x1F;
        break;
    case 4:
        sum = static_cast<uint8_t>(buffer[begin_byte]) & 0x0F;
        break;
    case 5:
        sum = static_cast<uint8_t>(buffer[begin_byte]) & 0x07;
        break;
    case 6:
        sum = static_cast<uint8_t>(buffer[begin_byte]) & 0x03;
        break;
    case 7:
        sum = static_cast<uint8_t>(buffer[begin_byte]) & 0x01;
        break;
    }
    if (begin_byte == end_byte) {
        sum >>= (8 - end_pos - 1);
        return sum;
    } else {
        for (int i = 1; begin_byte + i < end_byte; i++) {
            sum <<= 8;
            sum += static_cast<uint8_t>(buffer[begin_byte + i]);
        }
        sum <<= (end_pos + 1);
        sum += static_cast<uint8_t>(buffer[end_byte]) >> (8 - end_pos -1);
        return sum;
    }
}

template <class T>
T Decoder::decode_byte(const char* buffer, size_t begin, size_t end) {
    T sum = 0;
    for (int i = 0; begin + i <= end; i++) {
        sum <<= 8;
        sum += static_cast<uint8_t>(buffer[begin + i]);
    }
    return sum;
}

#endif
