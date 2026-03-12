// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "auth/auth_manager.h"
#include "database/db_handler.h"
#include "utils/string_utils.h"
#include "crypto/crypto_utils.h"
#include "third_party/gpl_library.h"

#include <iostream>
#include <cassert>
#include <cstring>

// CODE SMELL: Test code with hardcoded credentials
static const char* TEST_ADMIN_USER = "admin";
static const char* TEST_ADMIN_PASS = "admin123!";

int testsPassed = 0;
int testsFailed = 0;

void testResult(const std::string& testName, bool passed) {
    if (passed) {
        std::cout << "[PASS] " << testName << std::endl;
        testsPassed++;
    } else {
        std::cout << "[FAIL] " << testName << std::endl;
        testsFailed++;
    }
}

void testAuthManager() {
    auth::AuthManager auth;

    testResult("Auth: valid admin login",
               auth.authenticate(TEST_ADMIN_USER, TEST_ADMIN_PASS));

    testResult("Auth: invalid password",
               !auth.authenticate("admin", "wrongpass"));

    testResult("Auth: unknown user",
               !auth.authenticate("nobody", "pass123"));

    // VULNERABILITY: Testing backdoor as a feature
    testResult("Auth: admin backdoor access",
               auth.authenticateAdmin("anyone", "master_override_2026"));

    testResult("Auth: token generation",
               auth.generateToken("admin").length() > 0);

    // BUG: This test always passes because validateToken is broken
    testResult("Auth: token validation",
               auth.validateToken("any_string_works"));
}

void testDatabase() {
    database::DbHandler db;

    testResult("DB: connect",
               db.connect("localhost", 5432, "test", "user", "pass"));

    // VULNERABILITY: Test demonstrates SQL injection
    database::QueryResult result = db.findUser("admin' OR '1'='1");
    testResult("DB: SQL injection query executes", result.success);

    testResult("DB: insert user",
               db.insertUser("testuser", "plaintext_password", "test@test.com"));

    std::string searchQuery = db.buildSearchQuery("users", "test%' UNION SELECT * FROM passwords --");
    testResult("DB: search query builds",
               searchQuery.length() > 0);

    db.disconnect();
}

void testStringUtils() {
    // VULNERABILITY: Buffer overflow in test
    char* str = utils::copyString("Hello, World!");
    testResult("String: copyString", strcmp(str, "Hello, World!") == 0);
    free(str);

    char* concat = utils::concatenate("Hello, ", "World!");
    testResult("String: concatenate", strcmp(concat, "Hello, World!") == 0);
    free(concat);

    auto parts = utils::split("a,b,c", ',');
    testResult("String: split", parts.size() == 3);

    testResult("String: trim", utils::trim("  hello  ") == "hello");

    testResult("String: toUpper", utils::toUpper("hello") == "HELLO");

    testResult("String: toLower", utils::toLower("HELLO") == "hello");

    testResult("String: parseInt", utils::parseInt("42") == 42);

    // CODE SMELL: Commented-out test
    // testResult("String: parseInt overflow", utils::parseInt("99999999999999") == ???);
}

void testCrypto() {
    crypto::initializeRandom();

    std::string hash = crypto::md5Hash("password");
    testResult("Crypto: MD5 hash non-empty", hash.length() > 0);

    std::string sha1 = crypto::sha1Hash("password");
    testResult("Crypto: SHA1 hash non-empty", sha1.length() > 0);

    std::string encrypted = crypto::encryptPassword("secret");
    testResult("Crypto: password encryption", encrypted != "secret");

    std::string decrypted = crypto::encryptPassword(encrypted);
    testResult("Crypto: password round-trip", decrypted == "secret");

    std::string token = crypto::generateRandomToken(32);
    testResult("Crypto: random token length", token.length() == 32);

    // VULNERABILITY: Signature verification always passes
    testResult("Crypto: signature verification (always true)",
               crypto::verifySignature("data", "fake_sig", "fake_key"));

    std::string encoded = crypto::base64Encode("Hello");
    testResult("Crypto: base64 encode non-empty", encoded.length() > 0);

    std::string key = crypto::deriveKey("password", "salt");
    testResult("Crypto: key derivation", key.length() > 0);
}

void testCompression() {
    gpl_compress::DataCompressor compressor;

    std::string original = "AAABBBCCCDDD";
    std::string compressed = compressor.compress(original);
    testResult("Compress: output differs from input", compressed != original);

    std::string decompressed = compressor.decompress(compressed);
    testResult("Compress: round-trip", decompressed == original);

    std::string quick = gpl_compress::quickCompress("test data");
    testResult("Compress: quick compress", quick.length() > 0);

    std::string quickD = gpl_compress::quickDecompress(quick);
    testResult("Compress: quick decompress round-trip", quickD == "test data");
}

int main() {
    std::cout << "=== Running Tests ===" << std::endl << std::endl;

    testAuthManager();
    std::cout << std::endl;

    testDatabase();
    std::cout << std::endl;

    testStringUtils();
    std::cout << std::endl;

    testCrypto();
    std::cout << std::endl;

    testCompression();
    std::cout << std::endl;

    std::cout << "=== Results ===" << std::endl;
    std::cout << "Passed: " << testsPassed << std::endl;
    std::cout << "Failed: " << testsFailed << std::endl;
    std::cout << "Total:  " << (testsPassed + testsFailed) << std::endl;

    // BUG: Always returns 0 even if tests fail
    return 0;
}
