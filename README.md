# Tree Playground 🌳

A terminal-based C project that implements, visualizes, and compares the core data structures from a second-year Computer Engineering curriculum: **BST**, **AVL Tree**, **Min/Max Heap**, and **Hash Table**.

Every operation prints a visual tree to the terminal, shows step-by-step execution, and tracks complexity (comparisons, swaps) and memory usage in real time.

---

## Project Structure

```
tree_playground/
├── main.c              ← Main menu + all sessions
├── display.c / .h      ← Terminal tree display engine
├── heap.c / .h         ← Min / Max Heap
├── bst.c / .h          ← Binary Search Tree
├── avl.c / .h          ← AVL Tree (self-balancing)
├── hashtable.c / .h    ← Linear Probing + Separate Chaining
├── queue.c / .h        ← Integer queue (used in Level-Order)
├── stack.c / .h        ← Integer stack (used in iterative traversals)
├── utils.c / .h        ← Complexity Tracker + Memory Tracker
└── Makefile
```

---

## Build & Run

```bash
make
./tree_playground
```

To clean object files:
```bash
make clean
```

**Requirements:** GCC, standard C99, `libm` (linked automatically via Makefile).

---

## Features

### Heap (Min / Max)

| Feature | Notes |
|---|---|
| Insert | Step-by-step heapifyUp after each insert |
| Extract root | Step-by-step heapifyDown |
| Delete value | Combines heapifyUp + heapifyDown |
| Search | Linear scan |
| Build from array | O(n) bottom-up heapify |
| Heap Sort | Step-by-step, works on a copy |
| Toggle Min/Max | Separate sessions from main menu |

**Storage:** Array-based. Index math: `parent = (i-1)/2`, `left = 2i+1`, `right = 2i+2`.

---

### BST (Binary Search Tree)

| Feature | Notes |
|---|---|
| Insert | Recursive, prints path taken |
| Delete | Handles all 3 cases (leaf, one child, two children) |
| Search | Iterative, prints each comparison |
| Inorder / Preorder / Postorder | Recursive |
| Level-Order | BFS using pointer queue |
| Iterative Inorder | Uses explicit pointer stack, step-by-step option |
| Iterative Preorder | Uses explicit pointer stack, step-by-step option |
| Level-Order step-by-step | Shows queue contents after each level |
| Stats | Height, node count, BST validity check |
| Clear | Frees all nodes recursively |

---

### AVL Tree

Extends BST with automatic rebalancing after every insert and delete.

| Feature | Notes |
|---|---|
| Insert | Triggers rebalance on the way back up |
| Delete | In-order successor strategy + rebalance |
| Search | Same as BST |
| All traversals | Same as BST |
| Level-Order step-by-step | Shows balance factor per node |
| Display | Balance factors printed below each node value |
| Stats | Height, node count, root balance factor |

**Four rotation cases:**

| Case | Condition | Fix |
|---|---|---|
| LL | BF = +2, left child BF ≥ 0 | Right rotation |
| RR | BF = -2, right child BF ≤ 0 | Left rotation |
| LR | BF = +2, left child BF < 0 | Left then Right rotation |
| RL | BF = -2, right child BF > 0 | Right then Left rotation |

Balance Factor = `height(left) - height(right)`. Valid range: `{-1, 0, +1}`.

---

### Hash Table

Two collision resolution strategies implemented side by side.

#### Linear Probing

- Hash function: `h(k) = k % capacity`
- Collision: probe `(h+1) % cap`, `(h+2) % cap`, ...
- Deletion: lazy (marks slot as `DELETED`, not `EMPTY`)
- Display: full table with slot status (EMPTY / OCCUPIED / DELETED)

#### Separate Chaining

- Each slot is a singly linked list
- Insert at head of chain → O(1)
- Display: each bucket shown as `[idx] → [k1] → [k2] → NULL`

**Compare option (menu item 11):** inserts the same key into both tables simultaneously and shows how each handles it.

---

### Display Engine

Both display modes produce the same visual top-down tree format:

```
         4
        +0
      /   \
     2       6
    +0      +0
  /   \    /   \
 1     3  5     7
+0    +0 +0    +0
```

**Heap display** (`printHeapTree`): uses array index math directly.

**Virtual tree display** (`printVirtualTree`): used for BST and AVL. The caller fills a virtual array via recursive pre-order BFS (root at index 0, left child at `2i+1`, right child at `2i+2`). Empty slots are marked with `EMPTY_SLOT` sentinel and printed as blanks. AVL balance factors are overlaid on a second row when `showBalance = 1`.

Maximum display height is capped at **6 levels** to keep terminal output readable.

---

### Complexity Tracker (`utils`)

A global `CompTracker` struct counts comparisons and swaps per operation.

```c
resetTracker("BST Insert");   // call before operation
countComparison();            // call at every key comparison
printStats();                 // call after operation
```

Output example:
```
┌─ Complexity: BST Insert ─────────────┐
│  Comparisons : 3                     │
│  Swaps/Moves : 0                     │
└──────────────────────────────────────┘
```

---

### Memory Tracker (`utils`)

A global `MemTracker` struct records bytes allocated and freed.

```c
trackAlloc(sizeof(BSTNode));   // call after every malloc
trackFree(sizeof(BSTNode));    // call before every free
printMemStats();               // prints current usage
```

Output example:
```
┌─ Memory Usage ─────────────────────┐
│  Currently allocated : 320 bytes   │
│  Total ever allocated: 560 bytes   │
│  Active allocations  : 10          │
└────────────────────────────────────┘
```

---

### Compare Mode

Inserts the same sequence of values into a BST, an AVL Tree, and a Max Heap simultaneously, then displays all three and prints a summary table:

```
Structure       Height   Nodes
BST             10       10
AVL             4        10
MaxHeap         4        10

AVL saved 6 levels vs BST → O(log n) guaranteed!
```

---

### Worst Case Demo

Inserts `1, 2, 3, ..., 10` (sorted order) into both a BST and an AVL Tree to demonstrate BST's worst case: sorted input causes BST to degenerate into a linked list with height = n, while AVL self-balances and maintains height = ⌊log₂ n⌋ + 1.

---

## Queue & Stack Modules

These are standalone modules used internally by the traversal functions.

**Queue (`queue.h`):** Linked-list based integer queue. Used for Level-Order (BFS) traversal.

| Function | Description |
|---|---|
| `createQueue()` | Allocates empty queue |
| `enqueue(q, val)` | Add to rear — O(1) |
| `dequeue(q)` | Remove from front — O(1) |
| `queuePeek(q)` | View front without removing |
| `destroyQueue(&q)` | Frees all nodes |

**Stack (`stack.h`):** Dynamic array-based integer stack. Used for iterative Inorder and Preorder traversal demonstrations.

| Function | Description |
|---|---|
| `createStack(cap)` | Allocates stack with initial capacity |
| `push(s, val)` | Push — O(1) amortized |
| `pop(s)` | Pop — O(1) |
| `stackPeek(s)` | View top without removing |
| `destroyStack(&s)` | Frees memory |

---

## Algorithms Reference

| Operation | BST avg | BST worst | AVL guaranteed | Heap |
|---|---|---|---|---|
| Insert | O(log n) | O(n) | O(log n) | O(log n) |
| Delete | O(log n) | O(n) | O(log n) | O(log n) |
| Search | O(log n) | O(n) | O(log n) | O(n) |
| Build from array | O(n log n) | O(n²) | O(n log n) | **O(n)** |
| Sort | O(n log n) | O(n²) | — | O(n log n) |
| Hash Insert | — | — | — | O(1) avg |
| Hash Search | — | — | — | O(1) avg |

---

## Author

Omar El-Banna — CCE Year 2, Alexandria University, Faculty of Engineering.
Data Structures.

## License
Released under the MIT License. Developed as a course project for Data Structures (1), Faculty of Engineering, Alexandria University. 
