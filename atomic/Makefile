XENO_CONFIG = /usr/xenomai/bin/xeno-config
CFLAGS := $(shell $(XENO_CONFIG) --posix --cflags)
LDFLAGS := $(shell $(XENO_CONFIG) --posix --ldflags) 
CXX := g++ -std=c++11

FILES = atomic_test.cpp
EXECUTABLE = atomic

#LDFLAGS+= -lrtdk
build: $(FILES)
	$(CXX) -o $(EXECUTABLE) $< $(CFLAGS) $(LDFLAGS)
	
clean:
	rm -f $(EXECUTABLE)
	
rebuild: clean build
