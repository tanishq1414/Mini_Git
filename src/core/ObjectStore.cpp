#include "core/ObjectStore.h"
#include "utils/FileSystem.h"
#include "utils/Hash.h"
#include "utils/Exceptions.h"
#include <fstream>
#include <stdexcept>

namespace minigit {

ObjectStore::ObjectStore(const std::filesystem::path& objectsDir)
    : m_dir(objectsDir) {
    FileSystem::ensureDir(m_dir);
}

std::filesystem::path ObjectStore::objectPath(const std::string& hash) const {
    return m_dir / hash.substr(0, 2) / hash.substr(2);
}

std::string ObjectStore::pack(ObjectType t, const std::string& content) const {
    return objectTypeName(t) + "\n" + content;
}

std::pair<ObjectType, std::string> ObjectStore::unpack(const std::string& raw) const {
    auto nl = raw.find('\n');
    if (nl == std::string::npos)
        throw std::runtime_error("invalid object format");
    auto typeName = raw.substr(0, nl);
    auto content  = raw.substr(nl + 1);
    return {objectTypeFromName(typeName), content};
}

std::string ObjectStore::store(const Object& obj) {
    auto serialized = obj.serialize();
    auto h = Hash::sha256(serialized);
    auto p = objectPath(h);
    if (!std::filesystem::exists(p)) {
        FileSystem::ensureDir(p.parent_path());
        FileSystem::writeFileAtomic(p, serialized);
    }
    return h;
}

std::string ObjectStore::storeRaw(const std::string& serialized, ObjectType /*t*/) {
    auto h = Hash::sha256(serialized);
    auto p = objectPath(h);
    if (!std::filesystem::exists(p)) {
        FileSystem::ensureDir(p.parent_path());
        FileSystem::writeFileAtomic(p, serialized);
    }
    return h;
}

bool ObjectStore::exists(const std::string& hash) const {
    return std::filesystem::exists(objectPath(hash));
}

AnyObject ObjectStore::load(const std::string& hash) const {
    auto p = objectPath(hash);
    if (!std::filesystem::exists(p))
        throw ObjectNotFoundException(hash);
    auto raw = FileSystem::readFile(p);
    auto nl  = raw.find('\n');
    if (nl == std::string::npos)
        throw ObjectCorruptedException(hash);
    auto typeName = raw.substr(0, nl);
    ObjectType t;
    try { t = objectTypeFromName(typeName); }
    catch (...) { throw ObjectCorruptedException(hash); }

    switch (t) {
        case ObjectType::Blob:   return Blob::deserialize(raw);
        case ObjectType::Tree:   return Tree::deserialize(raw);
        case ObjectType::Commit: return Commit::deserialize(raw);
    }
    throw ObjectCorruptedException(hash);
}

Blob ObjectStore::loadBlob(const std::string& hash) const {
    auto obj = load(hash);
    if (auto* b = std::get_if<Blob>(&obj)) return *b;
    throw ObjectCorruptedException(hash);
}

Tree ObjectStore::loadTree(const std::string& hash) const {
    auto obj = load(hash);
    if (auto* t = std::get_if<Tree>(&obj)) return *t;
    throw ObjectCorruptedException(hash);
}

Commit ObjectStore::loadCommit(const std::string& hash) const {
    auto obj = load(hash);
    if (auto* c = std::get_if<Commit>(&obj)) return *c;
    throw ObjectCorruptedException(hash);
}

std::string ObjectStore::resolveAbbrev(const std::string& prefix) const {
    if (prefix.size() < 2) return "";
    auto dir = m_dir / prefix.substr(0, 2);
    if (!std::filesystem::exists(dir)) return "";
    auto rest = prefix.substr(2);
    std::vector<std::string> matches;
    for (auto& e : std::filesystem::directory_iterator(dir)) {
        auto name = e.path().filename().string();
        if (name.starts_with(rest))
            matches.push_back(prefix.substr(0, 2) + name);
    }
    if (matches.size() == 1) return matches[0];
    if (matches.size() > 1) throw std::runtime_error("ambiguous hash prefix: " + prefix);
    return "";
}

} // namespace minigit
