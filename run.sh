mkdir models

pushd models
wget http://graphics.stanford.edu/pub/3Dscanrep/bunny.tar.gz
tar -zxvf bunny.tar.gz
rm bunny.tar.gz
popd

clang++ -fopenmp=libiomp5 -Ofast -std=c++1z main.cpp
./a.out

