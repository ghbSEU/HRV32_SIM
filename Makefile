CC := gcc
target := hrv32sim

src := mem.c hrv32.c dec_exc.c rvcdecoder.c

all: $(src)
	$(CC) $(src) -o $(target)

run:
	./$(target)

clean:
	rm $(target) instlog.txt cpulog.txt dmem.txt dmemlog.txt
	touch instlog.txt
	chmod 666 instlog.txt
	touch cpulog.txt
	chmod 666 cpulog.txt
