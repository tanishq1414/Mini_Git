#include "utils/Hash.h"
#include "utils/FileSystem.h"
#include <openssl/evp.h>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace minigit {

static std::string bytesToHex(const unsigned char* buf, unsigned int len) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (unsigned int i = 0; i < len; ++i)
        oss << std::setw(2) << static_cast<unsigned>(buf[i]);
    return oss.str();
}

static std::string computeSha256(const void* data, size_t size) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLen = 0;

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx, data, size) != 1 ||
        EVP_DigestFinal_ex(ctx, digest, &digestLen) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("SHA-256 computation failed");
    }
    EVP_MD_CTX_free(ctx);
    return bytesToHex(digest, digestLen);
}

std::string Hash::sha256(std::string_view data) {
    return computeSha256(data.data(), data.size());
}

std::string Hash::sha256(const std::vector<uint8_t>& data) {
    return computeSha256(data.data(), data.size());
}

std::string Hash::sha256File(const std::filesystem::path& path) {
    auto content = FileSystem::readFile(path);
    return sha256(content);
}

std::string Hash::abbreviate(std::string_view fullHash, size_t len) {
    return std::string(fullHash.substr(0, len));
}

bool Hash::isValidHash(std::string_view hash) {
    if (hash.size() != 64) return false;
    for (char c : hash) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')))
            return false;
    }
    return true;
}

} // namespace minigit
