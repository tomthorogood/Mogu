EXE := create_syntax_map
INCLUDES := -I$(CURDIR)/../src -I/usr/local/lib
LINKS := -lturnleft
PARSERS := $(CURDIR)/../src/Parsers
DEPS := $(PARSERS)/SyntaxReader.cpp $(PARSERS)/SyntaxMapWriter.cpp

all:
	python syntax_generator.py
clean:
	rm -rf syntax.h syntax.py
