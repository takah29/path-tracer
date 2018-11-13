CXX      = clang++
CXXFLAGS = -std=c++17 -Ofast -Wall -Wextra -fopenmp -march=native -MMD -MP
LDFLAGS  = -fopenmp
LIBS     =
INCLUDE  =
SRC_DIR  = ./src
OBJ_DIR  = ./build
SOURCES  = $(shell ls $(SRC_DIR)/*.cpp)
OBJS     = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:.cpp=.o))
TARGET   = main
DEBUG_TARGET = main_debug
DEPENDS  = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if [ ! -d $(OBJ_DIR) ]; \
		then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
	fi
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPENDS)
	$(shell rm -r $(OBJ_DIR))

-include $(DEPENDS)

.PHONY: all clean
