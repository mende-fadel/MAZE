CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = maze

OBJS = maze.o maze_solve.o main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean