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
	@unlink $(PY_SYN_DIR)/syntax.py 2> /dev/null || echo "Python syntax module not installed."
	@unlink $(CP_SYN_DIR)/syntax.h  2> /dev/null || echo "C++ syntax module not installed."
	@rm -rf syntax.h syntax.py
	@rm -rf operators
	@rm -rf max
	@rm -rf *.out
	@echo "Removed syntax files."

max: max.cpp
	@echo "Determining how big numbers can be..."
	@g++ $< -o $@

syntax.py : max
	@echo "Checking to see if pyboro is installed. If not, will be installed."
	@$(CURDIR)/check -i pyboro
	@python syntax_generator.py -m `./max`

syntax.h : max
	@echo "Checking to see if pyboro is installed. If not, will be installed."
	@$(CURDIR)/check -i pyboro
	@python syntax_generator.py -m `./max`

test: $(CP_SYN_DIR)/syntax.h
	@echo "Building test..."
	@g++ -std=c++0x -I$(CURDIR)/../src -o $@ syntest.cpp $(CURDIR)/../src/Types/NodeValue.cpp 
