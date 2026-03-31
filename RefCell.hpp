#ifndef REFCELL_HPP
#define REFCELL_HPP

#include <iostream>
#include <optional>
#include <stdexcept>
#include <memory>

class RefCellError : public std::runtime_error {
public:
    explicit RefCellError(const std::string& message) : std::runtime_error(message) {}
    virtual ~RefCellError() = default;
};

class BorrowError : public RefCellError {
public:
    explicit BorrowError(const std::string& message) : RefCellError(message) {}
};

class BorrowMutError : public RefCellError {
public:
    explicit BorrowMutError(const std::string& message) : RefCellError(message) {}
};

class DestructionError : public RefCellError {
public:
    explicit DestructionError(const std::string& message) : RefCellError(message) {}
};

template <typename T>
class RefCell {
private:
    T value;

    // Shared state for borrow tracking
    struct BorrowState {
        int immutable_count;  // Number of immutable borrows
        bool has_mutable;     // Whether there's a mutable borrow

        BorrowState() : immutable_count(0), has_mutable(false) {}
    };

    std::shared_ptr<BorrowState> state;

public:
    // Forward declarations
    class Ref;
    class RefMut;

    // Constructor
    explicit RefCell(const T& initial_value) : value(initial_value), state(std::make_shared<BorrowState>()) {}

    explicit RefCell(T&& initial_value) : value(std::move(initial_value)), state(std::make_shared<BorrowState>()) {}

    // Disable copying and moving for simplicity
    RefCell(const RefCell&) = delete;
    RefCell& operator=(const RefCell&) = delete;
    RefCell(RefCell&&) = delete;
    RefCell& operator=(RefCell&&) = delete;

    // Borrow methods
    Ref borrow() const {
        if (state->has_mutable) {
            throw BorrowError("Cannot borrow immutably while mutable borrow exists");
        }
        return Ref(&value, state);
    }

    std::optional<Ref> try_borrow() const {
        if (state->has_mutable) {
            return std::nullopt;
        }
        return Ref(&value, state);
    }

    RefMut borrow_mut() {
        if (state->has_mutable || state->immutable_count > 0) {
            throw BorrowMutError("Cannot borrow mutably while other borrows exist");
        }
        return RefMut(&value, state);
    }

    std::optional<RefMut> try_borrow_mut() {
        if (state->has_mutable || state->immutable_count > 0) {
            return std::nullopt;
        }
        return RefMut(&value, state);
    }

    // Inner classes for borrows
    class Ref {
    private:
        const T* ptr;
        std::shared_ptr<BorrowState> state;

    public:
        Ref(const T* p, std::shared_ptr<BorrowState> s) : ptr(p), state(s) {
            state->immutable_count++;
        }

        ~Ref() {
            if (state) {
                state->immutable_count--;
            }
        }

        const T& operator*() const {
            return *ptr;
        }

        const T* operator->() const {
            return ptr;
        }

        // Allow copying
        Ref(const Ref& other) : ptr(other.ptr), state(other.state) {
            if (state) {
                state->immutable_count++;
            }
        }

        Ref& operator=(const Ref& other) {
            if (this != &other) {
                // Decrement old state
                if (state) {
                    state->immutable_count--;
                }
                // Copy new state
                ptr = other.ptr;
                state = other.state;
                if (state) {
                    state->immutable_count++;
                }
            }
            return *this;
        }

        // Allow moving
        Ref(Ref&& other) noexcept : ptr(other.ptr), state(std::move(other.state)) {
            other.ptr = nullptr;
        }

        Ref& operator=(Ref&& other) noexcept {
            if (this != &other) {
                // Decrement old state
                if (state) {
                    state->immutable_count--;
                }
                // Move new state
                ptr = other.ptr;
                state = std::move(other.state);
                other.ptr = nullptr;
            }
            return *this;
        }
    };

    class RefMut {
    private:
        T* ptr;
        std::shared_ptr<BorrowState> state;

    public:
        RefMut(T* p, std::shared_ptr<BorrowState> s) : ptr(p), state(s) {
            state->has_mutable = true;
        }

        ~RefMut() {
            if (state) {
                state->has_mutable = false;
            }
        }

        T& operator*() {
            return *ptr;
        }

        T* operator->() {
            return ptr;
        }

        // Disable copying to ensure correct borrow rules
        RefMut(const RefMut&) = delete;
        RefMut& operator=(const RefMut&) = delete;

        // Allow moving
        RefMut(RefMut&& other) noexcept : ptr(other.ptr), state(std::move(other.state)) {
            other.ptr = nullptr;
        }

        RefMut& operator=(RefMut&& other) noexcept {
            if (this != &other) {
                // Reset old state
                if (state) {
                    state->has_mutable = false;
                }
                // Move new state
                ptr = other.ptr;
                state = std::move(other.state);
                other.ptr = nullptr;
            }
            return *this;
        }
    };

    // Destructor
    ~RefCell() noexcept(false) {
        if (state->immutable_count > 0 || state->has_mutable) {
            throw DestructionError("RefCell destroyed while borrows still exist");
        }
    }
};

#endif // REFCELL_HPP
