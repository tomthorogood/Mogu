PY_SYN_DIR := $(CURDIR)/../cli/src/importer
CP_SYN_DIR := $(CURDIR)/../src/Types

all:
	ln -s $(PY_SYN_DIR)/pyRex pyRex
	ln -s $(PY_SYN_DIR)/regexlib.py regexlib.py
	python syntax_generator.py

install:
	cp syntax.py $(PY_SYN_DIR)/
	cp syntax.h $(CP_SYN_DIR)/

test:
	@echo $(PY_SYN_DIR)
	@echo $(CP_SYN_DIR)

clean:
	unlink pyRex
	unlink regexlib.py
	rm -rf syntax.h syntax.py
