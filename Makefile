CC = gcc

CFLAGS = -pg -g -Wall -lSDL_image -lm -lGL `pkg-config --cflags --libs pangocairo` `sdl-config --cflags --libs`				

herd_em: herd_em.c jaunty/jaunty.c jaunty/jaunty.h
	$(CC) herd_em.c jaunty/jaunty.c \
	   $(CFLAGS) -o herd_em


clean:
	rm -f *.o herd_em
