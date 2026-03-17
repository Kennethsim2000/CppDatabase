mkdir build
cd build
cmake -DENABLE_TSAN=ON -DCMAKE_CXX_COMPILER=clang++ ..
make
