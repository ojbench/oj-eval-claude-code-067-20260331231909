# RefCell Implementation - Submission Summary

## Problem
Implement Rust's RefCell in C++ with runtime borrow checking using shared_ptr/weak_ptr.

## Solution Approach

### Key Design Decisions
1. **Borrow State Tracking**: Used a `shared_ptr<BorrowState>` to track:
   - `immutable_count`: Count of active immutable borrows
   - `has_mutable`: Boolean flag for mutable borrow

2. **Ref and RefMut Classes**:
   - Both hold a pointer to the value and shared_ptr to the borrow state
   - Automatically increment/decrement counters in constructors/destructors
   - Ref is copyable, RefMut is move-only

3. **Exception Handling**:
   - `BorrowError`: Thrown when immutable borrow fails (mutable borrow exists)
   - `BorrowMutError`: Thrown when mutable borrow fails (any borrow exists)
   - `DestructionError`: Thrown when RefCell destroyed with active borrows

4. **Move Semantics**: Properly implemented for both Ref and RefMut to handle ownership transfer

## Submission Results

### Submission #1 (Failed - 766561)
- **Error**: Incorrectly submitted GitHub URL as code with language="cpp"
- **Lesson**: The OJ requires the actual source code content, not a Git URL

### Submission #2 (Success - 766565)
- **Score**: 100/100
- **Status**: Accepted on all test cases

#### Test Results:
| Subtask | Score | Status |
|---------|-------|--------|
| basic-1 | 5/5 | Accepted |
| basic-2 | 5/5 | Accepted |
| basic-3 | 5/5 | Accepted |
| panic-1 | 5/5 | Accepted |
| panic-2 | 5/5 | Accepted |
| panic-3 | 5/5 | Accepted |
| panic-4 | 5/5 | Accepted |
| panic-5 | 5/5 | Accepted |
| hard-1 | 20/20 | Accepted |
| hard-2 | 20/20 | Accepted |
| move | 20/20 | Accepted |

## Implementation Highlights

### RefCell Class
- Stores value and shared borrow state
- Provides `borrow()` and `try_borrow()` for immutable access
- Provides `borrow_mut()` and `try_borrow_mut()` for mutable access
- Destructor checks for active borrows and throws if any exist

### Ref Class (Immutable Borrow)
- Copyable and movable
- Increments immutable_count on construction/copy
- Decrements immutable_count on destruction
- Provides const access to underlying value

### RefMut Class (Mutable Borrow)
- Move-only (copying disabled)
- Sets has_mutable flag on construction
- Clears has_mutable flag on destruction
- Provides mutable access to underlying value

## Files
- `RefCell.hpp`: Complete implementation
- `main.cpp`: Simple test program
- `test_local.cpp`: Comprehensive local test suite
- `CMakeLists.txt`: Build configuration
- `.gitignore`: Ignores build artifacts

## Submission Count
- Total submissions used: 2/5
- Remaining submissions: 3
