# CustomQueue

A generic FIFO Queue implementation built from scratch in modern C++20.

This project is part of my C++ Engineering Projects series, focused on understanding data structures, memory management, object lifetime, RAII, copy/move semantics, templates, testing, CMake, and Git.

---

## Overview

`CustomQueue<T>` is a generic FIFO (First-In, First-Out) queue implementation built from scratch.

The queue uses a **circular buffer** internally to provide constant-time insertion and removal without shifting elements.

### FIFO Principle

FIFO means:

> First element inserted → First element removed

Example:

```text
enqueue(10)
enqueue(20)
enqueue(30)

Queue:

10 → 20 → 30

dequeue()
↓
10 is removed

Remaining:

20 → 30
```

---

## Project Structure

```text
06-custom-queue/
│
├── include/
│   └── custom_queue.hpp
│
├── tests/
│   └── test_custom_queue.cpp
│
├── benchmarks/
│   └── benchmark_custom_queue.cpp
│
├── docs/
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## Core API

The queue provides the following operations:

```cpp
enqueue()
dequeue()
front()
back()
empty()
size()
```

### `enqueue()`

Adds an element to the back of the queue.

```cpp
queue.enqueue(10);
```

### `dequeue()`

Removes the element from the front of the queue.

```cpp
queue.dequeue();
```

### `front()`

Returns the first element without removing it.

```cpp
queue.front();
```

### `back()`

Returns the last element without removing it.

```cpp
queue.back();
```

### `empty()`

Returns `true` when the queue contains no elements.

```cpp
queue.empty();
```

### `size()`

Returns the current number of elements.

```cpp
queue.size();
```

---

## Internal Design

The queue uses a circular buffer.

The implementation maintains four important pieces of state:

```cpp
T* data_;
std::size_t capacity_;
std::size_t front_;
std::size_t back_;
std::size_t size_;
```

### `front_`

`front_` stores the **index of the first logical element** in the queue.

### `back_`

`back_` stores the **index of the next insertion position**.

It does **not** point directly to the last element.

### `size_`

`size_` stores the **number of elements currently present in the queue**.

### `capacity_`

`capacity_` stores the total number of element positions available.

---

## Example State

Suppose:

```text
capacity = 4
```

After:

```cpp
queue.enqueue(10);
queue.enqueue(20);
queue.enqueue(30);
```

the physical storage looks like:

```text
Index:    0     1     2     3
         [10]  [20]  [30]  [ ]

front_ = 0
back_  = 3
size_  = 3
```

Notice:

```text
front_ → index 0 → first element
back_  → index 3 → next insertion position
size_  → 3      → number of elements
```

The last element is at:

```text
(back_ - 1 + capacity_) % capacity_
```

---

## Circular Buffer

A circular buffer allows the queue to reuse positions that became free after dequeue operations.

Example:

```text
Initial:

[10] [20] [30] [40]

front_ = 0
back_  = 0
size_  = 4
```

After removing `10`:

```text
[ ] [20] [30] [40]

front_ = 1
back_  = 0
size_  = 3
```

The free position at index `0` can now be reused.

If we enqueue `50`:

```text
[50] [20] [30] [40]

front_ = 1
back_  = 1
size_  = 4
```

The logical queue is:

```text
20 → 30 → 40 → 50
```

Even though the physical array looks like:

```text
[50] [20] [30] [40]
```

The logical order is determined by `front_`, `back_`, and `size_`.

---

## Circular Indexing

The queue uses modulo arithmetic to wrap indices around the storage.

### Advancing `front_`

```cpp
front_ = (front_ + 1) % capacity_;
```

### Advancing `back_`

```cpp
back_ = (back_ + 1) % capacity_;
```

This means that when an index reaches the end of the array, it returns to index `0`.

Example:

```text
capacity = 4

3 → 0
```

because:

```cpp
(3 + 1) % 4
```

gives:

```text
0
```

---

## Back Element Calculation

Because `back_` represents the next insertion position, the last element is one position before `back_`.

The circular index is calculated using:

```cpp
(back_ - 1 + capacity_) % capacity_
```

Example:

```text
back_ = 3
capacity = 4
```

Then:

```text
(3 - 1 + 4) % 4
= 6 % 4
= 2
```

Therefore:

```text
data_[2]
```

contains the last element.

The `+ capacity_` prevents a negative result when `back_ == 0`.

---

## Complexity

Because the queue uses a circular buffer and does not shift elements during dequeue:

| Operation   | Complexity |
| ----------- | ---------- |
| `enqueue()` | O(1)       |
| `dequeue()` | O(1)       |
| `front()`   | O(1)       |
| `back()`    | O(1)       |
| `empty()`   | O(1)       |
| `size()`    | O(1)       |

Memory usage:

```text
O(capacity)
```

---

## Memory Management

The queue uses manually managed raw storage.

Memory is allocated using:

```cpp
::operator new(...)
```

Objects are constructed explicitly using:

```cpp
std::construct_at(...)
```

Objects are destroyed explicitly using:

```cpp
std::destroy_at(...)
```

Finally, the raw memory is released using:

```cpp
::operator delete(...)
```

This separates:

```text
Memory allocation
        ↓
Object construction
        ↓
Object lifetime
        ↓
Object destruction
        ↓
Memory deallocation
```

This approach provides explicit control over object lifetime.

---

## RAII

The queue follows the RAII principle.

The constructor acquires the memory resource.

The destructor:

1. Destroys live objects.
2. Releases the allocated memory.

Therefore the queue owns its memory and is responsible for cleaning it up.

---

## Copy Semantics

The queue implements:

```cpp
CustomQueue(const CustomQueue& other);
```

and:

```cpp
CustomQueue& operator=(const CustomQueue& other);
```

A copied queue owns its own memory.

Example:

```text
Original:

[10] [20] [30]

        copy
         ↓

Copied:

[10] [20] [30]
```

The two queues do not share the same storage.

---

## Move Semantics

The queue also implements:

```cpp
CustomQueue(CustomQueue&& other);
```

and:

```cpp
CustomQueue& operator=(CustomQueue&& other);
```

Move semantics transfer ownership of the allocated storage instead of performing a deep copy.

Conceptually:

```text
Before:

source → [10][20][30]
target → own storage

After move:

source → empty/null state
target → [10][20][30]
```

This avoids unnecessary element copying.

---

## Exception Handling

The queue handles invalid operations using exceptions.

### Dequeue from empty queue

```text
Queue is empty
```

### Front on empty queue

```text
Queue is empty
```

### Back on empty queue

```text
Queue is empty
```

### Enqueue into full queue

```text
Queue is full
```

### Zero capacity

A queue cannot be created with zero capacity.

---

## Const Correctness

Read-only operations provide `const` overloads.

For example:

```cpp
T& front();

const T& front() const;
```

This allows both mutable and read-only queues to access their elements correctly.

---

## Lvalue and Rvalue Enqueue

The queue supports both lvalue and rvalue insertion.

### Lvalue

```cpp
std::string value = "C++";

queue.enqueue(value);
```

### Rvalue

```cpp
queue.enqueue(std::string("C++"));
```

The rvalue overload allows move construction when appropriate.

---

## Testing

The test program verifies:

* Initial empty state
* Initial size
* Enqueue
* Dequeue
* FIFO behavior
* Front access
* Back access
* Size tracking
* Empty state
* Full queue detection
* Circular wrap-around
* Empty queue exceptions
* Rvalue insertion
* Copy construction
* Move construction
* Const access

---

## Build

Configure the project:

```bash
cmake -S . -B build -G "MinGW Makefiles"
```

Build:

```bash
cmake --build build
```

Run tests:

```bash
.\build\custom_queue_tests.exe
```

---

## Example Usage

```cpp
#include "custom_queue.hpp"

#include <iostream>

int main()
{
    custom::CustomQueue<int> queue(4);

    queue.enqueue(10);
    queue.enqueue(20);
    queue.enqueue(30);

    std::cout << queue.front() << '\n';
    std::cout << queue.back() << '\n';

    queue.dequeue();

    std::cout << queue.front() << '\n';

    return 0;
}
```

Output:

```text
10
30
20
```

---

## Benchmarking

A separate benchmark is included to measure the performance of:

* Enqueue operations
* Dequeue operations
* Large queue workloads
* Circular buffer behavior

The benchmark is intended for learning and performance observation.

It is not intended to be a formal scientific benchmark.

---

## Learning Goals

This project focuses on understanding:

* FIFO data structures
* Circular buffers
* Index management
* Modulo arithmetic
* Raw memory
* Object lifetime
* Explicit construction and destruction
* RAII
* Templates
* Copy semantics
* Move semantics
* Const correctness
* Exception handling
* Exception safety
* CMake
* Testing
* Benchmarking
* Git
* GitHub workflow

---

## Engineering Workflow

The project follows this workflow:

```text
Requirements
     ↓
Architecture
     ↓
Data Structure / Memory Model
     ↓
API Design
     ↓
Implementation
     ↓
Build
     ↓
Testing
     ↓
Debugging
     ↓
Edge Cases
     ↓
Refactoring
     ↓
Documentation
     ↓
Benchmarking
     ↓
Git
     ↓
GitHub
```

---

## Project Status

```text
CustomQueue
```

Status:

```text
Implementation      ✅
Basic API           ✅
FIFO behavior       ✅
Circular buffer     ✅
Exception handling  ✅
Object lifetime     ✅
Copy semantics      ✅
Move semantics      ✅
Const correctness   ✅
Testing             ✅
Benchmark setup     🚧
Documentation       ✅
Git                  🚧
```

---

## Part of C++ Engineering Projects

This project is part of a long-term C++ Engineering Projects series.

Project:

```text
06 - CustomQueue
```

Previous projects:

```text
01 - CustomVector
02 - CustomString
03 - CustomArray
04 - CustomLinkedList
05 - CustomStack
```

Next project:

```text
07 - CustomHashMap
```

The goal of this series is to build data structures and systems-oriented components from scratch while developing a deeper understanding of modern C++, memory management, object lifetime, performance, and software engineering practices.
