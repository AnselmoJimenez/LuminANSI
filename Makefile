# Program Variables
PROGNAME := LuminANSI
SOURCE_DIR := ./source
INCLUDE_DIR := ./include
BUILD_DIR := ./build

# Compilation Variables
CC := gcc
CFLAGS := -Wall
LDFLAGS := -lm

# Create all corresponding .o files from .c filenames
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: all clean debug

all: $(PROGNAME)

debug: CFLAGS := -Wall -g
debug: $(PROGNAME)

# Link all the object files into the final program
$(PROGNAME): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile each .c file to .o file
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it does not exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	$(RM) -r $(BUILD_DIR)/*.o $(PROGNAME)