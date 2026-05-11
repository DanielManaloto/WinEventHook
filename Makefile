# ==============================
# Project Configuration
# ==============================

PROJECT = WinEventHook

CC = gcc

CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -luser32

SRC_DIR = src
BUILD_DIR = build
DEBUG_DIR = $(BUILD_DIR)/debug

# ==============================
# Source Files
# ==============================

SRC = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/input_recorder.c \
	$(SRC_DIR)/input_replay.c \
	$(SRC_DIR)/ui.c \
	$(SRC_DIR)/file_handler.c

# Convert src/file.c -> build/debug/file.o
OBJ = $(SRC:$(SRC_DIR)/%.c=$(DEBUG_DIR)/%.o)

# ==============================
# Targets
# ==============================

TARGET = $(DEBUG_DIR)/$(PROJECT).exe

all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# Compile
$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(DEBUG_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ==============================
# Utilities
# ==============================

clean:
	rm -rf $(BUILD_DIR)

run: all
	./$(TARGET)

# ==============================
# Phony Targets
# ==============================

.PHONY: all clean run