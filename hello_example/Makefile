XENO_CONFIG = /usr/xenomai/bin/xeno-config
CFLAGS := $(shell $(XENO_CONFIG) --alchemy --cflags)
LDFLAGS := $(shell $(XENO_CONFIG) --alchemy --ldflags) 
CC := $(shell $(XENO_CONFIG) --cc)


#LDFLAGS+= -lrtdk

EXECUTABLE := hello

all: $(EXECUTABLE)

%: %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)