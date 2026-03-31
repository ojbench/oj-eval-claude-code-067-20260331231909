#include "RefCell.hpp"
#include <iostream>
#include <string>
#include <sstream>

// This is a placeholder main function for the OJ system
// The OJ will likely provide its own test harness that includes RefCell.hpp
int main() {
    // Simple test to verify compilation
    try {
        RefCell<int> cell(42);
        auto ref1 = cell.borrow();
        std::cout << *ref1 << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
