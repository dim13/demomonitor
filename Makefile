demo: demomonitor.c Graph.c GraphDisplay.c BarDisplay.c
	$(CC) -o $@ `pkg-config --cflags --libs xt` $^
