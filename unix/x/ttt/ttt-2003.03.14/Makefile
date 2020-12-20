CFLAGS=-Wall -O2 $(SDL_CFLAGS)
LIBS=$(SDL_LIBS)

SDL_CFLAGS=$(shell sdl-config --cflags)
SDL_LIBS=$(shell sdl-config --libs) -lSDL_mixer


all:    ttt

clean:
	-rm ttt
	-rm ttt.o

ttt: ttt.o
	$(CC) $(CFLAGS) ttt.o -o ttt $(LIBS)

ttt.o:       ttt.c
