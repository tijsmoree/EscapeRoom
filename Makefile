CC = gcc
CFLAGS = -ansi -Wall -pedantic

main.exe: main.o
	$(CC) $(CFLAGS) -o main.exe main.c

clean: 
	$(RM) $(TARGET).exe *.o *~