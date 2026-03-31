#include "AddressDoorMultimap.h"

#include <iostream>


int main() {
    using customadt::AddressDoorMultimap;
    using customadt::DuplicateEntryException;

    AddressDoorMultimap book;
    std::cout << "Tuscia: " << (book.empty() ? "true" : "false") << '\n';

    book += AddressDoorMultimap::Entry("Sesta g. 7", 1);
    book += AddressDoorMultimap::Entry("Sesta g. 7", 2);
    book += AddressDoorMultimap::Entry("Didlaukio g. 200", 20);
    std::cout << book.toString() << '\n';

    std::cout << "Duru adresu Sesta g. 7 viso: " << book["Sesta g. 7"] << '\n';
    std::cout << "Ivestis pirmu indeksu: " << book.at(1).first << " / " << book.at(1).second << '\n';
    std::cout << "Dabartinis ADT dydis: " << book.size() << '\n';

    AddressDoorMultimap copy(book);
    std::cout << "copy == book: " << (copy == book) << '\n';

    copy %= AddressDoorMultimap::Edit(
        AddressDoorMultimap::Entry("Sesta g. 7", 2),
        AddressDoorMultimap::Entry("Didlaukio g. 200", 10));
    std::cout << "copy != book: " << (copy != book) << '\n';
    std::cout << "copy < book: " << (copy < book) << '\n';
    std::cout << "copy <= book: " << (copy <= book) << '\n';
    std::cout << "copy > book: " << (copy > book) << '\n';
    std::cout << "copy >= book: " << (copy >= book) << '\n';

    copy -= AddressDoorMultimap::Entry("Didlaukio g. 200", 20);
    std::cout << "After deletion: " << copy.toString() << '\n';

    try {
        copy += AddressDoorMultimap::Entry("Didlaukio g. 200", 10);
    } catch (const DuplicateEntryException& ex) {
        std::cout << "Exception: " << ex.what() << '\n';
    }

    try {
        std::cout << "Trying invalid lookup:\n";
        std::cout << copy[""] << '\n';
    } catch (const std::exception& ex) {
        std::cout << "Exception: " << ex.what() << '\n';
    }

    !copy;
    std::cout << "After !copy: " << copy.toString() << '\n';

    return 0;
}
