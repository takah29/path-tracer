CXX      = clang++
CXXFLAGS = -std=c++1z
SRC_DIR  = ./../src
TESTS  = $(shell ls *.cpp)
TARGET = $(TESTS:.cpp=)

all: $(TARGET)

test_vec: test_vec.cpp $(SRC_DIR)/vec.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_utility: test_utility.cpp $(SRC_DIR)/utility.cpp $(SRC_DIR)/vec.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_camera: test_camera.cpp $(SRC_DIR)/camera.cpp $(SRC_DIR)/vec.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_object: test_object.cpp $(SRC_DIR)/object.cpp $(SRC_DIR)/vec.cpp \
	$(SRC_DIR)/ray.cpp $(SRC_DIR)/material.cpp $(SRC_DIR)/texture.cpp \
	$(SRC_DIR)/lattice_noise.cpp $(SRC_DIR)/random.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_scene: test_scene.cpp $(SRC_DIR)/scene.cpp $(SRC_DIR)/utility.cpp \
	$(SRC_DIR)/bvh.cpp $(SRC_DIR)/ray.cpp $(SRC_DIR)/object.cpp \
	$(SRC_DIR)/random.cpp $(SRC_DIR)/tracer.cpp $(SRC_DIR)/vec.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_tracer: test_tracer.cpp $(SRC_DIR)/tracer.cpp $(SRC_DIR)/vec.cpp \
	$(SRC_DIR)/random.cpp $(SRC_DIR)/bvh.cpp $(SRC_DIR)/bvh.cpp \
	$(SRC_DIR)/ray.cpp $(SRC_DIR)/object.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_bvh: test_bvh.cpp $(SRC_DIR)/bvh.cpp $(SRC_DIR)/vec.cpp \
	$(SRC_DIR)/object.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

test_random: test_random.cpp $(SRC_DIR)/random.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) $(TARGET)

.PHONY: all clean
