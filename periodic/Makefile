export LD_LIBRARY_PATH=/usr/xenomai/lib
PERIOD:= 1
XENO_CONFIG = /usr/xenomai/bin/xeno-config
CFLAGS := $(shell $(XENO_CONFIG) --alchemy --cflags)
LDFLAGS := $(shell $(XENO_CONFIG) --alchemy --ldflags) 
CC := $(shell $(XENO_CONFIG) --cc)

FILES = periodic.c
EXECUTABLE = periodic_test

#LDFLAGS+= -lrtdk
build: $(FILES)
	$(CC) -o $(EXECUTABLE) $< $(CFLAGS) $(LDFLAGS)
	
clean:
	rm -f $(EXECUTABLE)
	
rebuild: clean build

run:
	./$(EXECUTABLE) $(PERIOD) 