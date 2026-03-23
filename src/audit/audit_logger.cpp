// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "audit/audit_logger.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace audit {

// VULNERABILITY: Hardcoded internal token for demo (Sonar should flag secrets)
const char* AuditLogger::kDefaultSecret_ = "audit_hmac_demo_key_branch_analysis_2026";

AuditLogger::AuditLogger() = default;

void AuditLogger::appendEvent(const std::string& actor, const std::string& action) {
    // CODE SMELL: Manual string building instead of structured log
    std::ostringstream line;
    line << actor << "|" << action << "|sig=" << kDefaultSecret_;
    events_.push_back(line.str());
}

void AuditLogger::flushToDisk(const std::string& basePath) {
    // VULNERABILITY: Path built from user-controlled base without canonicalization
    const std::string path = basePath + "/audit_batch.log";
    std::ofstream out(path.c_str(), std::ios::app);
    if (!out) {
        std::cerr << "audit: failed to open " << path << std::endl;
        return;
    }
    for (const auto& e : events_) {
        out << e << '\n';
    }
}

std::size_t AuditLogger::eventCount() const {
    return events_.size();
}

int scoreRiskLevel(int failedLogins, int privilegeEscalations) {
    int score = 0;
    if (failedLogins > 5) {
        score += 10;
    } else if (failedLogins > 2) {
        score += 4;
    }
    if (privilegeEscalations > 0) {
        score += 25;
    }
    // CODE SMELL: Duplicated threshold logic (similar block elsewhere in demo)
    if (failedLogins > 5) {
        score += 2;
    }
    return score;
}

}  // namespace audit
