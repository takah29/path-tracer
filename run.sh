make

mkdir models
cd models
wget http://graphics.stanford.edu/pub/3Dscanrep/bunny.tar.gz
tar -zxvf bunny.tar.gz
rm bunny.tar.gz
cd ..

./main -n 3 -w 320 -h 240 -s 2 -ss 8

