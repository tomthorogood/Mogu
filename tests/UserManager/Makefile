artifact := UserManager
source := $(artifact).Test.cpp
base_dir := $(CURDIR)/../../

CC := g++ -std=c++0x $(flags)

deps := \
	$(base_dir)src/Security/UserManager.cpp \
	$(base_dir)src/Redis/DatabaseConfigReader.cpp \
	$(base_dir)src/Redis/NodeEditor.cpp \
	$(base_dir)src/Types/MoguLogger.cpp \
	$(base_dir)src/Redis/QueryHandler/QueryHandler.cpp \
	$(base_dir)src/Types/NodeValue.cpp \
	$(base_dir)src/Security/Encryption.cpp \
	$(base_dir)src/crypt/BlowfishKey.cpp \
	$(base_dir)src/hash.cpp

libs := -lhiredis -lcrypto -lturnleft -lcpptest


$(artifact):
	$(CC) -o $@ $(source) $(deps) $(libs)

clean:
	rm -f $(artifact)
