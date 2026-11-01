#include "utils/FileSystem.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iterator>

namespace minigit {

std::string FileSystem::readFile(const std::filesystem::path& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("cannot read file: " + path.string());
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::vector<uint8_t> FileSystem::readFileBytes(const std::filesystem::path& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("cannot read file: " + path.string());
    return {std::istreambuf_iterator<char>(f), {}};
}

void FileSystem::writeFileAtomic(const std::filesystem::path& path, const std::string& content) {
    ensureDir(path.parent_path());
    auto tmp = path;
    tmp += ".tmp";
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f) throw std::runtime_error("cannot write file: " + tmp.string());
        f << content;
    }
    std::filesystem::rename(tmp, path);
}

void FileSystem::writeFileBytesAtomic(const std::filesystem::path& path,
                                       const std::vector<uint8_t>& data) {
    ensureDir(path.parent_path());
    auto tmp = path;
    tmp += ".tmp";
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f) throw std::runtime_error("cannot write file: " + tmp.string());
        f.write(reinterpret_cast<const char*>(data.data()),
                static_cast<std::streamsize>(data.size()));
    }
    std::filesystem::rename(tmp, path);
}

void FileSystem::ensureDir(const std::filesystem::path& path) {
    if (!path.empty())
        std::filesystem::create_directories(path);
}

std::vector<std::filesystem::path> FileSystem::listFilesRecursive(
    const std::filesystem::path& root,
    const std::function<bool(const std::filesystem::path&)>& filter)
{
    std::vector<std::filesystem::path> result;
    if (!std::filesystem::exists(root)) return result;
    for (auto& entry : std::filesystem::recursive_directory_iterator(root,
            std::filesystem::directory_options::skip_permission_denied)) {
        if (!entry.is_regular_file()) continue;
        if (!filter || filter(entry.path()))
            result.push_back(entry.path());
    }
    return result;
}

bool FileSystem::isInsideDirectory(const std::filesystem::path& path,
                                    const std::filesystem::path& root) {
    auto rel = std::filesystem::relative(path, root);
    auto s = rel.string();
    return !s.empty() && !s.starts_with("..");
}

std::filesystem::path FileSystem::normalize(const std::filesystem::path& path) {
    return std::filesystem::absolute(path).lexically_normal();
}

std::filesystem::path FileSystem::makeRelative(const std::filesystem::path& path,
                                                const std::filesystem::path& base) {
    auto rel = std::filesystem::relative(path, base);
    return rel;
}

std::vector<std::string> FileSystem::splitLines(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        // Strip Windows \r
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        lines.push_back(line);
    }
    return lines;
}

} // namespace minigit
