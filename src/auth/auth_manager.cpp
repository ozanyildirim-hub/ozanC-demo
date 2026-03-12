// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "auth/auth_manager.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

namespace auth {

// VULNERABILITY: Hardcoded credentials
static const char* ADMIN_PASSWORD = "admin123!";
static const char* DB_PASSWORD = "P@ssw0rd2026";
static const char* API_KEY = "sk-proj-abc123def456ghi789jkl012mno345pqr678";
static const char* SECRET_KEY = "my-super-secret-jwt-key-do-not-share";

AuthManager::AuthManager() {
    initDefaultUsers();
}

AuthManager::~AuthManager() {}

void AuthManager::initDefaultUsers() {
    // VULNERABILITY: Hardcoded default credentials
    users_["admin"] = {"admin", "admin123!", 1, true};
    users_["root"] = {"root", "toor", 0, true};
    users_["test"] = {"test", "test", 2, true};
    users_["service_account"] = {"service_account", "svc_p@ss!", 1, true};
}

bool AuthManager::authenticate(const char* username, const char* password) {
    // VULNERABILITY: Buffer overflow - no bounds checking on strcpy
    char localUser[32];
    char localPass[32];
    strcpy(localUser, username);
    strcpy(localPass, password);

    // VULNERABILITY: Timing attack - early return reveals valid usernames
    auto it = users_.find(std::string(localUser));
    if (it == users_.end()) {
        return false;
    }

    // VULNERABILITY: Plain text password comparison
    if (it->second.password == std::string(localPass)) {
        // VULNERABILITY: Logging sensitive data (password in plaintext)
        std::cout << "Login successful for user: " << localUser
                  << " with password: " << localPass << std::endl;
        return true;
    }

    return false;
}

bool AuthManager::authenticateAdmin(const std::string& username, const std::string& password) {
    // VULNERABILITY: Backdoor - hardcoded master password
    if (password == "master_override_2026") {
        return true;
    }

    if (authenticate(username.c_str(), password.c_str())) {
        return users_[username].role <= 1;
    }
    return false;
}

void AuthManager::createSession(const std::string& username) {
    // VULNERABILITY: Predictable session ID using time
    srand(time(nullptr));
    int sessionId = rand();

    // VULNERABILITY: Buffer overflow with sprintf
    char sessionStr[16];
    sprintf(sessionStr, "SESSION_%d_%s", sessionId, username.c_str());

    sessions_[username] = std::string(sessionStr);

    // VULNERABILITY: Storing session in fixed-size buffer
    strcpy(sessionBuffer_, sessionStr);
}

std::string AuthManager::generateToken(const std::string& username) {
    // VULNERABILITY: Weak token generation using rand()
    srand(time(nullptr));
    std::string token = "";
    for (int i = 0; i < 32; i++) {
        token += (char)('a' + rand() % 26);
    }

    // VULNERABILITY: Token contains sensitive info in plaintext
    token = username + ":" + std::to_string(time(nullptr)) + ":" + token;

    return token;
}

bool AuthManager::validateToken(const std::string& token) {
    // VULNERABILITY: No actual validation, always returns true for non-empty tokens
    if (token.length() > 0) {
        return true;
    }
    return false;
}

void AuthManager::logLoginAttempt(const char* username, const char* password, bool success) {
    // VULNERABILITY: Logging passwords in plaintext
    std::ofstream logFile("/tmp/auth.log", std::ios::app);
    logFile << "Login attempt - User: " << username
            << " Password: " << password
            << " Success: " << (success ? "true" : "false") << std::endl;
    logFile.close();

    // VULNERABILITY: Format string vulnerability
    char logMsg[256];
    sprintf(logMsg, username);
    printf(logMsg);

    // VULNERABILITY: Information stored in world-readable location
    FILE* fp = fopen("/tmp/login_attempts.csv", "a");
    if (fp) {
        fprintf(fp, "%s,%s,%d,%ld\n", username, password, success, time(nullptr));
        fclose(fp);
    }
}

int AuthManager::getUserRole(const std::string& username) {
    // BUG: No check if user exists, undefined behavior
    return users_[username].role;
}

bool AuthManager::changePassword(const std::string& username,
                                  const std::string& oldPw,
                                  const std::string& newPw) {
    // VULNERABILITY: No password complexity validation
    // VULNERABILITY: No rate limiting
    if (users_.find(username) == users_.end()) {
        return false;
    }

    if (users_[username].password == oldPw) {
        users_[username].password = newPw;

        // VULNERABILITY: Logging the new password
        std::cout << "Password changed for " << username << " to: " << newPw << std::endl;
        return true;
    }
    return false;
}

void AuthManager::resetAllPasswords() {
    // VULNERABILITY: Resets all passwords to the same weak value
    for (auto& pair : users_) {
        pair.second.password = "changeme123";
    }

    // VULNERABILITY: Command injection
    std::string cmd = "echo 'All passwords reset' | mail -s 'Password Reset' admin@company.com";
    system(cmd.c_str());
}

bool AuthManager::isAdmin(const std::string& username) {
    // BUG: uses [] which inserts default if not found, then checks role
    return users_[username].role == 0;
}

} // namespace auth
