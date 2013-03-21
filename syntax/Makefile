PY_SYN_DIR := $(CURDIR)/../cli/src/moguio/moguio
CP_SYN_DIR := $(CURDIR)/../src/Types

all: $(PY_SYN_DIR)/syntax.py

$(PY_SYN_DIR)/syntax.py: syntax.py
	ln -s $(CURDIR)/syntax.py $(PY_SYN_DIR)/syntax.py
	cp syntax.h $(CP_SYN_DIR)/

test:
	@echo $(PY_SYN_DIR)
	@echo $(CP_SYN_DIR)

clean:
	unlink regexlib.py
	unlink $(PY_SYN_DIR)/syntax.py
	rm -rf syntax.h syntax.py
	rm -rf operators

syntax.py : operators
	$(CURDIR)/check
	ln -s $(PY_SYN_DIR)/regexlib.py regexlib.py
	python syntax_generator.py

operators:
	g++ --std=c++0x -o operators operators.cpp

