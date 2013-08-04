artifact := Encryption
source := $(artifact).Test.cpp

base_dir := $(CURDIR)/../..

deps = $(base_dir)/src/crypt/BlowfishKey.cpp $(base_dir)/src/Security/Encryption.cpp

CC = g++ -std=c++0x $(flags)

$(artifact):
	$(CC) -o $@ $(source) $(deps) -lcpptest -lcrypto

clean:
	rm -rf obj
	rm -f $(artifact)
