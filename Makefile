TARGET=todo_app.exe
SRCS=main.c
CFLAGS=-g -Wall -Wextra -Werror
CC=gcc

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

do: $(TARGET)
	.\$(TARGET)