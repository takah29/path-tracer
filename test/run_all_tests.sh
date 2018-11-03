make

echo "------------------"
echo "Run all test code.\n"

echo "run test_vec"
./test_vec
echo "---------------"

echo "run test_utility"
./test_utility
echo "---------------"

echo "run test_tracer"
./test_tracer
echo "---------------"

echo "run test_scene"
./test_scene
echo "---------------"

echo "run test_random"
./test_random
echo "---------------"

echo "run test_object"
./test_object
echo "---------------"

echo "run test_camera"
./test_camera
echo "---------------"

echo "run test_bvh"
./test_bvh
echo "---------------"

echo "run test_loader"
./test_loader
echo "---------------"
