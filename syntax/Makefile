PY_SYN_DIR := $(CURDIR)/../cli/src/moguio/moguio
CP_SYN_DIR := $(CURDIR)/../src/Types

all: $(PY_SYN_DIR)/syntax.py $(CP_SYN_DIR)/syntax.h

$(PY_SYN_DIR)/syntax.py: syntax.py
	ln -s $(CURDIR)/syntax.py $(PY_SYN_DIR)/syntax.py

$(CP_SYN_DIR)/syntax.h: syntax.py
	ln -s $(CURDIR)/syntax.h $(CP_SYN_DIR)/syntax.h

clean:
	unlink $(PY_SYN_DIR)/syntax.py
	unlink $(CP_SYN_DIR)/syntax.h
	rm -rf syntax.h syntax.py
	rm -rf operators

syntax.py : operators
	$(CURDIR)/check
	python syntax_generator.py

operators:
	g++ --std=c++0x -o operators operators.cpp

