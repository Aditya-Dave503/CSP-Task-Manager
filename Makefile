CC = gcc
CFLAGS = -Wall -Wextra -g -I./include
LDFLAGS = 
SRC = src/main.c src/process_list.c src/ui_controller.c src/system_stats.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = task_manager

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
