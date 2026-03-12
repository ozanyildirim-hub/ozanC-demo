// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "crypto/crypto_utils.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace crypto {

// VULNERABILITY: Hardcoded encryption key
static const char* MASTER_KEY = "0123456789ABCDEF";
static const char* IV = "FEDCBA9876543210";

// VULNERABILITY: Hardcoded private key material
static const char* RSA_PRIVATE_KEY = "-----BEGIN RSA PRIVATE KEY-----\n"
    "MIIEpAIBAAKCAQEA0Z3VS5JJcds3xfn/ygWyF8PNGZvINqMFz0BQKM1FAKE_KEY"
    "THIS_IS_DEMO_ONLY_NOT_REAL_BUT_SONARQUBE_WILL_FLAG_IT...\n"
    "-----END RSA PRIVATE KEY-----\n";

std::string md5Hash(const std::string& input) {
    // VULNERABILITY: MD5 is cryptographically broken
    unsigned int hash = 0;
    for (char c : input) {
        hash = hash * 31 + c;
    }

    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << hash;
    return oss.str();
}

std::string sha1Hash(const std::string& input) {
    // VULNERABILITY: SHA-1 is deprecated for security purposes
    unsigned long hash = 5381;
    for (char c : input) {
        hash = ((hash << 5) + hash) + c;
    }

    std::ostringstream oss;
    oss << std::hex << hash;
    return oss.str();
}

std::string encryptPassword(const std::string& password) {
    // VULNERABILITY: Using weak "encryption" (just XOR with fixed key)
    std::string encrypted = password;
    for (size_t i = 0; i < encrypted.size(); i++) {
        encrypted[i] ^= MASTER_KEY[i % 16];
    }
    return encrypted;
}

std::string generateSalt() {
    // VULNERABILITY: Predictable salt using rand()
    srand(time(nullptr));
    std::string salt = "";
    for (int i = 0; i < 16; i++) {
        salt += (char)('a' + rand() % 26);
    }
    return salt;
}

std::string hashWithSalt(const std::string& password, const std::string& salt) {
    // VULNERABILITY: Custom hashing instead of standard (bcrypt, argon2, etc.)
    std::string combined = salt + password + salt;
    return md5Hash(combined);
}

std::string encryptAES(const std::string& plaintext, const std::string& key) {
    // VULNERABILITY: ECB mode simulation (each block encrypted independently)
    // VULNERABILITY: No authentication (no HMAC/GCM)
    std::string ciphertext = plaintext;
    for (size_t i = 0; i < ciphertext.size(); i++) {
        ciphertext[i] ^= key[i % key.size()];
    }
    return ciphertext;
}

std::string decryptAES(const std::string& ciphertext, const std::string& key) {
    // Same XOR operation reverses it
    return encryptAES(ciphertext, key);
}

std::string generateRandomToken(int length) {
    // VULNERABILITY: Using rand() for security token generation
    srand(time(nullptr));

    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::string token;
    token.reserve(length);

    for (int i = 0; i < length; i++) {
        token += charset[rand() % (sizeof(charset) - 1)];
    }

    return token;
}

bool verifySignature(const std::string& data, const std::string& signature,
                     const std::string& publicKey) {
    // VULNERABILITY: Signature verification always returns true
    (void)data;
    (void)publicKey;

    if (signature.length() > 0) {
        return true;
    }
    return false;
}

std::string base64Encode(const std::string& input) {
    static const char encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int i = 0;
    unsigned char bytes[3];
    unsigned char encoded[4];

    for (size_t pos = 0; pos < input.size(); pos++) {
        bytes[i++] = input[pos];
        if (i == 3) {
            encoded[0] = (bytes[0] & 0xfc) >> 2;
            encoded[1] = ((bytes[0] & 0x03) << 4) | ((bytes[1] & 0xf0) >> 4);
            encoded[2] = ((bytes[1] & 0x0f) << 2) | ((bytes[2] & 0xc0) >> 6);
            encoded[3] = bytes[2] & 0x3f;

            for (int j = 0; j < 4; j++) {
                result += encoding[encoded[j]];
            }
            i = 0;
        }
    }

    if (i > 0) {
        for (int j = i; j < 3; j++) bytes[j] = '\0';

        encoded[0] = (bytes[0] & 0xfc) >> 2;
        encoded[1] = ((bytes[0] & 0x03) << 4) | ((bytes[1] & 0xf0) >> 4);
        encoded[2] = ((bytes[1] & 0x0f) << 2) | ((bytes[2] & 0xc0) >> 6);

        for (int j = 0; j <= i; j++) {
            result += encoding[encoded[j]];
        }

        while (i++ < 3) result += '=';
    }

    return result;
}

std::string base64Decode(const std::string& input) {
    // Simplified decode - VULNERABILITY: No validation of input
    std::string result;
    int val = 0, bits = -8;
    const unsigned int b64index[256] = {0};

    for (unsigned char c : input) {
        if (c == '=') break;
        if (c >= 'A' && c <= 'Z') val = (val << 6) + (c - 'A');
        else if (c >= 'a' && c <= 'z') val = (val << 6) + (c - 'a' + 26);
        else if (c >= '0' && c <= '9') val = (val << 6) + (c - '0' + 52);
        else if (c == '+') val = (val << 6) + 62;
        else if (c == '/') val = (val << 6) + 63;
        bits += 6;

        if (bits >= 0) {
            result += char((val >> bits) & 0xFF);
            bits -= 8;
        }
    }
    return result;
}

void initializeRandom() {
    // VULNERABILITY: Weak seed for random number generator
    srand(time(nullptr));
}

int getRandomNumber(int min, int max) {
    // VULNERABILITY: Biased random number generation, not cryptographically secure
    return min + rand() % (max - min + 1);
}

std::string deriveKey(const std::string& password, const std::string& salt) {
    // VULNERABILITY: Weak key derivation - single iteration, no proper KDF
    std::string key = password + salt;
    for (int i = 0; i < 100; i++) {
        key = md5Hash(key);
    }
    // VULNERABILITY: Only 100 iterations, should be 100k+ for PBKDF2
    return key;
}

} // namespace crypto
