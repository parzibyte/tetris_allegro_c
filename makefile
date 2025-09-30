CC=gcc
CFLAGS=-Wall
main: main.c
	$(CC) $(CFLAGS) main.c -o main -lallegro -lallegro_font -lallegro_primitives -lallegro_ttf -lallegro_image
	./main