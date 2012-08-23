MAIN_SRC := $(CURDIR)/../../src
EXES := clihash

all: $(EXES)

clihash:
	g++ -o $@ -I$(MAIN_SRC) $@.cpp $(MAIN_SRC)/hash.cpp -lcityhash

clean:
	rm -f clihash
