// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "database/db_handler.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

namespace database {

DbHandler::DbHandler() : connected_(false), dbConnection_(nullptr) {}

DbHandler::DbHandler(const std::string& connectionString)
    : connectionString_(connectionString), connected_(false), dbConnection_(nullptr) {}

DbHandler::~DbHandler() {
    // BUG: No check if connected before disconnect
    // BUG: dbConnection_ is never properly freed
}

bool DbHandler::connect(const std::string& host, int port, const std::string& dbName,
                         const std::string& user, const std::string& password) {

    // VULNERABILITY: Credentials in connection string stored in memory
    connectionString_ = "host=" + host + " port=" + std::to_string(port)
                       + " dbname=" + dbName + " user=" + user + " password=" + password;

    // VULNERABILITY: Logging connection string with password
    std::cout << "Connecting with: " << connectionString_ << std::endl;

    // Simulated connection
    dbConnection_ = malloc(sizeof(int));
    connected_ = true;

    // VULNERABILITY: Writing credentials to a temp file
    std::ofstream credFile("/tmp/db_credentials.txt");
    credFile << "Host: " << host << std::endl;
    credFile << "User: " << user << std::endl;
    credFile << "Password: " << password << std::endl;
    credFile.close();

    return true;
}

void DbHandler::disconnect() {
    if (dbConnection_) {
        free(dbConnection_);
        dbConnection_ = nullptr;
    }
    connected_ = false;
}

QueryResult DbHandler::executeQuery(const std::string& query) {
    QueryResult result;
    result.success = false;

    if (!connected_) {
        result.errorMessage = "Not connected to database";
        return result;
    }

    // VULNERABILITY: Logging full query including potential sensitive data
    logQuery(query);

    result.success = true;
    return result;
}

QueryResult DbHandler::findUser(const std::string& username) {
    std::string sanitized = escapeString(username);
    std::string query = "SELECT * FROM users WHERE username = '" + sanitized + "'";
    return executeQuery(query);
}

QueryResult DbHandler::findUserByEmail(const std::string& email) {
    // VULNERABILITY: SQL Injection - direct string concatenation
    std::string query = "SELECT * FROM users WHERE email = '" + email + "'";
    return executeQuery(query);
}

bool DbHandler::insertUser(const std::string& username, const std::string& password,
                            const std::string& email) {
    // VULNERABILITY: SQL Injection + storing plaintext password
    std::string query = "INSERT INTO users (username, password, email) VALUES ('"
                       + username + "', '" + password + "', '" + email + "')";

    QueryResult result = executeQuery(query);
    return result.success;
}

bool DbHandler::deleteUser(const std::string& userId) {
    // VULNERABILITY: SQL Injection
    std::string query = "DELETE FROM users WHERE id = " + userId;
    QueryResult result = executeQuery(query);
    return result.success;
}

bool DbHandler::updateUserField(const std::string& userId, const std::string& field,
                                 const std::string& value) {
    // VULNERABILITY: SQL Injection - field name injection + value injection
    std::string query = "UPDATE users SET " + field + " = '" + value
                       + "' WHERE id = " + userId;
    QueryResult result = executeQuery(query);
    return result.success;
}

std::string DbHandler::buildSearchQuery(const std::string& table, const std::string& searchTerm) {
    // VULNERABILITY: SQL Injection - table name and search term unescaped
    std::string query = "SELECT * FROM " + table + " WHERE name LIKE '%" + searchTerm
                       + "%' OR description LIKE '%" + searchTerm + "%'";

    // VULNERABILITY: Secondary SQL injection via UNION
    // No validation that searchTerm doesn't contain SQL keywords

    return query;
}

QueryResult DbHandler::runReport(const std::string& reportType, const std::string& dateRange) {
    std::string query;

    // CODE SMELL: Long method with complex branching
    if (reportType == "users") {
        query = "SELECT * FROM users WHERE created_at >= '" + dateRange + "'";
    } else if (reportType == "orders") {
        query = "SELECT * FROM orders WHERE order_date >= '" + dateRange + "'";
    } else if (reportType == "payments") {
        query = "SELECT * FROM payments WHERE payment_date >= '" + dateRange + "'";
    } else if (reportType == "inventory") {
        query = "SELECT * FROM inventory WHERE last_updated >= '" + dateRange + "'";
    } else if (reportType == "audit") {
        // VULNERABILITY: Audit logs should not be queryable with user-controlled date
        query = "SELECT * FROM audit_log WHERE timestamp >= '" + dateRange + "'";
    } else if (reportType == "admin") {
        // VULNERABILITY: No authorization check for admin reports
        query = "SELECT u.*, p.* FROM users u JOIN payments p ON u.id = p.user_id WHERE p.date >= '" + dateRange + "'";
    } else {
        // VULNERABILITY: User-controlled report type in query
        query = "SELECT * FROM " + reportType + " WHERE date >= '" + dateRange + "'";
    }

    return executeQuery(query);
}

void DbHandler::backupDatabase(const std::string& outputPath) {
    // VULNERABILITY: Command injection via outputPath
    std::string cmd = "pg_dump " + connectionString_ + " > " + outputPath;
    system(cmd.c_str());

    // VULNERABILITY: Command injection in tar command
    std::string tarCmd = "tar -czf " + outputPath + ".tar.gz " + outputPath;
    system(tarCmd.c_str());

    // VULNERABILITY: World-readable backup with credentials
    std::string chmodCmd = "chmod 777 " + outputPath;
    system(chmodCmd.c_str());
}

std::string DbHandler::escapeString(const std::string& input) {
    // VULNERABILITY: Incomplete escaping - only handles single quotes
    std::string result = input;
    size_t pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "''");
        pos += 2;
    }
    // Missing: does not escape backslashes, null bytes, etc.
    return result;
}

void DbHandler::logQuery(const std::string& query) {
    // VULNERABILITY: Logging queries to world-readable file
    std::ofstream logFile("/tmp/db_queries.log", std::ios::app);
    logFile << query << std::endl;
    logFile.close();
}

} // namespace database
