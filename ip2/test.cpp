#include "AddressDoorMultimap.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace {

void writeLine(std::ostream& console, std::ofstream& log, const std::string& line) {
    console << line << '\n';
    log << line << '\n';
}

void report(std::ostream& console, std::ofstream& log, int index, const std::string& name,
    bool passed,const std::string& detail) {
    std::ostringstream prefix;
    prefix << "Test " << index << ": " << name;
    writeLine(console, log, prefix.str());
    writeLine(console, log, "Detail: " + detail);
    writeLine(console, log, std::string("Result: ") + (passed ? "PASSED" : "FAILED"));
}

bool expectThrows(const std::function<void()>& action) {
    try {
        action();
    } catch (...) {
        return true;
    }

    return false;
}

}

int main() {
    using Customadt::AddressDoorMultimap;
    using Customadt::DuplicateEntryException;

    std::ofstream log("log.txt");
    if (!log) {
        std::cerr << "Failed to open log.txt\n";
        return 1;
    }

    int passedCount = 0;

    writeLine(std::cout, log, "Beginning AddressDoorMultimap test run");
    writeLine(std::cout, log, "All tests directed to stdout and log.txt");

    {
        AddressDoorMultimap book;
        const bool passed = book.empty() && book.size() == 0;
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 1, "Constructor test", passed, 
        "A new object should start empty and its size should be 0.");
    }

    {
        AddressDoorMultimap book;
        book += AddressDoorMultimap::Entry("Testu g. 1", 1);
        const bool passed = !book.empty() && book.size() == 1;
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 2, "Single insertion", passed,
        "Operator+= should insert one valid address-door pair.");
    }

    {
        AddressDoorMultimap book;
        book += AddressDoorMultimap::Entry("Testu g. 1", 2);
        book += AddressDoorMultimap::Entry("Testu g. 1", 3);
        const bool passed = book["Testu g. 1"] == 2;
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 3, "Count by address", passed,
        "Operator[] should return the number of doors stored for an address.");
    }

    {
        AddressDoorMultimap book;
        book += AddressDoorMultimap::Entry("Kita g. 23", 101);
        bool customThrown = false;

        try {
            book += AddressDoorMultimap::Entry("Kita g. 23", 101);
        } catch (const DuplicateEntryException&) {
            customThrown = true;
        }

        passedCount += customThrown ? 1 : 0;
        report(std::cout, log, 4, "Duplicate exception (custom)", customThrown,
        "An identical address-door pair should throw DuplicateEntryException.");
    }

    {
        AddressDoorMultimap book;
        book += AddressDoorMultimap::Entry("Pasikartojanti g. 100", 2);
        book += AddressDoorMultimap::Entry("Pasikartojanti g. 100", 10);
        const bool passed = book.size() == 2;
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 5, "Duplicate key allowed", passed,
        "The same address may own multiple different door numbers.");
    }

    {
        AddressDoorMultimap book;
        book += AddressDoorMultimap::Entry("A g. 1", 1);
        book += AddressDoorMultimap::Entry("B g. 2", 2);
        const AddressDoorMultimap::Entry entry = book.at(1);
        const bool passed = entry.first == "B g. 2" && entry.second == 2;
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 6, "Indexed read", passed,
        "Index at(1) should return the second stored pair in sorted order (Alphabetically).");
    }

    {
        AddressDoorMultimap book;
        book += AddressDoorMultimap::Entry("Trintuko g. 1", 1);
        book -= AddressDoorMultimap::Entry("Trintuko g. 1", 1);
        const bool passed = book.empty();
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 7, "Deletion", passed,
        "Operator-= should remove an existing address-door pair.");
    }

    {
        AddressDoorMultimap book;
        const bool passed = expectThrows([&book]() {
            book -= AddressDoorMultimap::Entry("Neegzistuojanti g. 1", 99);
        });
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 8, "Missing delete throws", passed,
        "Removing a pair that does not exist should throw std::out_of_range.");
    }

    {
        AddressDoorMultimap book;
        book += AddressDoorMultimap::Entry("Typo g. 1", 4);
        book %= AddressDoorMultimap::Edit(
            AddressDoorMultimap::Entry("Typo g. 1", 4),
            AddressDoorMultimap::Entry("Correct g. 1", 4));
        const bool passed = book["Correct g. 1"] == 1 && book.at(0).second == 4;
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 9, "Edit operation", passed,
        "Operator%= should replace the old pair with the new one.");
    }

    {
        AddressDoorMultimap book;
        const bool passed = expectThrows([&book]() {
            book %= AddressDoorMultimap::Edit(
                AddressDoorMultimap::Entry("Poros g. 1", 1),
                AddressDoorMultimap::Entry("Poros g. 1", 2));
        });
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 10, "Missing edit throws", passed,
        "Editing a pair that does not exist should throw std::out_of_range.");
    }

    {
        AddressDoorMultimap book;
        const bool passed = expectThrows([&book]() {
            book += AddressDoorMultimap::Entry("", 5);
        });
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 11, "Invalid empty address", passed,
        "An empty address should throw std::invalid_argument.");
    }

    {
        AddressDoorMultimap book;
        const bool passed = expectThrows([&book]() {
            book += AddressDoorMultimap::Entry("Nera duru g.", 0);
        });
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 12, "Invalid door number", passed,
        "A non-positive door number should throw std::invalid_argument.");
    }

    {
        AddressDoorMultimap original;
        original += AddressDoorMultimap::Entry("Kopijavimo g. 1", 1);
        AddressDoorMultimap copy(original);
        copy += AddressDoorMultimap::Entry("Kopijavimo g. 2", 2);
        const bool passed = original.size() == 1 && copy.size() == 2;
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 13, "Deep copy", passed,
        "Copy construction should duplicate data instead of sharing ownership.");
    }

    {
        AddressDoorMultimap left;
        AddressDoorMultimap right;
        left += AddressDoorMultimap::Entry("A", 1);
        right += AddressDoorMultimap::Entry("A", 2);
        const bool passed = (left != right) && (left < right) && (right > left)
            && (left <= right) && (right >= left);
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 14, "Comparison operators", passed,
        "All six comparison operators should agree with each other.");
    }

    {
        AddressDoorMultimap book;
        book += AddressDoorMultimap::Entry("Svari g. 1", 101);
        !book;
        const bool passed = book.empty() && book.toString().find("entries=0") != std::string::npos;
        passedCount += passed ? 1 : 0;
        report(std::cout, log, 15, "Clear and summary", passed,
        "operator! should clear the object and toString() should reflect it.");
    }

    std::ostringstream summary;
    summary << "Summary: " << passedCount << " / 15 tests passed";
    writeLine(std::cout, log, summary.str());

    return passedCount == 15 ? 0 : 1;
}
