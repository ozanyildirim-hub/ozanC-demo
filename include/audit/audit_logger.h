// Copyright (c) 2026 Ozan Yildirim - MIT License
#pragma once

#include <string>
#include <vector>

namespace audit {

// Batch audit trail for Sonar branch analysis demo (intentional issues for SQ).
class AuditLogger {
public:
    AuditLogger();
    void appendEvent(const std::string& actor, const std::string& action);
    void flushToDisk(const std::string& basePath);
    std::size_t eventCount() const;

private:
    std::vector<std::string> events_;
    static const char* kDefaultSecret_;
};

// CODE SMELL: Free function with magic numbers for demo
int scoreRiskLevel(int failedLogins, int privilegeEscalations);

}  // namespace audit
