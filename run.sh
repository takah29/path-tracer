wget http://graphics.stanford.edu/pub/3Dscanrep/bunny.tar.gz
tar -zxvf bunny.tar.gz
mkdir models
mv bunny models

clang++ -fopenmp=libiomp5 -Ofast -std=c++1z main.cpp
./a.out

