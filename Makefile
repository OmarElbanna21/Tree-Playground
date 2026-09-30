CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = tree_playground
SRCS   = main.c heap.c bst.c avl.c hashtable.c display.c queue.c stack.c utils.c
OBJS   = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
