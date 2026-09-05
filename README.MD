## Build & Run

From the project root:

```bash
mkdir build
cd build
cmake ..
make
```

To run with thread sanitiser

```bash
mkdir build
cd build
cmake -DENABLE_TSAN=ON -DCMAKE_CXX_COMPILER=clang++ ..
make
```

To run the TCP Server:

```bash
cd build
./mini_trader
```

To run the tests:

```bash
cd build/tests
./test_database
```

## Features to Come

### Query Execution

- **Table scan** — full sequential scan over all rows in a table, the foundation for all other query types
- **Predicates & comparisons** — filter rows using expressions like `col > 5`, `col = "foo"`, and compound conditions with `AND` / `OR`

### Indexing

- **B-tree index** — speed up point lookups and range queries on indexed columns
- **Index selection** — query planner automatically picks an index when one exists, falling back to a table scan otherwise

### Query Planner

- **Cost-based optimiser** — estimate I/O and CPU cost per plan and choose the cheapest execution path
- **Join support** — nested-loop and hash join between two tables

### Storage & Durability

- **Write-ahead log (WAL)** — persist changes to a log before applying them so the database can recover after a crash
- **Buffer pool manager** — cache frequently accessed pages in memory to reduce disk I/O
