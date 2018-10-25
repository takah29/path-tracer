mkdir models

pushd models
wget http://graphics.stanford.edu/pub/3Dscanrep/bunny.tar.gz
tar -zxvf bunny.tar.gz
rm bunny.tar.gz
popd

make
./main

