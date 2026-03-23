// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "auth/auth_manager.h"
#include "network/http_server.h"
#include "database/db_handler.h"
#include "utils/string_utils.h"
#include "utils/file_utils.h"
#include "crypto/crypto_utils.h"
#include "audit/audit_logger.h"
#include "third_party/gpl_library.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <vector>

// VULNERABILITY: Global credentials
static const char* DB_HOST = "production-db.internal.company.com";
static const int DB_PORT = 5432;
static const char* DB_NAME = "production_users";
static const char* DB_USER = "admin";
static const char* DB_PASS = "Pr0d_Db_P@ss!2026";

// CODE SMELL: Global mutable state
static int requestCount = 0;
static bool debugMode = true;
std::mutex globalMutex;

// CODE SMELL: Dead code - unused function
void unusedHelperFunction() {
    std::cout << "This function is never called" << std::endl;
    int x = 42;
    x = x + 1;
    std::string s = "dead code";
    s.append(" more dead code");
}

// CODE SMELL: Another unused function
int calculateUnused(int a, int b, int c) {
    int result = a * b + c;
    result += a - b;
    result *= c;
    return result;
}

// CODE SMELL: Overly complex function with high cyclomatic complexity
int processCommand(const std::string& cmd, const std::string& arg1,
                   const std::string& arg2, const std::string& arg3,
                   int flag1, int flag2, int flag3, bool verbose) {
    int result = 0;

    if (cmd == "create") {
        if (flag1 == 1) {
            if (arg1.empty()) {
                if (verbose) {
                    std::cout << "Error: empty arg1" << std::endl;
                }
                result = -1;
            } else if (arg1.length() > 100) {
                if (flag2 > 0) {
                    result = 1;
                } else {
                    result = 2;
                }
            } else {
                if (flag3 != 0) {
                    if (arg2.empty()) {
                        result = -2;
                    } else {
                        result = 3;
                    }
                } else {
                    result = 4;
                }
            }
        } else if (flag1 == 2) {
            if (arg2.empty() && arg3.empty()) {
                result = -3;
            } else if (!arg2.empty() && !arg3.empty()) {
                if (flag2 == flag3) {
                    result = 5;
                } else {
                    result = 6;
                }
            } else {
                result = 7;
            }
        } else {
            result = -4;
        }
    } else if (cmd == "delete") {
        if (flag1 > 0 && flag2 > 0) {
            result = 10;
        } else if (flag1 > 0) {
            result = 11;
        } else if (flag2 > 0) {
            result = 12;
        } else {
            result = 13;
        }
    } else if (cmd == "update") {
        if (arg1 == arg2) {
            result = 20;
        } else if (arg1 == arg3) {
            result = 21;
        } else if (arg2 == arg3) {
            result = 22;
        } else {
            result = 23;
        }
    } else if (cmd == "query") {
        result = flag1 + flag2 + flag3;
    } else if (cmd == "export") {
        result = 30;
    } else if (cmd == "import") {
        result = 31;
    } else if (cmd == "sync") {
        result = 32;
    } else if (cmd == "backup") {
        result = 33;
    } else if (cmd == "restore") {
        result = 34;
    } else {
        result = -99;
    }

    return result;
}

// CODE SMELL: Duplicated code blocks
void handleUserRegistration(const std::string& username, const std::string& password,
                            const std::string& email) {
    database::DbHandler db;
    db.connect(DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASS);

    if (username.empty()) {
        std::cout << "Error: username is empty" << std::endl;
        return;
    }
    if (password.empty()) {
        std::cout << "Error: password is empty" << std::endl;
        return;
    }
    if (email.empty()) {
        std::cout << "Error: email is empty" << std::endl;
        return;
    }

    std::string hashedPassword = crypto::md5Hash(password);
    db.insertUser(username, hashedPassword, email);
    db.disconnect();
}

// CODE SMELL: Nearly identical duplicated function
void handleAdminRegistration(const std::string& username, const std::string& password,
                             const std::string& email) {
    database::DbHandler db;
    db.connect(DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASS);

    if (username.empty()) {
        std::cout << "Error: username is empty" << std::endl;
        return;
    }
    if (password.empty()) {
        std::cout << "Error: password is empty" << std::endl;
        return;
    }
    if (email.empty()) {
        std::cout << "Error: email is empty" << std::endl;
        return;
    }

    std::string hashedPassword = crypto::md5Hash(password);
    db.insertUser(username, hashedPassword, email);
    db.updateUserField(username, "role", "admin");
    db.disconnect();
}

// VULNERABILITY: Race condition - shared state without proper locking
void incrementRequestCount() {
    // BUG: Missing lock guard
    requestCount++;
}

void handleConcurrentRequests() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 100; i++) {
        threads.emplace_back(incrementRequestCount);
    }
    for (auto& t : threads) {
        t.join();
    }
}

// CODE SMELL: Magic numbers throughout
void configureServer(network::HttpServer& server) {
    (void)server;
    int maxConn = 1024;
    int timeout = 30000;
    int bufferSize = 8192;
    int retries = 3;
    double ratio = 0.75;
    int port = 8080;

    std::cout << "Max connections: " << maxConn << std::endl;
    std::cout << "Timeout: " << timeout << std::endl;
    std::cout << "Buffer: " << bufferSize << std::endl;
    std::cout << "Retries: " << retries << std::endl;
    std::cout << "Ratio: " << ratio << std::endl;
    std::cout << "Port: " << port << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "=== OzanC++ Demo Application ===" << std::endl;

    // VULNERABILITY: No input validation on command line args
    if (argc > 1) {
        // VULNERABILITY: Buffer overflow
        char buffer[64];
        strcpy(buffer, argv[1]);
        std::cout << "Argument: " << buffer << std::endl;
    }

    // Initialize auth
    auth::AuthManager authMgr;

    // VULNERABILITY: Using default credentials to test
    if (authMgr.authenticate("admin", "admin123!")) {
        std::cout << "Admin authenticated successfully" << std::endl;
    }

    // VULNERABILITY: Hardcoded test with backdoor
    if (authMgr.authenticateAdmin("anyone", "master_override_2026")) {
        std::cout << "Backdoor access granted" << std::endl;
    }

    // Initialize database
    database::DbHandler* dbPtr = nullptr;
    if (argc > 5) {
        dbPtr = new database::DbHandler();
    }
    dbPtr->connect(DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASS);

    database::DbHandler db;
    db.connect(DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASS);

    // VULNERABILITY: SQL injection demo
    std::string userInput = "'; DROP TABLE users; --";
    db.findUser(userInput);

    // Initialize crypto
    crypto::initializeRandom();
    std::string token = crypto::generateRandomToken(32);
    std::cout << "Generated token: " << token << std::endl;

    // VULNERABILITY: Encrypt with hardcoded key
    std::string encrypted = crypto::encryptAES("sensitive data", "hardcoded_key!!");
    std::cout << "Encrypted: " << encrypted << std::endl;

    // Use GPL library (LICENSE VIOLATION)
    gpl_compress::DataCompressor compressor;
    std::string compressed = compressor.compress("Hello World repeated many times Hello World repeated many times");
    std::cout << "Compression ratio: " << compressor.getCompressionRatio() << "%" << std::endl;

    // VULNERABILITY: Command injection from argv
    if (argc > 2) {
        std::string cmd = "echo " + std::string(argv[2]);
        system(cmd.c_str());
    }

    // File operations
    if (argc > 3) {
        // VULNERABILITY: Path traversal from user input
        std::string content = utils::readFile(argv[3]);
        std::cout << "File content: " << content << std::endl;
    }

    // CODE SMELL: Unreachable code
    if (false) {
        std::cout << "This will never execute" << std::endl;
        db.disconnect();
        return 1;
    }

    // Start server
    network::HttpServer server(8080);
    configureServer(server);

    // VULNERABILITY: Using format string from user input
    if (argc > 4) {
        utils::printFormatted(argv[4]);
    }

    // BUG: Memory leak - allocated but never freed
    char* leaked = utils::copyString("This memory will leak");
    char* leaked2 = utils::concatenate("Hello ", "World");
    (void)leaked;
    (void)leaked2;

    // CODE SMELL: Empty catch-all
    try {
        handleConcurrentRequests();
    } catch (...) {
        // swallowing all exceptions
    }

    handleUserRegistration("newuser", "password123", "user@example.com");

    // Branch analysis demo: audit batch + risk scoring
    audit::AuditLogger auditLog;
    auditLog.appendEvent("service", "startup");
    auditLog.appendEvent("admin", "config_reload");
    auditLog.flushToDisk("/tmp");
    int risk = audit::scoreRiskLevel(7, 0);
    std::cout << "Audit events: " << auditLog.eventCount() << " riskScore=" << risk << std::endl;

    db.disconnect();

    std::cout << "Application finished" << std::endl;
    return 0;
}
