# linked-list-serializer

Serialize and deserialize a doubly-linked list with random pointers to/from a compact binary format. Reads a human-readable text definition, builds the in-memory structure, and persists it efficiently in O(n) time and space.

## Structure

```
struct ListNode {
    ListNode* prev = nullptr;  // previous node
    ListNode* next = nullptr;  // next node
    ListNode* rand = nullptr;  // arbitrary node in the list or nullptr
    std::string data;          // user data (UTF-8)
};
```

## Requirements

- C++17 compiler: `g++` (GCC 7+), `clang++` (5+), or MSVC 2017+
- No external dependencies

## Build

```bash
g++ -O2 -std=c++17 -o solution solution.cpp
```

## Input format (`inlet.in`)

Plain text file. Each line describes one node:

```
<data>;<rand_index>
```

- `<data>` — UTF-8 string (spaces and special characters allowed; `;` is also allowed inside data since the last `;` is used as delimiter)
- `<rand_index>` — zero-based index of the node pointed to by `rand`, or `-1` if `rand == nullptr`

**Example `inlet.in`:**

```
apple;2
banana;-1
carrot;1
```

| Index | data     | rand points to |
|-------|----------|----------------|
| 0     | `apple`  | node 2 (`carrot`) |
| 1     | `banana` | `nullptr`      |
| 2     | `carrot` | node 1 (`banana`) |

## Run

Place `inlet.in` in the same directory as the binary, then:

```bash
./solution
```

Output:
```
Serialization complete → outlet.out
Deserialization check:
  [0] data="apple"  rand="carrot"
  [1] data="banana"  rand=nullptr
  [2] data="carrot"  rand="banana"
```

The program:
1. Reads and parses `inlet.in`
2. Builds the linked list in memory
3. Serializes it to `outlet.out` (binary)
4. Deserializes `outlet.out` back and prints the result as a sanity check

## Binary format (`outlet.out`)

| Field        | Type      | Description                        |
|--------------|-----------|------------------------------------|
| `count`      | `int32`   | Total number of nodes              |
| — per node — |           |                                    |
| `data_len`   | `int32`   | Length of `data` string in bytes   |
| `data`       | `bytes`   | UTF-8 encoded string               |
| `rand_index` | `int32`   | Index of `rand` node, or `-1`      |

All integers are little-endian.

## Constraints

| Parameter        | Limit              |
|------------------|--------------------|
| Number of nodes  | up to 10⁶          |
| `data` length    | up to 1000 bytes   |

## Testing

**Minimal list (single node):**
```
hello;-1
```
Expected:
```
[0] data="hello"  rand=nullptr
```

**Self-referencing rand:**
```
only;0
```
Expected:
```
[0] data="only"  rand="only"
```

**All nodes point to the first:**
```
first;0
second;0
third;0
```
Expected:
```
[0] data="first"   rand="first"
[1] data="second"  rand="first"
[2] data="third"   rand="first"
```

**Data with semicolons and spaces:**
```
hello; world;-1
a;b;c;0
```
Expected:
```
[0] data="hello; world"  rand=nullptr
[1] data="a;b;c"         rand="hello; world"
```
