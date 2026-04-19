#ifndef CUSTOMADT_ADDRESSDOORMULTIMAP_H
#define CUSTOMADT_ADDRESSDOORMULTIMAP_H

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace Customadt {

class DuplicateEntryException : public std::logic_error {
public:
    explicit DuplicateEntryException(const std::string& message);
};

class AddressDoorMultimap {
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
public:
    using Entry = std::pair<std::string, int>;
    using Edit = std::pair<Entry, Entry>;

    AddressDoorMultimap();
    AddressDoorMultimap(const AddressDoorMultimap& other);
    AddressDoorMultimap& operator=(const AddressDoorMultimap& other);
    ~AddressDoorMultimap();

    AddressDoorMultimap& operator+=(const Entry& entry);
    AddressDoorMultimap& operator-=(const Entry& entry);
    AddressDoorMultimap& operator%=(const Edit& edit);
    AddressDoorMultimap& operator!();

    std::size_t operator[](const std::string& address) const;

    bool operator==(const AddressDoorMultimap& other) const;
    bool operator!=(const AddressDoorMultimap& other) const;
    bool operator<(const AddressDoorMultimap& other) const;
    bool operator<=(const AddressDoorMultimap& other) const;
    bool operator>(const AddressDoorMultimap& other) const;
    bool operator>=(const AddressDoorMultimap& other) const;

    std::size_t size() const noexcept;
    bool empty() const noexcept;
    Entry at(std::size_t index) const;
    std::string toString() const;
};

}
#endif

/*
Implementacija sukurta naudojantis GPT-5.4 (Naudojau ir kitur).

Multimap - tai ADT, kuriame daugiau nei vienas value gali turėti tą patį raktą, pavyzdžiui
kažkoks tikslus daugiabučio adresas adresas sujungtas su keliomis skirtingomis bute esančiomis
durimis. Insertionai saugomi abėcėlės tvarka.

Makefile:

build module .o: 
    make -f makefile.txt module
build demo: 
    make -f makefile.txt demo
run demo: 
    make -f makefile.txt run-demo
build test: 
    make -f makefile.txt test
run test: 
    make -f makefile.txt run-test
clean everything: 
    make -f makefile.txt clean
rebuild from scratch: 
    make -f makefile.txt rebuild
*/
