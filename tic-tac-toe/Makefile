CFLAGS=-Wall -ggdb -std=c11 -pedantic `pkg-config --cflags ncurses`
LIBS=`pkg-config --libs ncurses`

main: main.c
	$(CC) $(CFLAGS) main.c -o main $(LIBS)
