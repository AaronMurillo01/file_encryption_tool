#include "encryption.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>
#include <vector>

const int KEY_SIZE = 32;
const int IV_SIZE = 16;

std::string encrypt(const std::string& plaintext, const std::string& password) {
    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];

    if (!RAND_bytes(iv, IV_SIZE)) {
        throw std::runtime_error("Error generating IV");
    }

    if (EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), NULL,
                       reinterpret_cast<const unsigned char*>(password.c_str()),
                       password.length(), 1, key, NULL) != KEY_SIZE) {
        throw std::runtime_error("Error deriving key");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Error creating cipher context");
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Error initializing encryption");
    }

    std::vector<unsigned char> ciphertext(plaintext.length() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, ciphertext_len = 0;

    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                          reinterpret_cast<const unsigned char*>(plaintext.c_str()),
                          plaintext.length()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Error encrypting data");
    }
    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Error finalizing encryption");
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    std::string result(reinterpret_cast<char*>(iv), IV_SIZE);
    result.append(reinterpret_cast<char*>(ciphertext.data()), ciphertext_len);
    return result;
}

std::string decrypt(const std::string& ciphertext, const std::string& password) {
    if (ciphertext.length() <= IV_SIZE) {
        throw std::runtime_error("Invalid ciphertext");
    }

    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];
    std::copy(ciphertext.begin(), ciphertext.begin() + IV_SIZE, iv);

    if (EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), NULL,
                       reinterpret_cast<const unsigned char*>(password.c_str()),
                       password.length(), 1, key, NULL) != KEY_SIZE) {
        throw std::runtime_error("Error deriving key");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Error creating cipher context");
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Error initializing decryption");
    }

    std::vector<unsigned char> plaintext(ciphertext.length() - IV_SIZE);
    int len = 0, plaintext_len = 0;

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                          reinterpret_cast<const unsigned char*>(ciphertext.c_str() + IV_SIZE),
                          ciphertext.length() - IV_SIZE) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Error decrypting data");
    }
    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Error finalizing decryption");
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string(reinterpret_cast<char*>(plaintext.data()), plaintext_len);
}