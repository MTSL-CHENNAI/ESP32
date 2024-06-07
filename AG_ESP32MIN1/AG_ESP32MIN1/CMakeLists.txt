# Compiler
CC = clang
CXX = clang++

# Compiler flags
CFLAGS = -Wall -Wextra -Werror
CXXFLAGS = $(CFLAGS) -std=c++11

# Directories
SRC_DIR = src
INCLUDE_DIR = include
EEPROM_INCLUDE_DIR = eeprom_include
BUILD_DIR = build

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
EXECUTABLE = your_executable

# Targets
.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BUILD_DIR)/$@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -I$(EEPROM_INCLUDE_DIR) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

# Additional rules for tests, coverage, etc. can be added here
