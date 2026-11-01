#pragma once
#include <stdexcept>
#include <string>

namespace minigit {

class MiniGitException : public std::runtime_error {
public:
    explicit MiniGitException(const std::string& msg) : std::runtime_error(msg) {}
};

class NotARepositoryException : public MiniGitException {
public:
    NotARepositoryException()
        : MiniGitException("not a minigit repository (or any of the parent directories): .minigit") {}
};

class ObjectNotFoundException : public MiniGitException {
public:
    explicit ObjectNotFoundException(const std::string& hash)
        : MiniGitException("object not found: " + hash) {}
};

class ObjectCorruptedException : public MiniGitException {
public:
    explicit ObjectCorruptedException(const std::string& hash)
        : MiniGitException("corrupt object: " + hash) {}
};

class IndexException : public MiniGitException {
public:
    explicit IndexException(const std::string& msg) : MiniGitException("index error: " + msg) {}
};

class CommandException : public MiniGitException {
public:
    explicit CommandException(const std::string& msg) : MiniGitException(msg) {}
};

class PathException : public MiniGitException {
public:
    explicit PathException(const std::string& msg) : MiniGitException(msg) {}
};

class ConfigException : public MiniGitException {
public:
    explicit ConfigException(const std::string& msg) : MiniGitException("config error: " + msg) {}
};

} // namespace minigit
