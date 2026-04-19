#include "AddressDoorMultimap.h"

#include <sstream>
#include <utility>

namespace Customadt {

DuplicateEntryException::DuplicateEntryException(const std::string& message)
    : std::logic_error(message) {
}

class AddressDoorMultimap::Impl {
public:
    Impl()
        : data_(nullptr), size_(0), capacity_(0) {
    }

    Impl(const Impl& other)
        : data_(nullptr), size_(0), capacity_(0) {
        reserve(other.size_);
        for (std::size_t i = 0; i < other.size_; ++i) {
            data_[i] = other.data_[i];
        }
        size_ = other.size_;
    }

    Impl& operator=(const Impl& other) {
        if (this != &other) {
            Impl copy(other);
            swap(copy);
        }
        return *this;
    }

    ~Impl() {
        delete[] data_;
    }

    void insertAt(std::size_t index, const AddressDoorMultimap::Entry& entry) {
        reserve(size_ + 1);
        for (std::size_t i = size_; i > index; --i) {
            data_[i] = data_[i - 1];
        }
        data_[index] = entry;
        ++size_;
    }

    void eraseAt(std::size_t index) {
        for (std::size_t i = index; i + 1 < size_; ++i) {
            data_[i] = data_[i + 1];
        }
        --size_;
    }

    void clear() noexcept {
        size_ = 0;
    }

    const AddressDoorMultimap::Entry& operator[](std::size_t index) const {
        return data_[index];
    }

    AddressDoorMultimap::Entry& operator[](std::size_t index) {
        return data_[index];
    }

    std::size_t size() const noexcept {
        return size_;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    static int compareEntries(const AddressDoorMultimap::Entry& left,
        const AddressDoorMultimap::Entry& right);

    std::size_t lowerBoundEntry(const AddressDoorMultimap::Entry& target) const;
    std::size_t lowerBoundAddress(const std::string& address) const;
    bool hasExactEntry(const AddressDoorMultimap::Entry& target) const;
    std::size_t findExactEntryIndex(const AddressDoorMultimap::Entry& target) const;
    bool lexicographicallyLess(const Impl& other) const;

private:
    void reserve(std::size_t requested) {
        if (requested <= capacity_) {
            return;
        }

        std::size_t newCapacity = capacity_ == 0 ? 4 : capacity_ * 2;
        while (newCapacity < requested) {
            newCapacity *= 2;
        }

        AddressDoorMultimap::Entry* newData = new AddressDoorMultimap::Entry[newCapacity];
        for (std::size_t i = 0; i < size_; ++i) {
            newData[i] = data_[i];
        }

        delete[] data_;
        data_ = newData;
        capacity_ = newCapacity;
    }

    void swap(Impl& other) noexcept {
        AddressDoorMultimap::Entry* tempData = data_;
        data_ = other.data_;
        other.data_ = tempData;

        const std::size_t tempSize = size_;
        size_ = other.size_;
        other.size_ = tempSize;

        const std::size_t tempCapacity = capacity_;
        capacity_ = other.capacity_;
        other.capacity_ = tempCapacity;
    }

    AddressDoorMultimap::Entry* data_;
    std::size_t size_;
    std::size_t capacity_;
};

namespace {

void validateEntry(const AddressDoorMultimap::Entry& entry) {
    if (entry.first.empty()) {
        throw std::invalid_argument("Address must not be empty.");
    }

    if (entry.second <= 0) {
        throw std::invalid_argument("Door number must be positive.");
    }
}

}

int AddressDoorMultimap::Impl::compareEntries(const AddressDoorMultimap::Entry& left,
    const AddressDoorMultimap::Entry& right) {
    if (left.first < right.first) {
        return -1;
    }
    if (right.first < left.first) {
        return 1;
    }
    if (left.second < right.second) {
        return -1;
    }
    if (right.second < left.second) {
        return 1;
    }
    return 0;
}

std::size_t AddressDoorMultimap::Impl::lowerBoundEntry(
    const AddressDoorMultimap::Entry& target) const {
    std::size_t left = 0;
    std::size_t right = size();

    while (left < right) {
        const std::size_t middle = left + (right - left) / 2;
        if (compareEntries((*this)[middle], target) < 0) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }

    return left;
}

std::size_t AddressDoorMultimap::Impl::lowerBoundAddress(const std::string& address) const {
    std::size_t left = 0;
    std::size_t right = size();

    while (left < right) {
        const std::size_t middle = left + (right - left) / 2;
        if ((*this)[middle].first < address) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }

    return left;
}

bool AddressDoorMultimap::Impl::hasExactEntry(const AddressDoorMultimap::Entry& target) const {
    const std::size_t index = lowerBoundEntry(target);
    return index < size() && compareEntries((*this)[index], target) == 0;
}

std::size_t AddressDoorMultimap::Impl::findExactEntryIndex(
    const AddressDoorMultimap::Entry& target) const {
    const std::size_t index = lowerBoundEntry(target);
    if (index < size() && compareEntries((*this)[index], target) == 0) {
        return index;
    }
    return size();
}

bool AddressDoorMultimap::Impl::lexicographicallyLess(const Impl& other) const {
    const std::size_t limit = size() < other.size() ? size() : other.size();

    for (std::size_t i = 0; i < limit; ++i) {
        const int comparison = compareEntries((*this)[i], other[i]);
        if (comparison < 0) {
            return true;
        }
        if (comparison > 0) {
            return false;
        }
    }

    return size() < other.size();
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

    if (impl_->hasExactEntry(entry)) {
        throw DuplicateEntryException("Identical address-door pair already exists.");
    }

    impl_->insertAt(impl_->lowerBoundEntry(entry), entry);
    return *this;
}

AddressDoorMultimap& AddressDoorMultimap::operator-=(const Entry& entry) {
    validateEntry(entry);

    const std::size_t index = impl_->findExactEntryIndex(entry);
    if (index == impl_->size()) {
        throw std::out_of_range("Cannot remove a non-existent address-door pair.");
    }

    impl_->eraseAt(index);
    return *this;
}

AddressDoorMultimap& AddressDoorMultimap::operator%=(const Edit& edit) {
    validateEntry(edit.first);
    validateEntry(edit.second);

    const std::size_t oldIndex = impl_->findExactEntryIndex(edit.first);
    if (oldIndex == impl_->size()) {
        throw std::out_of_range("Cannot edit a non-existent address-door pair.");
    }

    const bool sameEntry = edit.first == edit.second;
    if (!sameEntry && impl_->hasExactEntry(edit.second)) {
        throw DuplicateEntryException("Edit would create a duplicate address-door pair.");
    }

    impl_->eraseAt(oldIndex);
    impl_->insertAt(impl_->lowerBoundEntry(edit.second), edit.second);
    return *this;
}

AddressDoorMultimap& AddressDoorMultimap::operator!() {
    impl_->clear();
    return *this;
}

std::size_t AddressDoorMultimap::operator[](const std::string& address) const {
    if (address.empty()) {
        throw std::invalid_argument("Address must not be empty.");
    }

    const std::size_t begin = impl_->lowerBoundAddress(address);
    std::size_t count = 0;
    for (std::size_t i = begin; i < impl_->size() && (*impl_)[i].first == address; ++i) {
        ++count;
    }
    return count;
}

bool AddressDoorMultimap::operator==(const AddressDoorMultimap& other) const {
    if (impl_->size() != other.impl_->size()) {
        return false;
    }

    for (std::size_t i = 0; i < impl_->size(); ++i) {
        if ((*impl_)[i] != (*other.impl_)[i]) {
            return false;
        }
    }

    return true;
}

bool AddressDoorMultimap::operator!=(const AddressDoorMultimap& other) const {
    return !(*this == other);
}

bool AddressDoorMultimap::operator<(const AddressDoorMultimap& other) const {
    return impl_->lexicographicallyLess(*other.impl_);
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
    return impl_->size();
}

bool AddressDoorMultimap::empty() const noexcept {
    return impl_->empty();
}

AddressDoorMultimap::Entry AddressDoorMultimap::at(std::size_t index) const {
    if (index >= impl_->size()) {
        throw std::out_of_range("Index is out of range.");
    }

    return (*impl_)[index];
}

std::string AddressDoorMultimap::toString() const {
    std::ostringstream output;
    output << "AddressDoorMultimap{entries=" << impl_->size();

    std::size_t uniqueAddresses = 0;
    for (std::size_t i = 0; i < impl_->size(); ++i) {
        if (i == 0 || (*impl_)[i - 1].first != (*impl_)[i].first) {
            ++uniqueAddresses;
        }
    }

    output << ", uniqueAddresses=" << uniqueAddresses << ", data=[";

    bool firstAddress = true;
    for (std::size_t i = 0; i < impl_->size();) {
        if (!firstAddress) {
            output << "; ";
        }

        firstAddress = false;
        const std::string& currentAddress = (*impl_)[i].first;
        output << currentAddress << ": ";

        bool firstDoor = true;
        while (i < impl_->size() && (*impl_)[i].first == currentAddress) {
            if (!firstDoor) {
                output << ", ";
            }

            firstDoor = false;
            output << (*impl_)[i].second;
            ++i;
        }
    }

    output << "]}";
    return output.str();
}

}