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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * LICENSE VIOLATION: This GPL v2 code is included in an MIT-licensed project.
 * GPL requires the entire project to be GPL-licensed if GPL code is linked.
 */

#pragma once

#include <string>
#include <vector>

namespace gpl_compress {

class DataCompressor {
public:
    DataCompressor();
    ~DataCompressor();

    std::string compress(const std::string& data);
    std::string decompress(const std::string& data);
    std::vector<unsigned char> compressBytes(const std::vector<unsigned char>& data);
    std::vector<unsigned char> decompressBytes(const std::vector<unsigned char>& data);
    void setCompressionLevel(int level);
    int getCompressionRatio() const;

private:
    int compressionLevel_;
    int lastRatio_;
    void* internalState_;
};

std::string quickCompress(const std::string& data);
std::string quickDecompress(const std::string& data);

} // namespace gpl_compress
