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
