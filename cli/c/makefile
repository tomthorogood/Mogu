MAIN_SRC := $(CURDIR)/../../src
EXES := clihash clidec
dbg = NODEBUG

dec_deps := hash.cpp crypt/BlowfishKey.cpp crypt/PacketCenter.cpp crypt/Packet.cpp Security/Encryption.cpp
dec_deps_r := $(patsubst %, $(MAIN_SRC)/%, $(dec_deps))

libs = -lturnleft -lcityhash -lhiredis -lcrypto

all: $(EXES)

clihash:
	g++ -o $@ -I$(MAIN_SRC) $@.cpp $(MAIN_SRC)/hash.cpp -lwt -lcityhash

clidec:
	g++ -o $@ -D$(dbg) -I$(MAIN_SRC) $@.cpp $(dec_deps_r) $(libs)

clean:
	rm -f $(EXES)
