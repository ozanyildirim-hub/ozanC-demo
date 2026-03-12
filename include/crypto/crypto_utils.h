// Copyright (c) 2026 Ozan Yildirim - MIT License
#pragma once

#include <string>
#include <vector>

namespace crypto {

std::string md5Hash(const std::string& input);
std::string sha1Hash(const std::string& input);
std::string encryptPassword(const std::string& password);
std::string generateSalt();
std::string hashWithSalt(const std::string& password, const std::string& salt);
std::string encryptAES(const std::string& plaintext, const std::string& key);
std::string decryptAES(const std::string& ciphertext, const std::string& key);
std::string generateRandomToken(int length);
bool verifySignature(const std::string& data, const std::string& signature, const std::string& publicKey);
std::string base64Encode(const std::string& input);
std::string base64Decode(const std::string& input);
void initializeRandom();
int getRandomNumber(int min, int max);
std::string deriveKey(const std::string& password, const std::string& salt);

} // namespace crypto
