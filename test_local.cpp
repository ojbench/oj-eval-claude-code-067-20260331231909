#include "RefCell.hpp"
#include <iostream>
#include <cassert>

void test_basic_immutable_borrow() {
    std::cout << "Test 1: Basic immutable borrow" << std::endl;
    RefCell<int> cell(42);
    {
        auto ref1 = cell.borrow();
        assert(*ref1 == 42);
        auto ref2 = cell.borrow();  // Multiple immutable borrows OK
        assert(*ref2 == 42);
    }
    std::cout << "PASS" << std::endl;
}

void test_basic_mutable_borrow() {
    std::cout << "Test 2: Basic mutable borrow" << std::endl;
    RefCell<int> cell(42);
    {
        auto ref_mut = cell.borrow_mut();
        *ref_mut = 100;
        assert(*ref_mut == 100);
    }
    {
        auto ref = cell.borrow();
        assert(*ref == 100);
    }
    std::cout << "PASS" << std::endl;
}

void test_mutable_with_immutable_error() {
    std::cout << "Test 3: Mutable borrow with existing immutable borrow" << std::endl;
    RefCell<int> cell(42);
    auto ref = cell.borrow();
    try {
        auto ref_mut = cell.borrow_mut();
        std::cout << "FAIL: Should have thrown BorrowMutError" << std::endl;
    } catch (const BorrowMutError& e) {
        std::cout << "PASS: Caught expected BorrowMutError" << std::endl;
    }
}

void test_immutable_with_mutable_error() {
    std::cout << "Test 4: Immutable borrow with existing mutable borrow" << std::endl;
    RefCell<int> cell(42);
    auto ref_mut = cell.borrow_mut();
    try {
        auto ref = cell.borrow();
        std::cout << "FAIL: Should have thrown BorrowError" << std::endl;
    } catch (const BorrowError& e) {
        std::cout << "PASS: Caught expected BorrowError" << std::endl;
    }
}

void test_try_borrow() {
    std::cout << "Test 5: try_borrow" << std::endl;
    RefCell<int> cell(42);
    {
        auto ref_mut = cell.borrow_mut();
        auto opt = cell.try_borrow();
        assert(!opt.has_value());
    }
    {
        auto opt = cell.try_borrow();
        assert(opt.has_value());
        assert(**opt == 42);
    }
    std::cout << "PASS" << std::endl;
}

void test_try_borrow_mut() {
    std::cout << "Test 6: try_borrow_mut" << std::endl;
    RefCell<int> cell(42);
    {
        auto ref = cell.borrow();
        auto opt = cell.try_borrow_mut();
        assert(!opt.has_value());
    }
    {
        auto opt = cell.try_borrow_mut();
        assert(opt.has_value());
        **opt = 100;
        assert(**opt == 100);
    }
    std::cout << "PASS" << std::endl;
}

void test_destruction_with_borrows() {
    std::cout << "Test 7: Destruction with active borrows" << std::endl;
    try {
        RefCell<int>* cell = new RefCell<int>(42);
        auto ref = cell->borrow();
        delete cell;
        std::cout << "FAIL: Should have thrown DestructionError" << std::endl;
    } catch (const DestructionError& e) {
        std::cout << "PASS: Caught expected DestructionError" << std::endl;
    }
}

void test_move_ref() {
    std::cout << "Test 8: Move Ref" << std::endl;
    RefCell<int> cell(42);
    auto ref1 = cell.borrow();
    auto ref2 = std::move(ref1);
    assert(*ref2 == 42);
    std::cout << "PASS" << std::endl;
}

void test_move_refmut() {
    std::cout << "Test 9: Move RefMut" << std::endl;
    RefCell<int> cell(42);
    auto ref_mut1 = cell.borrow_mut();
    auto ref_mut2 = std::move(ref_mut1);
    *ref_mut2 = 100;
    assert(*ref_mut2 == 100);
    std::cout << "PASS" << std::endl;
}

int main() {
    test_basic_immutable_borrow();
    test_basic_mutable_borrow();
    test_mutable_with_immutable_error();
    test_immutable_with_mutable_error();
    test_try_borrow();
    test_try_borrow_mut();
    test_destruction_with_borrows();
    test_move_ref();
    test_move_refmut();

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
