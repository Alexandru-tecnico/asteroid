

OFILES=config.o main.o jogo.o res.o

PATHG2=/home/alexandru/Desktop/prog/asteroid2016/g2-0.72

PATHX11=/home/alexandru/Desktop/prog/asteroid2016/libX11-1.4.99.1-mingw32/lib

jogo:$(OFILES)
		gcc $(OFILES) -L$(PATHG2) -lg2 -L$(PATHX11) -lX11 -lm -o $@


