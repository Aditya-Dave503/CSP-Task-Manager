# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -I./include
LDFLAGS = -lpthread -lrt

# Source files (Only the working ones for now)
SRC_DIR = src
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/process_list.c $(SRC_DIR)/ui_controller.c \
      $(SRC_DIR)/utils.c $(SRC_DIR)/system_stats.c
OBJ = $(SRC:.c=.o)

# Target executable
TARGET = task_manager

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)

debug: $(TARGET)
	gdb ./$(TARGET)
