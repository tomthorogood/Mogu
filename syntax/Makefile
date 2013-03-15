PY_SYN_DIR := $(CURDIR)/../cli/src/importer
CP_SYN_DIR := $(CURDIR)/../src/Types

all: operators
	ln -s $(PY_SYN_DIR)/pyRex pyRex
	ln -s $(PY_SYN_DIR)/regexlib.py regexlib.py
	python syntax_generator.py

install:
	ln -s $(CURDIR)/syntax.py $(PY_SYN_DIR)/syntax.py
	ln -s $(CURDIR)/syntax.py $(PY_SYN_DIR)/../exporter/syntax.py
	cp syntax.h $(CP_SYN_DIR)/

test:
	@echo $(PY_SYN_DIR)
	@echo $(CP_SYN_DIR)

clean:
	unlink pyRex
	unlink regexlib.py
	unlink $(PY_SYN_DIR)/syntax.py
	unlink $(PY_SYN_DIR)/../exporter/syntax.py
	rm -rf syntax.h syntax.py
	rm -rf operators

operators:
	g++ --std=c++0x -o operators operators.cpp
