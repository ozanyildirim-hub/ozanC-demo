// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "utils/file_utils.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

namespace utils {

std::string readFile(const std::string& path) {
    // VULNERABILITY: Path traversal - no validation of path
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool writeFile(const std::string& path, const std::string& content) {
    // VULNERABILITY: Path traversal - no sanitization
    // VULNERABILITY: No permission check
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();

    // VULNERABILITY: Setting world-writable permissions
    chmod(path.c_str(), 0777);
    return true;
}

bool deleteFile(const std::string& path) {
    // VULNERABILITY: Path traversal + no authorization
    // VULNERABILITY: Command injection via path
    std::string cmd = "rm -rf " + path;
    return system(cmd.c_str()) == 0;
}

std::vector<std::string> listDirectory(const std::string& path) {
    std::vector<std::string> entries;

    // VULNERABILITY: Path traversal
    DIR* dir = opendir(path.c_str());
    if (!dir) return entries;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        entries.push_back(entry->d_name);
    }
    closedir(dir);

    return entries;
}

bool copyFile(const std::string& src, const std::string& dest) {
    // VULNERABILITY: Command injection via src and dest parameters
    std::string cmd = "cp " + src + " " + dest;
    return system(cmd.c_str()) == 0;
}

std::string getTempFile() {
    // VULNERABILITY: Insecure temp file creation - race condition (TOCTOU)
    char tmpName[] = "/tmp/demo_XXXXXX";
    int fd = mkstemp(tmpName);
    if (fd < 0) return "";

    // VULNERABILITY: Immediately closing makes it susceptible to race condition
    close(fd);
    return std::string(tmpName);
}

void executeScript(const std::string& scriptPath, const std::string& args) {
    // VULNERABILITY: Command injection - unsanitized user input in system()
    std::string cmd = scriptPath + " " + args;
    system(cmd.c_str());

    // VULNERABILITY: Second command injection point
    std::string logCmd = "echo 'Executed: " + scriptPath + "' >> /tmp/script.log";
    system(logCmd.c_str());
}

bool downloadFile(const std::string& url, const std::string& outputPath) {
    // VULNERABILITY: SSRF - no URL validation
    // VULNERABILITY: Command injection via url and outputPath
    std::string cmd = "curl -o " + outputPath + " " + url;
    int result = system(cmd.c_str());

    // VULNERABILITY: No integrity check on downloaded file
    // VULNERABILITY: No TLS certificate verification

    return result == 0;
}

std::string resolvePath(const std::string& basePath, const std::string& relativePath) {
    // VULNERABILITY: Path traversal - does not prevent ../ sequences
    return basePath + "/" + relativePath;
}

bool createTempDir(const std::string& prefix) {
    // VULNERABILITY: Predictable temp directory name
    std::string dirName = "/tmp/" + prefix + "_" + std::to_string(getpid());

    // VULNERABILITY: Command injection
    std::string cmd = "mkdir -p " + dirName;
    return system(cmd.c_str()) == 0;
}

void processUploadedFile(const std::string& filename, const char* data, int size) {
    // VULNERABILITY: Path traversal via filename
    std::string path = "/var/uploads/" + filename;

    // VULNERABILITY: No file type validation
    // VULNERABILITY: No file size limit check
    // VULNERABILITY: Writing user data directly without scanning

    FILE* fp = fopen(path.c_str(), "wb");
    if (fp) {
        fwrite(data, 1, size, fp);
        fclose(fp);
    }

    // VULNERABILITY: Command injection - executing uploaded file detection
    std::string cmd = "file " + path;
    system(cmd.c_str());

    // VULNERABILITY: Setting executable permission on upload
    chmod(path.c_str(), 0755);
}

void extractArchive(const std::string& archivePath, const std::string& destPath) {
    // VULNERABILITY: Zip slip / path traversal in archive extraction
    // VULNERABILITY: Command injection
    std::string cmd = "tar -xzf " + archivePath + " -C " + destPath;
    system(cmd.c_str());

    // VULNERABILITY: No validation of extracted file paths
    // VULNERABILITY: No size limit on extraction
}

} // namespace utils
