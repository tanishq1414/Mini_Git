#pragma once
#include <string>
#include <string_view>
#include <filesystem>
#include <vector>

namespace minigit {

/// SHA-256 hashing utilities using OpenSSL EVP API.
class Hash {
public:
    /// Compute SHA-256 of raw bytes, returns lowercase hex string (64 chars).
    static std::string sha256(std::string_view data);

    /// Compute SHA-256 of a file's content.
    static std::string sha256File(const std::filesystem::path& path);

    /// Compute SHA-256 of bytes from a vector.
    static std::string sha256(const std::vector<uint8_t>& data);

    /// Returns first n characters of a hash (abbreviated hash).
    static std::string abbreviate(std::string_view fullHash, size_t len = 8);

    /// Check if a string looks like a valid SHA-256 hex string (exactly 64 chars).
    static bool isValidHash(std::string_view hash);
};

} // namespace minigit
