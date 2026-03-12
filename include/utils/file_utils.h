// Copyright (c) 2026 Ozan Yildirim - MIT License
#pragma once

#include <string>
#include <vector>

namespace utils {

std::string readFile(const std::string& path);
bool writeFile(const std::string& path, const std::string& content);
bool deleteFile(const std::string& path);
std::vector<std::string> listDirectory(const std::string& path);
bool copyFile(const std::string& src, const std::string& dest);
std::string getTempFile();
void executeScript(const std::string& scriptPath, const std::string& args);
bool downloadFile(const std::string& url, const std::string& outputPath);
std::string resolvePath(const std::string& basePath, const std::string& relativePath);
bool createTempDir(const std::string& prefix);
void processUploadedFile(const std::string& filename, const char* data, int size);
void extractArchive(const std::string& archivePath, const std::string& destPath);

} // namespace utils
