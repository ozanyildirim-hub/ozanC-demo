// Copyright (c) 2026 Ozan Yildirim - MIT License
#pragma once

#include <string>
#include <vector>

namespace utils {

char* copyString(const char* src);
char* concatenate(const char* a, const char* b);
void formatMessage(char* dest, const char* format, const char* arg);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string trim(const std::string& str);
std::string toUpper(const std::string& str);
std::string toLower(const std::string& str);
int parseInt(const char* str);
void printFormatted(const char* userInput);
char* duplicateBuffer(const char* src, int maxLen);
std::string sanitizeHtml(const std::string& input);
std::string buildUrl(const std::string& base, const std::string& path, const std::string& query);

} // namespace utils
