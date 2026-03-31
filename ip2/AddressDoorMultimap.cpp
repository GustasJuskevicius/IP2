#include "AddressDoorMultimap.h"

#include <iterator>
#include <map>
#include <sstream>
#include <utility>

namespace customadt {

DuplicateEntryException::DuplicateEntryException(const std::string& message)
    : std::logic_error(message) {
}

class AddressDoorMultimap::Impl {
public:
    std::multimap<std::string, int> entries;
};

void validateEntry(const AddressDoorMultimap::Entry& entry) {
    if (entry.first.empty()) {
        throw std::invalid_argument("Address must not be empty.");
    }

    if (entry.second <= 0) {
        throw std::invalid_argument("Door number must be positive.");
    }
}

bool hasExactEntry(const std::multimap<std::string, int>& entries,
    const AddressDoorMultimap::Entry& target) {
    const auto range = entries.equal_range(target.first);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == target.second) {
            return true;
        }
    }

    return false;
}

std::multimap<std::string, int>::iterator findExactEntry(std::multimap<std::string, int>& entries,
    const AddressDoorMultimap::Entry& target) {
    const auto range = entries.equal_range(target.first);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second == target.second) {
            return it;
        }
    }

    return entries.end();
}

AddressDoorMultimap::AddressDoorMultimap()
    : impl_(std::make_unique<Impl>()) {
}

AddressDoorMultimap::AddressDoorMultimap(const AddressDoorMultimap& other)
    : impl_(std::make_unique<Impl>(*other.impl_)) {
}

AddressDoorMultimap& AddressDoorMultimap::operator=(const AddressDoorMultimap& other) {
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }

    return *this;
}

AddressDoorMultimap::~AddressDoorMultimap() = default;

AddressDoorMultimap& AddressDoorMultimap::operator+=(const Entry& entry) {
    validateEntry(entry);

    if (hasExactEntry(impl_->entries, entry)) {
        throw DuplicateEntryException("Identical address-door pair already exists.");
    }

    impl_->entries.insert(entry);
    return *this;
}

AddressDoorMultimap& AddressDoorMultimap::operator-=(const Entry& entry) {
    validateEntry(entry);

    const auto it = findExactEntry(impl_->entries, entry);
    if (it == impl_->entries.end()) {
        throw std::out_of_range("Cannot remove a non-existent address-door pair.");
    }

    impl_->entries.erase(it);
    return *this;
}

AddressDoorMultimap& AddressDoorMultimap::operator%=(const Edit& edit) {
    validateEntry(edit.first);
    validateEntry(edit.second);

    const auto it = findExactEntry(impl_->entries, edit.first);
    if (it == impl_->entries.end()) {
        throw std::out_of_range("Cannot edit a non-existent address-door pair.");
    }

    const bool sameEntry = edit.first == edit.second;

    if (!sameEntry && hasExactEntry(impl_->entries, edit.second)) {
        throw DuplicateEntryException("Edit would create a duplicate address-door pair.");
    }

    impl_->entries.erase(it);
    impl_->entries.insert(edit.second);
    return *this;
}

AddressDoorMultimap& AddressDoorMultimap::operator!() {
    impl_->entries.clear();
    return *this;
}

std::size_t AddressDoorMultimap::operator[](const std::string& address) const {
    if (address.empty()) {
        throw std::invalid_argument("Address must not be empty.");
    }

    return impl_->entries.count(address);
}

//comparative operatoriai - lexicographical (t.y. b>a, 2>1 paraidziui)

bool AddressDoorMultimap::operator==(const AddressDoorMultimap& other) const {
    return impl_->entries == other.impl_->entries;
}

bool AddressDoorMultimap::operator!=(const AddressDoorMultimap& other) const {
    return !(*this == other);
}

bool AddressDoorMultimap::operator<(const AddressDoorMultimap& other) const {
    return impl_->entries < other.impl_->entries;
}

bool AddressDoorMultimap::operator<=(const AddressDoorMultimap& other) const {
    return !(other < *this);
}

bool AddressDoorMultimap::operator>(const AddressDoorMultimap& other) const {
    return other < *this;
}

bool AddressDoorMultimap::operator>=(const AddressDoorMultimap& other) const {
    return !(*this < other);
}

std::size_t AddressDoorMultimap::size() const noexcept {
    return impl_->entries.size();
}

bool AddressDoorMultimap::empty() const noexcept {
    return impl_->entries.empty();
}

AddressDoorMultimap::Entry AddressDoorMultimap::at(std::size_t index) const {
    if (index >= impl_->entries.size()) {
        throw std::out_of_range("Index is out of range.");
    }

    auto it = impl_->entries.begin();
    std::advance(it, static_cast<long long>(index));
    return Entry(it->first, it->second);
}

std::string AddressDoorMultimap::toString() const {
    std::ostringstream output;
    output << "AddressDoorMultimap{entries=" << impl_->entries.size();

    std::size_t uniqueAddresses = 0;
    std::string previousAddress;
    bool firstSeen = true;

    for (const auto& pair : impl_->entries) {
        if (firstSeen || pair.first != previousAddress) {
            ++uniqueAddresses;
            previousAddress = pair.first;
            firstSeen = false;
        }
    }

    output << ", uniqueAddresses=" << uniqueAddresses << ", data=[";

    bool firstAddress = true;
    for (auto it = impl_->entries.begin(); it != impl_->entries.end();) {
        if (!firstAddress) {
            output << "; ";
        }

        firstAddress = false;
        const std::string currentAddress = it->first;
        output << currentAddress << ": ";

        bool firstDoor = true;
        const auto range = impl_->entries.equal_range(currentAddress);
        for (auto current = range.first; current != range.second; ++current) {
            if (!firstDoor) {
                output << ", ";
            }

            firstDoor = false;
            output << current->second;
        }

        it = range.second;
    }

    output << "]}";
    return output.str();
}

}