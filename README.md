# ozanC++demo

A C++ demonstration project designed to showcase **SonarQube Cloud** analysis capabilities. This project intentionally contains various categories of issues for demonstration purposes.

> **WARNING**: This project contains intentionally vulnerable code for demonstration purposes only. Do NOT use any of this code in production.

## Purpose

This project is used to demonstrate SonarQube Cloud's ability to detect:

### Security Vulnerabilities
- **Buffer Overflows**: `strcpy`, `sprintf`, `memcpy` without bounds checking
- **SQL Injection**: Direct string concatenation in SQL queries
- **Command Injection**: Unsanitized input passed to `system()`
- **Hardcoded Credentials**: Passwords, API keys, and secrets in source code
- **Path Traversal**: Unsanitized file paths from user input
- **Format String Vulnerabilities**: User-controlled format strings in `printf`/`sprintf`
- **Use After Free**: Accessing memory after deallocation
- **Weak Cryptography**: MD5, SHA-1, XOR "encryption", weak PRNG (`rand()`)
- **SSRF**: Server-side request forgery via unvalidated URLs
- **XSS**: Template rendering without output escaping
- **Information Disclosure**: Logging sensitive data, server version headers

### Security Hotspots
- Insecure file permissions (`chmod 777`)
- World-readable temp files with credentials
- Missing TLS certificate verification
- Predictable session tokens and random values
- Disabled signature verification

### Code Smells
- Dead/unreachable code
- Duplicated code blocks
- High cyclomatic complexity
- Magic numbers
- Empty catch blocks
- Unused variables and functions
- Poor error handling

### Bugs
- Memory leaks (missing `free`/`delete`)
- Null pointer dereferences
- Race conditions (shared state without synchronization)
- Off-by-one errors
- Integer overflow potential

### License Violations
- **GPL v2 code** (`src/third_party/`) included in an **MIT-licensed** project
- Incompatible license mixing that violates GPL copyleft requirements

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running Analysis

```bash
# Generate compile_commands.json
mkdir build && cd build
cmake ..

# Run SonarQube analysis
sonar-scanner
```

## Project Structure

```
ozanC++demo/
├── CMakeLists.txt
├── LICENSE                          # MIT License (project-level)
├── sonar-project.properties
├── include/
│   ├── auth/auth_manager.h
│   ├── network/http_server.h
│   ├── database/db_handler.h
│   ├── utils/string_utils.h
│   ├── utils/file_utils.h
│   ├── crypto/crypto_utils.h
│   └── third_party/gpl_library.h   # GPL v2 (license conflict!)
├── src/
│   ├── main.cpp
│   ├── auth/auth_manager.cpp
│   ├── network/http_server.cpp
│   ├── database/db_handler.cpp
│   ├── utils/string_utils.cpp
│   ├── utils/file_utils.cpp
│   ├── crypto/crypto_utils.cpp
│   └── third_party/
│       ├── gpl_library.cpp          # GPL v2 (license conflict!)
│       └── COPYING                  # GPL v2 license text
└── tests/
    └── test_main.cpp
```
