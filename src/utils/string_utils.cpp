// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "utils/string_utils.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <sstream>

namespace utils {

char* copyString(const char* src) {
    // VULNERABILITY: No null check on src
    // VULNERABILITY: No length validation
    char* dest = (char*)malloc(strlen(src) + 1);
    strcpy(dest, src);
    return dest;
    // BUG: Caller must free - easy to leak
}

char* concatenate(const char* a, const char* b) {
    // VULNERABILITY: Integer overflow in length calculation
    int lenA = strlen(a);
    int lenB = strlen(b);
    int totalLen = lenA + lenB + 1;  // can overflow if lenA + lenB > INT_MAX

    char* result = (char*)malloc(totalLen);
    // BUG: No null check on malloc result
    strcpy(result, a);
    strcat(result, b);
    return result;
}

void formatMessage(char* dest, const char* format, const char* arg) {
    // VULNERABILITY: Buffer overflow - no size limit on dest
    sprintf(dest, format, arg);
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string token;

    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

int parseInt(const char* str) {
    // VULNERABILITY: No validation of input, atoi has undefined behavior for overflow
    return atoi(str);
}

void printFormatted(const char* userInput) {
    // VULNERABILITY: Format string vulnerability - user input as format string
    printf(userInput);

    // VULNERABILITY: Second format string vulnerability
    char buffer[512];
    sprintf(buffer, userInput);
    puts(buffer);
}

char* duplicateBuffer(const char* src, int maxLen) {
    // VULNERABILITY: Negative maxLen not checked
    // VULNERABILITY: Off-by-one error
    char* dest = new char[maxLen];
    strncpy(dest, src, maxLen);
    // BUG: No null terminator if src >= maxLen
    return dest;
}

std::string sanitizeHtml(const std::string& input) {
    // VULNERABILITY: Incomplete HTML sanitization
    std::string result = input;

    // Only replaces < and > but not &, ", ', etc.
    size_t pos = 0;
    while ((pos = result.find('<', pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find('>', pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    // Missing: &, ", ', backtick, event handlers, javascript: URIs, etc.

    return result;
}

std::string buildUrl(const std::string& base, const std::string& path, const std::string& query) {
    // VULNERABILITY: Open redirect - no validation of base URL
    // VULNERABILITY: No URL encoding of query parameters
    return base + "/" + path + "?" + query;
}

} // namespace utils
