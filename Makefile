# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c17
LDFLAGS = 

# Directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/program

# Default target
all: compile_commands.json $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if they don't exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) .cache
	@echo "Clean complete"

# Generate compile_commands.json for clangd
compile_commands.json: $(SOURCES)
	@echo "[" > compile_commands.json
	@for src in $(SOURCES); do \
		obj=$$(echo $$src | sed 's|$(SRC_DIR)|$(OBJ_DIR)|;s|\.c$$|.o|'); \
		echo "  {" >> compile_commands.json; \
		echo "    \"directory\": \"$$(pwd)\"," >> compile_commands.json; \
		echo "    \"command\": \"$(CC) $(CFLAGS) -c $$src -o $$obj\"," >> compile_commands.json; \
		echo "    \"file\": \"$$src\"" >> compile_commands.json; \
		if [ "$$src" = "$$(echo $(SOURCES) | awk '{print $$NF}')" ]; then \
			echo "  }" >> compile_commands.json; \
		else \
			echo "  }," >> compile_commands.json; \
		fi \
	done
	@echo "]" >> compile_commands.json
	@echo "Generated compile_commands.json"

# Phony targets
.PHONY: all run clean compile_commands.json
