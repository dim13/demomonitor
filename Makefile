demo: demomonitor.c Graph.c GraphDisplay.c BarDisplay.c
	$(CC) $(CFLAGS) -o $@ $^ `pkg-config --cflags --libs xt`
