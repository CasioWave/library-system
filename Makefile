CC=gcc
CFLAGS=-Wall
TARGET=lms
MAIN=main.c
OBJS=ui.o library.o login.o

all: $(TARGET)

$(TARGET): $(MAIN) $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(OBJS)
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<
clean: 
	rm $(TARGET) $(OBJS)
