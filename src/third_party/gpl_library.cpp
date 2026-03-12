/*
 * GPL Data Compression Library
 * Copyright (C) 2024 Open Source Contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * LICENSE VIOLATION: GPL v2 code distributed under MIT license.
 */

#include "third_party/gpl_library.h"
#include <cstring>
#include <cstdlib>
#include <algorithm>

namespace gpl_compress {

DataCompressor::DataCompressor()
    : compressionLevel_(6), lastRatio_(0), internalState_(nullptr) {
    // MEMORY LEAK: allocating but never freeing in destructor
    internalState_ = malloc(4096);
}

DataCompressor::~DataCompressor() {
    // BUG: Memory leak - internalState_ never freed
}

std::string DataCompressor::compress(const std::string& data) {
    std::string result;
    result.reserve(data.size());

    // Simple RLE compression for demo
    size_t i = 0;
    while (i < data.size()) {
        char current = data[i];
        int count = 1;
        while (i + count < data.size() && data[i + count] == current && count < 255) {
            count++;
        }
        result += current;
        result += (char)count;
        i += count;
    }

    if (data.size() > 0) {
        lastRatio_ = (int)((result.size() * 100) / data.size());
    }

    return result;
}

std::string DataCompressor::decompress(const std::string& data) {
    std::string result;

    // BUG: No bounds checking - can read past end of data
    for (size_t i = 0; i < data.size(); i += 2) {
        char c = data[i];
        int count = (unsigned char)data[i + 1];  // can read past end when data.size() is odd
        result.append(count, c);
    }

    return result;
}

std::vector<unsigned char> DataCompressor::compressBytes(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> result;

    for (size_t i = 0; i < data.size(); ) {
        unsigned char current = data[i];
        int count = 1;
        while (i + count < data.size() && data[i + count] == current && count < 255) {
            count++;
        }
        result.push_back(current);
        result.push_back((unsigned char)count);
        i += count;
    }

    return result;
}

std::vector<unsigned char> DataCompressor::decompressBytes(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> result;

    for (size_t i = 0; i + 1 < data.size(); i += 2) {
        unsigned char c = data[i];
        int count = data[i + 1];
        for (int j = 0; j < count; j++) {
            result.push_back(c);
        }
    }

    return result;
}

void DataCompressor::setCompressionLevel(int level) {
    // BUG: No validation of level range
    compressionLevel_ = level;
}

int DataCompressor::getCompressionRatio() const {
    return lastRatio_;
}

std::string quickCompress(const std::string& data) {
    DataCompressor compressor;
    return compressor.compress(data);
}

std::string quickDecompress(const std::string& data) {
    DataCompressor compressor;
    return compressor.decompress(data);
}

} // namespace gpl_compress
