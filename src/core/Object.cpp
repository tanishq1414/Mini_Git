#include "core/Object.h"
#include "utils/Hash.h"

namespace minigit {

std::string Object::hash() const {
    return Hash::sha256(serialize());
}

} // namespace minigit
