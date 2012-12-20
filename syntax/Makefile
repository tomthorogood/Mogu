EXE := create_syntax_map
INCLUDES := -I$(CURDIR)/../src -I/usr/local/lib
LINKS := -lturnleft
PARSERS := $(CURDIR)/../src/Parsers
DEPS := $(PARSERS)/SyntaxReader.cpp $(PARSERS)/SyntaxMapWriter.cpp

all:
	touch $(CURDIR)/../src/Types/syntax.h
	g++ -std=c++0x $(INCLUDES) -o $(EXE) $(DEPS) syntax.cpp $(LINKS)
	./$(EXE)

clean:
	rm -rf $(EXE) syntax.h syntax.txt
