##
# @file MakeFile
#
# @brief Makefile implements targets to run bash commands to build, clean, and run the project
#
# @author Bryce Schmisseur
#

# C++ Project settings
COMPONENT_NAME = ethernet_driver_simulator
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude
LINKER_FLAGS := -lavformat -lavcodec -lavutil -lswscale -lavdevice

# Directory Paths
SRC_DIR := src
BUILD_DIR := build
RELEASE_DIR := release
WINDOWS_BUILD_DIR = $(BUILD_DIR)/win
LINUX_BUILD_DIR = $(BUILD_DIR)/lin
OBJS_DIR = $(LINUX_BUILD_DIR)/objs
EXE_DIR = $(LINUX_BUILD_DIR)/exe
RELEASE_BIN_DIR = $(RELEASE_DIR)/bin
FFMPEG_DIR = cots/ffmpeg
RELEASE_TARGET = $(RELEASE_BIN_DIR)/$(TARGET)
TARGET := $(COMPONENT_NAME)
TARGET_PATH = $(EXE_DIR)/$(TARGET)

UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
	MKDIR = mkdir -p
	DISCLAIMER = 
	PREFIX_TARGET = ./
else
	MKDIR = mkdir.exe

	CURRENT_DIR = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

	CURRENT_DIR := $(subst /,\\,$(CURRENT_DIR))
	OBJS_DIR := $(WINDOWS_BUILD_DIR)/objs
	EXE_DIR := $(WINDOWS_BUILD_DIR)/exe
	BUILD_DIR := $(subst /,\\,$(BUILD_DIR))
	OBJS_DIR := $(subst /,\\,$(OBJS_DIR))
	EXE_DIR := $(subst /,\\,$(EXE_DIR))
	FFMPEG_DIR := $(subst /,\\,$(FFMPEG_DIR))
	FFMPEG_DIR := $(subst /,\\,$(FFMPEG_DIR))
	RELEASE_BIN_DIR := $(subst /,\\,$(RELEASE_BIN_DIR))
	RELEASE_TARGET := $(subst /,\\,$(RELEASE_TARGET)).exe
	TARGET_PATH := $(subst /,\\,$(TARGET_PATH)).exe
	TARGET := $(COMPONENT_NAME).exe
	CXXFLAGS := $(CXXFLAGS) -I$(FFMPEG_DIR)\\include -L$(FFMPEG_DIR)\\lib

	DISCLAIMER = !! To run on Windows please add $(CURRENT_DIR)..\$(FFMPEG_DIR)\bin to your environment path variable !!
	PREFIX_TARGET =
endif

# Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJS_DIR)/%.o)

.PHONY: all clean run

# Default target
all: $(EXE_DIR) $(OBJS_DIR) $(TARGET_PATH) dist

##
# Main Build Targets
#
$(TARGET_PATH): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LINKER_FLAGS) $^ -o $@

# Compile source files
$(OBJS_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

##
# Clean Targets
#
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(RELEASE_DIR)

##
# Run Targets
#
run:
	@echo $(DISCLAIMER)
	@$(PREFIX_TARGET)$(RELEASE_TARGET)

run_dev: 
	@echo $(DISCLAIMER)
	@$(PREFIX_TARGET)$(TARGET_PATH)

##
# Distribution Targets
#
dist: all $(RELEASE_BIN_DIR)
	cp $(TARGET_PATH) $(RELEASE_TARGET)

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

$(RELEASE_BIN_DIR): $(RELEASE_DIR)
	$(MKDIR) $(RELEASE_BIN_DIR)

$(RELEASE_DIR):
	$(MKDIR) $(RELEASE_DIR)
