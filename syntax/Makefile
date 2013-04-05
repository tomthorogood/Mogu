PY_SYN_DIR := $(CURDIR)/../cli/src/moguio/moguio
CP_SYN_DIR := $(CURDIR)/../src/Types

all: $(PY_SYN_DIR)/syntax.py $(CP_SYN_DIR)/syntax.h

$(PY_SYN_DIR)/syntax.py: syntax.py
	@echo "Linking moguio to your syntax file."
	@ln -s $(CURDIR)/syntax.py $(PY_SYN_DIR)/syntax.py

$(CP_SYN_DIR)/syntax.h: syntax.py
	@echo "Linking the Mogu engine to your syntax file."
	@ln -s $(CURDIR)/syntax.h $(CP_SYN_DIR)/syntax.h

clean:
	@unlink $(PY_SYN_DIR)/syntax.py
	@unlink $(CP_SYN_DIR)/syntax.h
	@rm -rf syntax.h syntax.py
	@rm -rf operators
	@echo "Removed syntax files."

syntax.py : operators
	@echo "Checking to see if pyboro is installed. If not, will be installed."
	@$(CURDIR)/check
	@python syntax_generator.py

operators:
	@echo "Generating mathematical operator symbols based on your system."
	@g++ --std=c++0x -o operators operators.cpp

