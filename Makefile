CC = gcc

CFLAGS = -pg -g -Wall -lSDL_image -lm -lGL `pkg-config --cflags --libs pangocairo` `sdl-config --cflags --libs`				

herdem: main.c herdem.c jaunty/jaunty.c \
	herdem.h herdem_dog.h herdem_sheep.h jaunty/jaunty.h
	$(CC) main.c herdem.c jaunty/jaunty.c \
	   $(CFLAGS) -o herdem


clean:
	rm -f *.o herd_em
