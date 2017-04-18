export LD_LIBRARY_PATH=/usr/xenomai/lib
XENO_CONFIG = /usr/xenomai/bin/xeno-config
CFLAGS := $(shell $(XENO_CONFIG) --posix --cflags)
LDFLAGS := $(shell $(XENO_CONFIG) --posix --ldflags) 
CC := $(shell $(XENO_CONFIG) --cc)

FILES = gpio_ptask.c gpio_task.h
EXECUTABLE = gpio_ptask

#LDFLAGS+= -lrtdk
build: $(FILES)
	$(CC) -o $(EXECUTABLE) $< $(CFLAGS) $(LDFLAGS)
	
clean:
	rm -f $(EXECUTABLE)
	
rebuild: clean build

run:
	./$(EXECUTABLE) $(PERIOD) 