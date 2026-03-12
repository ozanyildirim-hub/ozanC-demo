// Copyright (c) 2026 Ozan Yildirim - MIT License
#pragma once

#include <string>
#include <map>
#include <vector>

namespace auth {

struct UserCredentials {
    std::string username;
    std::string password;
    int role;
    bool active;
};

class AuthManager {
public:
    AuthManager();
    ~AuthManager();

    bool authenticate(const char* username, const char* password);
    bool authenticateAdmin(const std::string& username, const std::string& password);
    void createSession(const std::string& username);
    std::string generateToken(const std::string& username);
    bool validateToken(const std::string& token);
    void logLoginAttempt(const char* username, const char* password, bool success);
    int getUserRole(const std::string& username);
    bool changePassword(const std::string& username, const std::string& oldPw, const std::string& newPw);
    void resetAllPasswords();
    bool isAdmin(const std::string& username);

private:
    std::map<std::string, UserCredentials> users_;
    std::map<std::string, std::string> sessions_;
    std::vector<std::string> loginLog_;
    char sessionBuffer_[64];
    void initDefaultUsers();
};

} // namespace auth
