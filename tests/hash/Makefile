artifact := Hash
source = $(artifact).Test.cpp
base_dir = $(CURDIR)/../../

CC := g++ -std=c++0x $(flags)

deps = $(base_dir)src/hash.cpp

$(artifact):
	$(CC) -o $@ $(source) $(deps) -lcrypto -lcpptest

clean:
	rm -f $(artifact)
