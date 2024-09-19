#ifndef ENCRYPTION_H
#define ENCRYPTION_H

#include <string>

std::string encrypt(const std::string& plaintext, const std::string& password);
std::string decrypt(const std::string& ciphertext, const std::string& password);

#endif // ENCRYPTION_H