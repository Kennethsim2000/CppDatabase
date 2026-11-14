# C++ Database Project --- Assistant Skill / Project Context

## Purpose

Use this document as the working context when helping Kenneth continue his C++ database project. The project is a toy relational database built from scratch in C++17. The public documentation is split into two parts:

- Part 1 --- Storage engine: https://portfolio-kenneth-azure.vercel.app/cppdb1
- Part 2 --- Query layer: https://portfolio-kenneth-azure.vercel.app/cppdb2

Read the relevant page before making architectural recommendations. These pages describe documented milestones, not necessarily the exact latest state of the local source code.

## How to collaborate

The user prefers:

1. Work one step at a time.
2. Explain the underlying concept and why a change is needed before proposing code.
3. Prefer a small, focused change over a large rewrite.
4. Do not dump an entire implementation unless explicitly requested.
5. Preserve the existing architecture and naming conventions where practical.
6. When changing code, identify the relevant header/source/test files and explain their responsibilities.
7. Ask for the latest relevant source files when exact implementation details are needed. Do not assume the website's code snippets match the current working tree.
8. For debugging, reason from the actual failing test, stack trace, and source rather than guessing.

## Project technology and conventions

- Language standard: C++17
- Build system: CMake
- Tests: GoogleTest
- Namespace used in the documented database components: `db`
- Typical organization separates headers and implementations (for example, `include/db/...` and `src/...`).
- The user is interested in systems programming concepts, including memory lifetime, move semantics, concurrency, cache behavior, and correctness.

## Documented architecture

### Part 1 --- Storage engine

Source: https://portfolio-kenneth-azure.vercel.app/cppdb1

The documented storage layer contains:

- `Page`
  - Fixed page size: `PAGE_SIZE = 4096`.
  - Uses a byte buffer to hold page contents.
  - Tracks page ID, dirty state, and pin count.
  - Provides byte access and bounds-checked `read<T>()` / `write<T>()` operations using `std::memcpy`.
- `DiskManager`
  - Reads and writes fixed-size binary pages.
  - Computes disk offset as `page_id * PAGE_SIZE`.
  - Zero-fills unread portions when reading beyond the file's current contents.
- `LRUReplacer`
  - Tracks frames eligible for eviction.
  - Uses a `std::list` for LRU order and a `std::unordered_map` for iterator lookup.
  - Frames with non-zero pin counts must not be evicted.
- `BufferPoolManager`
  - Coordinates in-memory frames, page lookup, disk reads/writes, pinning, and eviction.
  - Works with the page table, free-frame tracking, and LRU replacement.
- `Tuple` and `RID`
  - Represent logical records and their record identifiers.
- `TablePage`
  - Stores tuples in a page-oriented layout.
- `TableHeap`
  - Provides table-level tuple insertion, retrieval, and scanning over table pages.

### Part 2 --- Query layer

Source: https://portfolio-kenneth-azure.vercel.app/cppdb2

The documented query layer contains:

- `TypeId`: `INTEGER`, `BIGINT`, `BOOLEAN`, and `VARCHAR`.
- `Value`
  - Represents a typed value.
  - Uses factory methods such as `Integer`, `BigInt`, `Boolean`, and `Varchar`.
  - Provides typed accessors and comparisons.
- `Column`
  - Stores a column name and type.
- `Schema`
  - Stores ordered column definitions.
  - Supports column count, lookup by index, lookup by name, and adding columns.
- `ComparisonType`
  - Supports equality, inequality, less-than, less-than-or-equal, greater-than, and greater-than-or-equal.
- `Expression`
  - Abstract interface with `evaluate(const Tuple&, const Schema&) const`.
- `Predicate`
  - Evaluates a comparison against a named column and a constant `Value`.
- `CompoundPredicate`
  - Combines expressions using logical `AND` and `OR`.
  - Uses short-circuit evaluation.
- `Executor`
  - Common pull-based interface: `init()` and `next(Tuple&, RID&)`.
  - `next()` returns whether another tuple is available.
- `SeqScanExecutor`
  - Scans a `TableHeap`, retrieves tuples by RID, evaluates an expression, and returns matching tuples incrementally.
  - The documented version first materializes a vector of RIDs during `init()`.
- `Projection`
  - Resolves requested column names against an input schema.
  - Builds an output schema and remembers input column indices.
  - Produces a tuple containing only the selected values.
- `ProjectionExecutor`
  - Wraps a child `Executor`.
  - Pulls tuples from the child and applies `Projection`.
  - Preserves the source RID and exposes the output schema.

The documented example composes a `CompoundPredicate`, `SeqScanExecutor`, and `ProjectionExecutor` to filter rows and then select a subset of columns.

## Additional project context from prior discussions

These details came from previous project conversations and should be verified against current source before relying on them:

- The project has a B+ tree implementation.
- The discussed B+ tree node representation included a leaf/internal-node flag, keys, leaf values that can hold multiple RIDs per key, child pointers for internal nodes, and parent/next-leaf pointers.
- The discussed internal-node separator convention was that `keys[i]` represents the minimum key in child `i + 1`.
- `IndexScanExecutor` has been implemented or worked on, and its tests were discussed.
- A previous debugging session encountered a segmentation fault at `BPlusTreeTest.SearchEmptyTree`. Do not assume this bug is still present; ask for the current test output and implementation if relevant.

## Guidance for future implementation work

When asked to implement or extend a feature:

1. First identify which layer owns the responsibility (storage, tuple/schema, expression, executor, or index).
2. Explain how the new component fits into the existing architecture.
3. State the smallest set of files likely to change.
4. Start with interfaces and data flow before implementation details.
5. Make one focused change at a time and wait for the user's feedback or test result.
6. Include or update GoogleTest coverage where appropriate.
7. Consider edge cases, ownership/lifetime, invalid input, and failure behavior.
8. Do not silently change public interfaces or invariants.

When asked to debug:

1. Identify the exact failing test or runtime path.
2. Trace relevant state transitions and ownership.
3. Check invariants (page IDs, pin counts, iterator validity, node key/child counts, RID validity, schema/type agreement).
4. Propose the next diagnostic or smallest fix, not a speculative rewrite.
5. Explain how to verify the fix with targeted tests and then the full test suite.

## Important caveat

This file is a reusable project orientation guide, not a substitute for the current source tree. The website is the baseline for documented design and progress. The latest source files, tests, compiler output, and runtime behavior are authoritative for implementation-level work.
