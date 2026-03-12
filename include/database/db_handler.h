// Copyright (c) 2026 Ozan Yildirim - MIT License
#pragma once

#include <string>
#include <vector>
#include <map>

namespace database {

struct QueryResult {
    std::vector<std::map<std::string, std::string>> rows;
    int affectedRows;
    bool success;
    std::string errorMessage;
};

class DbHandler {
public:
    DbHandler();
    DbHandler(const std::string& connectionString);
    ~DbHandler();

    bool connect(const std::string& host, int port, const std::string& dbName,
                 const std::string& user, const std::string& password);
    void disconnect();
    QueryResult executeQuery(const std::string& query);
    QueryResult findUser(const std::string& username);
    QueryResult findUserByEmail(const std::string& email);
    bool insertUser(const std::string& username, const std::string& password, const std::string& email);
    bool deleteUser(const std::string& userId);
    bool updateUserField(const std::string& userId, const std::string& field, const std::string& value);
    std::string buildSearchQuery(const std::string& table, const std::string& searchTerm);
    QueryResult runReport(const std::string& reportType, const std::string& dateRange);
    void backupDatabase(const std::string& outputPath);

private:
    std::string connectionString_;
    bool connected_;
    void* dbConnection_;
    std::string escapeString(const std::string& input);
    void logQuery(const std::string& query);
};

} // namespace database
