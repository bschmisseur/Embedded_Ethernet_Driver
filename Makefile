##
# @file MakeFile
#
# @brief Makefile implements targets to run bash commands to build, clean, and test the projects
#
# @author Bryce Schmisseur
#

# C++ Project settings
COMPONENT_NAME = embedded_ethernet_driver
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude

# Directory Paths
SRC_DIR := src
BUILD_DIR := build
TEST_DIR := tests
WINDOWS_BUILD_DIR = $(BUILD_DIR)/win
LINUX_BUILD_DIR = $(BUILD_DIR)/lin
OBJS_DIR = $(LINUX_BUILD_DIR)/objs
EXE_DIR = $(LINUX_BUILD_DIR)/exe

TARGET := $(COMPONENT_NAME)
TARGET_PATH = $(EXE_DIR)/$(TARGET)
TEST_TARGET := $(BUILD_DIR)/test_runner

UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
	MKDIR = mkdir -p
else
	MKDIR = mkdir.exe
	OBJS_DIR := $(WINDOWS_BUILD_DIR)/objs
	EXE_DIR := $(WINDOWS_BUILD_DIR)/exe
	BUILD_DIR := $(subst /,\\,$(BUILD_DIR))
	OBJS_DIR := $(subst /,\\,$(OBJS_DIR))
	EXE_DIR := $(subst /,\\,$(EXE_DIR))
	TARGET_PATH := $(subst /,\\,$(TARGET_PATH)).exe
	TARGET := $(COMPONENT_NAME).exe
endif

# Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJS_DIR)/%.o)

TEST_SRCS := $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.test.o,$(TEST_SRCS))

.PHONY: all clean test

# Default target
all: $(EXE_DIR) $(OBJS_DIR) $(TARGET_PATH)

##
# Main Build Targets
#
$(TARGET_PATH): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile source files
$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

##
# Unit testing Targets
#
test: $(OBJS) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $(TEST_TARGET)
	./$(TEST_TARGET)

$(BUILD_DIR)/%.test.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

##
# Directory Creation Targets
#
$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)
	$(MKDIR) $(WINDOWS_BUILD_DIR)
	$(MKDIR) $(LINUX_BUILD_DIR)

$(EXE_DIR): $(BUILD_DIR)
	$(MKDIR) $(EXE_DIR)

$(OBJS_DIR): $(BUILD_DIR)
	$(MKDIR) $(OBJS_DIR)


##
# Clean Targets
#
clean:
	rm -rf $(BUILD_DIR)

##
# Run Targets
#
run:
	$(TARGET_PATH)
