CC = gcc

CFLAGS = -pg -g -Wall -lSDL_image -lm -lGL `pkg-config --cflags --libs pangocairo` `sdl-config --cflags --libs`				

all: herdem

herdem: main.c herdem.c herdem_dog.c herdem_sheep.c herdem_timer.c\
   	herdem_info_board.c herdem_saved_tally.c \
	levels/inter_levels.c \
	levels/level1.c levels/level2.c levels/level3.c \
	levels/level4.c levels/level5.c levels/level6.c \
	levels/level7.c \
	jaunty/jaunty.c \
	herdem.h herdem_dog.h herdem_sheep.h herdem_timer.c \
	herdem_info_board.h herdem_saved_tally.h \
	levels/levels.h	\
	jaunty/jaunty.h
	$(CC) main.c herdem.c herdem_dog.c herdem_sheep.c herdem_timer.c \
		herdem_info_board.c herdem_saved_tally.c \
		levels/inter_levels.c \
		levels/level1.c levels/level2.c levels/level3.c \
		levels/level4.c levels/level5.c levels/level6.c \
		levels/level7.c \
		jaunty/jaunty.c \
	   $(CFLAGS) -o herdem


clean:
	rm -f *.o herd_em
