# CustomQueue — C++ Engineering Notes

## 1. Queue kya hoti hai?

Queue ek **FIFO (First In, First Out)** data structure hai.

Jo element sabse pehle queue mein enter karta hai, wahi sabse pehle remove hota hai.

Example:

```text
enqueue(10)
enqueue(20)
enqueue(30)

Queue:

Front
  ↓
[10] [20] [30]
             ↑
            Back

Dequeue karne par:

text
10 → remove
20 → next
30 → next



# 2. CustomQueue ka Goal

Is project mein hum standard library ki queue use nahi kar rahe.

Hum apni queue scratch se implement kar rahe hain.

Main goals:

* FIFO behavior
* Fixed capacity
* O(1) enqueue
* O(1) dequeue
* Circular buffer
* Manual memory management
* Object lifetime control
* Copy semantics
* Move semantics
* RAII
* Const correctness
* Exception handling
* CMake build
* Testing
* Benchmarking

---

# 3. Queue ki Basic Operations

Hamari `CustomQueue<T>` API:
cpp
enqueue()
dequeue()
front()
back()
empty()
size()


Meaning:

| Function    | Meaning                        |
| ----------- | ------------------------------ |
| `enqueue()` | Element insert karta hai       |
| `dequeue()` | Front element remove karta hai |
| `front()`   | First element return karta hai |
| `back()`    | Last element return karta hai  |
| `empty()`   | Queue empty hai ya nahi        |
| `size()`    | Current elements ki count      |

---

# 4. Simple Array Queue mein Problem

Agar normal array use karein:

```text
[10] [20] [30] [40]
 ↑
front
```

Agar `10` remove karna ho, remaining elements ko shift karna padega:

```text
[20] [30] [40]
```

Large queue mein shifting expensive ho sakti hai.

Agar `n` elements shift karne pad rahe hain:

```text
O(n)
```

Humein dequeue ko O(1) banana hai.

---

# 5. Circular Buffer

Is problem ko solve karne ke liye hum **circular buffer** use karte hain.

Memory physically circular nahi hoti.

Array normal hi hota hai:

```text
[0] [1] [2] [3]
```

Lekin indices ko circular manner mein use karte hain.

Last index ke baad:

```text
3 → 0
```

Isliye:

```cpp
index = (index + 1) % capacity;
```

Example:

```text
capacity = 4

0 → 1 → 2 → 3 → 0 → 1 → ...
```

---

# 6. CustomQueue ke Internal Members

Hamari queue mein:

```cpp
T* data_;

std::size_t capacity_;
std::size_t front_;
std::size_t back_;
std::size_t size_;
```

Inka meaning samajhna bahut important hai.

---

# 7. `data_`

```cpp
T* data_;
```

`data_` dynamically allocated raw memory ko point karta hai.

Important:

`data_` sirf memory ka address hold karta hai.

Memory allocate karna aur object construct karna same cheez nahi hai.

Hum:

```cpp
::operator new(...)
```

se raw memory allocate karte hain.

Phir:

```cpp
std::construct_at(...)
```

se us memory mein actual `T` object construct karte hain.

---

# 8. `capacity_`

```cpp
std::size_t capacity_;
```

`capacity_` batata hai ki queue mein maximum kitne elements simultaneously store ho sakte hain.

Example:

```cpp
CustomQueue<int> queue(4);
```

To:

```text
capacity_ = 4
```

Maximum 4 live elements ho sakte hain.

---

# 9. `size_`

```cpp
std::size_t size_;
```

`size_` batata hai ki currently queue mein kitne live elements hain.

Example:

```text
capacity = 5

[10] [20] [30] [ ] [ ]

size = 3
```

Important:

```text
capacity_ ≠ size_
```

Capacity = total available slots.

Size = currently stored elements.

---

# 10. `front_`

```cpp
std::size_t front_;
```

`front_` **current front element ka index** hai.

Example:

```text
[10] [20] [30] [ ]
 ↑
front_
```

Agar:

```text
front_ = 0
```

to front element `data_[0]` hai.

Dequeue ke baad:

```text
[10] [20] [30]
      ↑
    front_
```

Ab:

```text
front_ = 1
```

---

# 11. `back_`

Ye sabse important distinction hai.

`back_` **last element ka index nahi hai**.

`back_` batata hai:

> next element kis position par insert hoga.

Example:

```text
[10] [20] [30] [ ]
 ↑             ↑
front_       back_
```

Agar:

```text
front_ = 0
back_ = 3
size_ = 3
```

to:

```text
data_[0] = 10
data_[1] = 20
data_[2] = 30
data_[3] = empty slot
```

Next:

```cpp
enqueue(40);
```

to `40` `data_[3]` par construct hoga.

---

# 12. `back_` ko Last Element Samajhne ki Mistake

Ye galat mental model hai:

```text
back_ = last element
```

Hamare implementation mein correct model:

```text
front_ = current first element
back_  = next insertion position
```

Last element ka index:

```cpp
(back_ - 1 + capacity_) % capacity_
```

Isi purpose ke liye helper function hai:

```cpp
std::size_t back_index() const
```

---

# 13. Initial State

Queue create hone ke baad:

```cpp
CustomQueue<int> queue(4);
```

State:

```text
capacity_ = 4
front_    = 0
back_     = 0
size_     = 0
```

Conceptually:

```text
[ ] [ ] [ ] [ ]

 ↑
front_
back_
```

Kyunki queue empty hai, dono indices `0` par hain.

---

# 14. First `enqueue()`

```cpp
queue.enqueue(10);
```

Before:

```text
front_ = 0
back_  = 0
size_  = 0
```

Element `data_[back_]` par construct hota hai:

```text
[10] [ ] [ ] [ ]
```

Then:

```cpp
back_ = (back_ + 1) % capacity_;
++size_;
```

New state:

```text
front_ = 0
back_  = 1
size_  = 1
```

---

# 15. Multiple Enqueue

```cpp
queue.enqueue(10);
queue.enqueue(20);
queue.enqueue(30);
```

Memory:

```text
index:    0    1    2    3

data:    [10] [20] [30] [ ]
          ↑              ↑
       front_          back_
```

State:

```text
front_ = 0
back_  = 3
size_  = 3
capacity_ = 4
```

Logical queue:

```text
10 → 20 → 30
```

---

# 16. `enqueue()` ka Core Logic

```cpp
void enqueue(const T& value)
{
    if (size_ == capacity_) {
        throw std::runtime_error("Queue is full");
    }

    std::construct_at(data_ + back_, value);

    back_ = (back_ + 1) % capacity_;

    ++size_;
}
```

Order:

```text
1. Check full
2. Construct object
3. Move back_ circularly
4. Increase size_
```

---

# 17. Queue Full

Agar:

```text
capacity_ = 4
size_ = 4
```

to queue full hai.

Example:

```text
[10] [20] [30] [40]
 ↑                  ↑
front_             back_
```

Yahan `back_` circularly `0` ho sakta hai.

Lekin:

```text
size_ == capacity_
```

batata hai ki queue full hai.

Isliye:

```cpp
enqueue(50);
```

par:

```cpp
throw std::runtime_error("Queue is full");
```

---

# 18. `dequeue()`

`dequeue()` queue ke front element ko remove karta hai.

Example:

```text
[10] [20] [30] [ ]
 ↑
front_
```

`dequeue()`:

```text
10 destroy
```

Then:

```cpp
front_ = (front_ + 1) % capacity_;
--size_;
```

New logical queue:

```text
20 → 30
```

---

# 19. `dequeue()` mein Shift nahi hota

Important point:

Hum elements ko shift nahi karte.

Before:

```text
[10] [20] [30] [ ]
 ↑
front_
```

After dequeue:

```text
[destroyed] [20] [30] [ ]
              ↑
            front_
```

Physical array mein `20` ko index `0` par move nahi kiya gaya.

Sirf:

```cpp
front_
```

change hua.

Isi wajah se dequeue O(1) hai.

---

# 20. `dequeue()` ka Core Logic

```cpp
void dequeue()
{
    if (size_ == 0) {
        throw std::runtime_error("Queue is empty");
    }

    std::destroy_at(data_ + front_);

    front_ = (front_ + 1) % capacity_;

    --size_;
}
```

Order:

```text
1. Check empty
2. Destroy front object
3. Move front_ circularly
4. Decrease size_
```

---

# 21. Circular Reuse

Suppose:

```text
capacity = 4
```

Initially:

```text
[10] [20] [30] [40]
 ↑
front_
```

Agar `10` aur `20` dequeue kar diye:

```text
[ ] [ ] [30] [40]
          ↑
        front_
```

Ab beginning ke slots free hain.

Queue unhe reuse kar sakti hai.

```cpp
enqueue(50);
enqueue(60);
```

Result:

```text
[50] [60] [30] [40]
 ↑         ↑
 ?       front_
```

Logical order memory order jaisa zaroori nahi hai.

Logical queue:

```text
30 → 40 → 50 → 60
```

---

# 22. Circular Index Calculation

Next position:

```cpp
(index + 1) % capacity_
```

Example:

```text
capacity = 4

index = 0 → 1
index = 1 → 2
index = 2 → 3
index = 3 → 0
```

Isse array circularly reuse hota hai.

---

# 23. Logical Order vs Physical Order

Ye CustomQueue ka important concept hai.

Physical memory:

```text
[50] [60] [30] [40]
```

Logical queue:

```text
30 → 40 → 50 → 60
```

Kyun?

Because:

```text
front_ = 2
```

Logical traversal:

```cpp
(front_ + i) % capacity_
```

Example:

```text
i = 0 → 2 → 30
i = 1 → 3 → 40
i = 2 → 0 → 50
i = 3 → 1 → 60
```

---

# 24. `front()`

`front()` first logical element return karta hai.

```cpp
return data_[front_];
```

Example:

```text
[50] [60] [30] [40]
          ↑
        front_
```

Result:

```text
front() = 30
```

Empty queue mein:

```cpp
front()
```

exception throw karta hai.

---

# 25. `back()`

`back()` last logical element return karta hai.

Lekin:

```text
back_
```

next insertion position hai.

Isliye last element:

```cpp
(back_ - 1 + capacity_) % capacity_
```

se locate kiya jata hai.

Example:

```text
capacity = 4
back_ = 1
```

Last element index:

```text
(1 - 1 + 4) % 4
= 0
```

So:

```text
data_[0]
```

last logical element hai.

---

# 26. `back()` aur `back_` Different hain

Very important:

```text
back_       → next insertion position
back()      → last element
```

Ye dono same concept nahi hain.

Example:

```text
[50] [60] [30] [40]
 ↑    ↑
 ?    ?

front_ = 2
back_  = 2
```

Queue full hone par `back_` aur `front_` same index par ho sakte hain.

Isliye `size_` important hai.

---

# 27. Empty Queue

Queue empty hai jab:

```cpp
size_ == 0
```

Isliye:

```cpp
bool empty() const
{
    return size_ == 0;
}
```

Important:

Hum empty/full determine karne ke liye sirf `front_` aur `back_` par depend nahi kar rahe.

`size_` exact state batata hai.

---

# 28. `size()`

```cpp
std::size_t size() const
{
    return size_;
}
```

Ye current number of elements return karta hai.

Example:

```text
capacity = 10
size = 3
```

Queue mein sirf 3 live objects hain.

---

# 29. `std::size_t`

Sizes aur indices ke liye:

```cpp
std::size_t
```

use kiya gaya hai.

Example:

```cpp
std::size_t size_;
std::size_t capacity_;
std::size_t front_;
std::size_t back_;
```

Ye unsigned integer type hai jo object sizes aur indexing ke liye designed hai.

---

# 30. Raw Memory Allocation

Constructor mein:

```cpp
data_ = static_cast<T*>(
    ::operator new(sizeof(T) * capacity_)
);
```

Yahan hum raw memory allocate kar rahe hain.

Important:

```cpp
::operator new(...)
```

memory allocate karta hai.

Ye automatically har position par `T` object construct nahi karta.

---

# 31. Allocation vs Construction

Ye distinction C++ engineering mein extremely important hai.

### Allocation

```cpp
::operator new(...)
```

Raw memory deta hai.

### Construction

```cpp
std::construct_at(...)
```

Us memory location par actual object create karta hai.

Example:

```cpp
std::construct_at(data_ + back_, value);
```

---

# 32. Destruction

Object remove karne ke liye:

```cpp
std::destroy_at(data_ + front_);
```

Ye object ka destructor call karta hai.

Memory immediately free nahi hoti.

Memory poori queue destroy hone par:

```cpp
::operator delete(data_);
```

se release hoti hai.

---

# 33. Object Lifetime Model

Hamari queue mein lifecycle roughly:

```text
Raw Memory
    ↓
construct_at()
    ↓
Live T object
    ↓
destroy_at()
    ↓
Raw Memory
    ↓
operator delete()
    ↓
Memory released
```

Ye allocation aur object lifetime ka fundamental difference hai.

---

# 34. RAII

RAII:

> Resource Acquisition Is Initialization

Queue constructor resource acquire karta hai:

```cpp
::operator new(...)
```

Destructor resource release karta hai:

```cpp
::operator delete(data_);
```

Agar queue object scope se bahar chala jaye:

```cpp
{
    CustomQueue<int> queue(100);
}
```

to destructor automatically execute hota hai.

Isliye memory leak avoid hota hai.

---

# 35. Destructor

Destructor:

```cpp
~CustomQueue()
{
    if (data_) {
        destroy_elements();
        ::operator delete(data_);
    }
}
```

Pehle live objects destroy hote hain:

```cpp
destroy_elements();
```

Phir raw memory release hoti hai:

```cpp
::operator delete(data_);
```

Order important hai.

---

# 36. `destroy_elements()`

Queue circular hai, isliye live objects contiguous hona zaroori nahi.

Isliye:

```cpp
for (std::size_t i = 0; i < size_; ++i) {
    std::size_t index =
        (front_ + i) % capacity_;

    std::destroy_at(data_ + index);
}
```

Logical queue order follow karke har live object destroy hota hai.

---

# 37. Copy Constructor

Copy constructor:

```cpp
CustomQueue(const CustomQueue& other)
```

ka purpose hai independent copy banana.

Example:

```cpp
CustomQueue<int> original(4);

original.enqueue(10);
original.enqueue(20);

CustomQueue<int> copied(original);
```

Ab:

```text
original → [10][20]
copied   → [10][20]
```

Lekin dono ki memory alag hai.

---

# 38. Deep Copy

Queue mein:

```cpp
data_
```

pointer hai.

Sirf pointer copy karna dangerous hota:

```text
original.data_
      ↓
   same memory
      ↑
 copied.data_
```

Phir dono same memory own karenge.

Destructor mein double delete ho sakta hai.

Isliye copy constructor new memory allocate karta hai aur elements ko separately construct karta hai.

---

# 39. Copy Constructor Circular State ko Normalize karta hai

Original queue circular state mein ho sakti hai.

Example logical order:

```text
30 → 40 → 50 → 60
```

physical storage:

```text
[50] [60] [30] [40]
```

Copy constructor logical order ko copy karta hai:

```text
copied:

[30] [40] [50] [60]
```

Yaani copy ki internal layout normalize ho sakti hai.

Lekin externally queue ka behavior same hai.

---

# 40. Copy Assignment

```cpp
CustomQueue& operator=(const CustomQueue& other)
```

Copy assignment ke liye:

```cpp
CustomQueue temp(other);
swap(temp);
```

use kiya gaya hai.

Ye approach **copy-and-swap** pattern follow karti hai.

Basic idea:

```text
other
  ↓
temporary copy
  ↓
swap
  ↓
old resource temporary ke paas
  ↓
temporary destructor
```

---

# 41. Self Assignment

Example:

```cpp
queue = queue;
```

Isliye check:

```cpp
if (this == &other) {
    return *this;
}
```

kiya gaya hai.

---

# 42. Move Constructor

Move constructor:

```cpp
CustomQueue(CustomQueue&& other) noexcept
```

resources ko transfer karta hai.

Copy mein:

```text
A → resources
B → new resources
```

Move mein:

```text
A → resources
B → same resources
A → empty/null state
```

---

# 43. Move ka Main Idea

Agar:

```cpp
CustomQueue<int> source(4);
```

ke paas allocated memory hai:

```text
source.data_
     ↓
[500][600][ ][ ]
```

Move constructor memory ko copy nahi karta.

Bas pointer transfer karta hai:

```text
moved.data_
     ↓
[500][600][ ][ ]

source.data_ = nullptr
```

Isliye move generally cheaper hota hai.

---

# 44. Moved-From Object

Move ke baad source ko valid but empty-like state mein rakha gaya hai:

```cpp
other.data_ = nullptr;
other.capacity_ = 0;
other.front_ = 0;
other.back_ = 0;
other.size_ = 0;
```

Important:

Move ka matlab source destroy nahi hua.

Source object abhi exist karta hai.

Uski resources transfer ho gayi hain.

---

# 45. Move Assignment

Move assignment:

```cpp
CustomQueue& operator=(CustomQueue&& other) noexcept
```

mein pehle current object's resource release hota hai.

Then:

```cpp
data_ = other.data_;
capacity_ = other.capacity_;
front_ = other.front_;
back_ = other.back_;
size_ = other.size_;
```

Resources transfer hote hain.

Source reset hota hai.

---

# 46. `noexcept`

Move operations:

```cpp
noexcept
```

mark ki gayi hain.

Reason:

Move operation resource ownership transfer kar raha hai aur implementation exception throw nahi karti.

Standard containers bhi move operations ke behavior ke liye `noexcept` ko important consider karte hain.

---

# 47. Rvalue `enqueue()`

Queue mein do overloads hain:

```cpp
void enqueue(const T& value);
```

aur:

```cpp
void enqueue(T&& value);
```

First lvalue ke liye.

Second rvalue ke liye.

Rvalue version:

```cpp
std::construct_at(
    data_ + back_,
    std::move(value)
);
```

use karta hai.

---

# 48. Lvalue vs Rvalue

Example:

```cpp
std::string name = "Niteen";

queue.enqueue(name);
```

`name` lvalue hai.

Call:

```cpp
enqueue(const T&)
```

Example:

```cpp
queue.enqueue(std::string("C++"));
```

temporary object rvalue hai.

Call:

```cpp
enqueue(T&&)
```

---

# 49. `std::move`

```cpp
std::move(value)
```

khud resource move nahi karta.

Ye mainly value ko rvalue expression mein cast karta hai, taaki appropriate move constructor/move operation select ho sake.

Actual movement receiving type ke move constructor/assignment par depend karta hai.

---

# 50. Const Correctness

Queue mein:

```cpp
T& front();
const T& front() const;
```

dono overloads hain.

Same `back()` ke liye bhi.

Normal queue:

```cpp
queue.front() = 100;
```

allowed hai.

Const queue:

```cpp
const CustomQueue<int>& queue = ...;
```

mein:

```cpp
queue.front();
```

const reference return karega.

---

# 51. Exceptions

Empty queue par:

```cpp
dequeue();
front();
back();
```

invalid operations hain.

Isliye:

```cpp
throw std::runtime_error("Queue is empty");
```

use hota hai.

Full queue par:

```cpp
enqueue(...)
```

invalid hai.

Isliye:

```cpp
throw std::runtime_error("Queue is full");
```

---

# 52. Exception Safety

Constructor mein element construction fail ho sakti hai.

Isliye copy constructor mein:

```cpp
try {
    ...
}
catch (...) {
    destroy_elements();
    ::operator delete(data_);
    throw;
}
```

use kiya gaya hai.

Agar copying ke beech exception aaye:

```text
already constructed objects
        ↓
destroy
        ↓
memory release
        ↓
exception rethrow
```

Isse partially constructed object ki memory leak nahi hoti.

---

# 53. Complexity

Circular buffer ke saath:

| Operation   | Complexity |
| ----------- | ---------: |
| `enqueue()` |       O(1) |
| `dequeue()` |       O(1) |
| `front()`   |       O(1) |
| `back()`    |       O(1) |
| `empty()`   |       O(1) |
| `size()`    |       O(1) |

Memory:

```text
O(capacity)
```

---

# 54. Queue ka Complete Mental Model

Queue ko yaad rakhne ka best model:

```text
                 capacity
        ┌─────────────────────┐
        ↓                     ↓

      [   ][   ][   ][   ][   ]

        ↑
      front_

                    ↑
                  back_

size_ = number of live elements
```

But:

```text
front_ = first element
back_  = next insertion position
```

Ye sabse important rule hai.

---

# 55. Circular Queue Mental Model

Example:

```text
capacity = 5
```

Suppose:

```text
[50] [60] [ ] [30] [40]
 ↑    ↑         ↑
 0    1         3
```

Agar:

```text
front_ = 3
back_ = 2
size_ = 4
```

Logical order:

```text
30 → 40 → 50 → 60
```

Traversal:

```text
3 → 4 → 0 → 1
```

Formula:

```cpp
(front_ + i) % capacity_
```

---

# 56. Common Mistakes

### Mistake 1

Sochna:

```text
back_ = last element
```

Correct:

```text
back_ = next insertion position
```

---

### Mistake 2

Dequeue mein elements shift karna.

Hamari implementation mein:

```cpp
front_
```

sirf advance hota hai.

---

### Mistake 3

`capacity_` ko current element count samajhna.

Correct:

```text
capacity_ = total slots
size_ = live elements
```

---

### Mistake 4

Pointer copy ko deep copy samajhna.

Correct:

```text
copy → new memory + copied objects
move → resource ownership transfer
```

---

### Mistake 5

Memory allocation aur object construction ko same samajhna.

Correct:

```text
operator new
    ↓
raw memory

construct_at
    ↓
object lifetime begins
```

---

# 57. Testing Strategy

Queue ko sirf normal input se test karna enough nahi hai.

Important cases:

### Empty Queue

```text
size = 0
empty = true
```

Test:

```cpp
dequeue();
front();
back();
```

---

### Single Element

```text
enqueue(10)
dequeue()
```

Check:

```text
empty = true
size = 0
```

---

### FIFO

```text
enqueue(10)
enqueue(20)
enqueue(30)
```

Expected:

```text
10 20 30
```

---

### Full Queue

Capacity:

```text
3
```

Insert:

```text
10 20 30
```

Next insertion should throw.

---

### Wrap Around

Example:

```text
enqueue(10)
enqueue(20)
enqueue(30)

dequeue()

enqueue(40)
```

Check logical order:

```text
20 30 40
```

---

### Copy

Check:

```text
original
copied
```

independent hain ya nahi.

---

### Move

Check:

```text
source
moved
```

resource transfer hua ya nahi.

---

### Const

Check:

```cpp
const CustomQueue<int>& queue
```

par read operations work karte hain.

---

# 58. Benchmark

Benchmark ka purpose implementation ki performance observe karna hai.

Example:

```cpp
constexpr std::size_t N = 1'000'000;
```

Phir:

```text
1,000,000 enqueue operations
1,000,000 dequeue operations
```

ka time measure kiya ja sakta hai.

`std::chrono::steady_clock` timing ke liye use kiya gaya hai.

---

# 59. Why `steady_clock`?

Benchmark mein:

```cpp
std::chrono::steady_clock
```

use karna useful hai because ye duration measurement ke liye designed hai.

Basic pattern:

```cpp
auto start = std::chrono::steady_clock::now();

// operation

auto end = std::chrono::steady_clock::now();
```

Then:

```cpp
end - start
```

se elapsed time milta hai.

---

# 60. Final API

Hamari `CustomQueue<T>`:

```cpp
template <typename T>
class CustomQueue;
```

Main public operations:

```cpp
explicit CustomQueue(std::size_t capacity);

~CustomQueue();

CustomQueue(const CustomQueue& other);

CustomQueue& operator=(const CustomQueue& other);

CustomQueue(CustomQueue&& other) noexcept;

CustomQueue& operator=(CustomQueue&& other) noexcept;

void enqueue(const T& value);

void enqueue(T&& value);

void dequeue();

T& front();

const T& front() const;

T& back();

const T& back() const;

bool empty() const;

std::size_t size() const;
```

---

# 61. CustomQueue — Final Mental Model

Agar poori implementation ko ek mental model mein compress karein:

```text
                 CustomQueue
                      │
                      ↓
              Circular Buffer
                      │
        ┌─────────────┼─────────────┐
        ↓             ↓             ↓
     front_         back_         size_
        │             │             │
        │             │             │
 first element   next insertion   live elements
```

Memory management:

```text
operator new
      ↓
raw storage
      ↓
construct_at
      ↓
live objects
      ↓
destroy_at
      ↓
dead objects
      ↓
operator delete
      ↓
memory released
```

Ownership:

```text
Copy
 ↓
new resource + copied objects

Move
 ↓
resource ownership transfer
```

Performance:

```text
enqueue() → O(1)
dequeue() → O(1)
front()   → O(1)
back()    → O(1)
size()    → O(1)
empty()   → O(1)
```

---

# 62. Most Important Takeaways

CustomQueue se humein sirf Queue data structure nahi seekhna tha.

Is project ke through important C++ engineering concepts:

1. FIFO data structure
2. Circular buffer
3. Modulo-based indexing
4. Logical vs physical order
5. Raw memory allocation
6. Object lifetime
7. `std::construct_at`
8. `std::destroy_at`
9. RAII
10. Manual resource management
11. Deep copy
12. Copy constructor
13. Copy assignment
14. Move constructor
15. Move assignment
16. `std::move`
17. `noexcept`
18. Lvalue/rvalue overloads
19. Const correctness
20. Exception safety
21. O(1) queue operations
22. Testing edge cases
23. Benchmarking
24. CMake
25. Engineering workflow

---

## One-Line Mental Model

> **CustomQueue ek fixed-capacity circular buffer hai jahan `front_` current first element ko point karta hai, `back_` next insertion position ko, aur `size_` live elements ki count maintain karta hai.**
