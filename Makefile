# Compiler
CPP = g++
# Standard compiler
CPPSTD = -std=c++20
# Warning flags
WFLAGS = -Wall -Wextra
# Optimization flags
OPTFLAGS = -O3 -march=native -finline-functions -funroll-loops -flto
# Include flags
LDFLAGS = -lGL -lglut -lGLU -lm
# Source directory
SRC_DIR = src
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(SRC_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
BINARY = main

all: $(BINARY)

run: $(BINARY)
	./$^

main: $(OBJECTS)
	$(WFLAGS) $(CPP) $(CPPSTD) $(OPTFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp
	$(CPP) $(CPPSTD) -c $< -o $@

# Clean up object files and executable
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BINARY)

foda:
	clear && make clean && make && ./$(BINARY)
