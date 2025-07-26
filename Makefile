# Makefile for vibe project

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -I src

# Debug and release modes
DEBUG_FLAGS := -g -O0 -DDEBUG
RELEASE_FLAGS := -O3 -DNDEBUG

# Source and object files
SOURCES := src/vibe.cpp
OBJECTS := $(SOURCES:.cpp=.o)
TARGET := vibe

# Check if json.hpp exists
JSON_HPP := src/json.hpp

# Default target
all: $(TARGET)

# Check dependencies
$(JSON_HPP):
	@echo "Error: $(JSON_HPP) not found!"
	@exit 1

# Build executable
$(TARGET): $(OBJECTS) $(JSON_HPP)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile object files
%.o: %.cpp $(JSON_HPP)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean generated files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run program
run: $(TARGET)
	./$(TARGET)

# Debug version
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release version
release: CXXFLAGS += $(RELEASE_FLAGS)
release: clean $(TARGET)

# Install (Unix-like systems)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	mkdir -p /usr/local/share/vibe
	cp vibe.json /usr/local/share/vibe/

# Windows installation
install-windows: $(TARGET)
	copy $(TARGET).exe C:\Windows\System32\ 2>nul || copy $(TARGET).exe %USERPROFILE%\bin\
	if not exist %USERPROFILE%\share\vibe mkdir %USERPROFILE%\share\vibe
	copy vibe.json %USERPROFILE%\share\vibe\

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Compile program (default)"
	@echo "  clean     - Clean generated files"
	@echo "  run       - Compile and run program"
	@echo "  debug     - Compile debug version"
	@echo "  release   - Compile release version"
	@echo "  install   - Install to system (Unix-like)"
	@echo "  install-windows - Install to user directory (Windows)"
	@echo "  help      - Show this help information"

.PHONY: all clean run debug release install install-windows help
