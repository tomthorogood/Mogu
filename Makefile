#-------------------------------#
# Default Build Settings

# If you wish to change the default deployment of 
# the database config, change it here.
DBCONFIG_DIR := /usr/share/Mogu

# When running 'make check', you can set this to use fewer 
# cores. This has no effect unless 'CHECK_UNUSED' is off, as
# by default it will only use one processor.
MAX_JOBS := $(shell grep -c ^processor /proc/cpuinfo)


BUILDDIR 	= build
BINDIR 		= $(BUILDDIR)/bin
OBJDIR 		= $(BUILDDIR)/objects
HEADDIR		= $(BUILDDIR)/include

# When running 'make check', you can turn this off
# to run the check faster, with less accuracy.
# Turning this off is not recommended unless you 
# really want to run the checks but it's taking
# way too long for you.
CHECK_UNUSED := on

# Default optimization level is 0
o := 0

# If you already have a mogu.conf file, you can 
# make with CONFIG=/path/to/mogu.conf 
# in order to preserve it.

# Directory where the final build will 
# be deployed
INSTALL := /usr/share/Mogu

EXAMINE := 

#-------------------------------#
# Executables #

# The python executable responsible for generating the default config file.
gen_config := $(CURDIR)/cli/src/cli/config_generator.py $(CONFIG)

# The final artifact executable
EXECUTABLE := mogu-server

#-------------------------------#
#
# All source files are located here
source_files := src


# Each of the subfolders within the src directory
# Anytime a new subfolder is needed, it must be 
# added here.
#
branch_subs := \
	crypt \
	Config \
	Events \
	FiniteAutomaton \
	Factories \
	Moldable \
	Parsers \
	Parsers/NodeValueParser \
   	Perspectives \
	Redis \
	Security \
	Sessions \
	Types \
	Validators \

# Directories outside the PATH that must be 
# considered when locating header files.
chkincludes := -I$(CURDIR)/src -I/usr/local/include -I/usr/include
includes :=  $(chkincludes) -L/usr/local/lib

# The libraries that the Mogu executable will be linked against.
libs := -lwt -lwthttp -lboost_signals -lhiredis -lturnleft -lcrypto -lcityhash -lwt

# The location of the turnleft utilities
# If they are not found, they will be downloaded
# and built.
turnleft := /usr/local/include/TurnLeftLib/TurnLeft.h

# Create the path names for each of the source directories.
sources := $(source_files) $(foreach s, $(branch_subs), $(source_files)/$s)

# Find the *.cpp files located in each of the source directories.
cpp_files := $(foreach source, $(sources), $(wildcard $(source)/*.cpp))

# Find all *.h files located in each of the source directories.
header_files := $(foreach source, $(sources), $(wildcard $(source)/*.h))

# The object artifacts for the source files.
objects := $(patsubst %.cpp, %.o, $(cpp_files))
logs := $(patsubst %.o, %.o.log, $(objects))

# Mogu requires C++11 standards
flags := -std=c++0x -Wall -O$(o) -DDBCONF_DIR=$(DBCONF_DIR)

ifeq ($(dbg),on)
	$(flags) += -DDEBUG -g -pg
else
	$(flags) += -DNDEBUG
endif

# Location of some pre-compile requirements
MOGUIO_DIR = $(CURDIR)/cli/src/moguio
SYNTAX_PY := $(MOGUIO_DIR)/moguio/syntax.py


all: $(objects) | $(turnleft)
	@echo "Linking object files and creating executable..."
	@g++ $(flags) -o $(EXECUTABLE) $(objects) $(libs)

# Attempts to compile all object files, but does not stop if there are
# errors, instead appending them to 'build_log.txt' for you to review.
# This is meant only for developing in Mogu, as it will help to find issues
# before attempting a full, linked build.
compile-ignore-errors: $(objects)
	@echo "Job complete. Check build_log.txt for information about the builds."


# Attempts to compile a single object file, and then stops regarldess of
# whether the compilation was a success, opening the output in 'vim'. If
# there is no output, then everything is great. Rejoice.
compile-examine: 
	$(MAKE) clean && $(MAKE) $(EXAMINE).o 

%.o: | build $(CURDIR)/syntax/syntax.h
	@echo -n "Compiling $@..."
	@(g++ $(flags) -c $(includes) -o $@ $(patsubst %.o, %.cpp, $@) 2> $@.log \
		&& echo "DONE!") \
		|| (echo "failed. See log in build/ for more information.")
	@if [ -s $@.log ]; then \
		mv $@.log build/ ; \
	else \
   		rm -f $@.log; \
	fi

build:
	mkdir -p $(CURDIR)/build

$(turnleft):
	@echo "TurnLeftLib required but not found. Installing..."
	@git clone git://www.github.com/tomthorogood/TurnLeftLib.git
	@cd TurnLeftLib && $(MAKE) && sudo $(MAKE) install
	@rm -rf TurnLeftLib

upgrade:
	@echo Uninstalling current version of Mogu
	@$(MAKE) uninstall
	@echo Building latest Mogu version
	@$(MAKE)
	@echo Installing latest Mogu version
	@$(MAKE) install

syntax: $(SYNTAX_PY)
	@echo "Autogenerated Syntax File"

$(SYNTAX_PY):
	@cd syntax && $(MAKE)

$(CURDIR)/syntax/syntax.h: syntax

clean:
	@cd $(MOGUIO_DIR) && $(MAKE) clean
	@cd syntax && $(MAKE) clean
	@rm -rf $(CURDIR)/build
	@rm -rf $(objects) $(syntax) $(logs) all.check
	@echo "Removed build, syntax, and check files..."
	@rm -rf *.pyc build_log.txt autom4te.cache *.log
	@sudo pip uninstall moguio -y \
		|| echo "moguio not installed, so not removing"

purge: clean
	@rm -rf configure
	@rm -rf $(EXECUTABLE)
	@rm -f mogu.conf
	@echo "Mogu has been purged from your system."

mogu.conf:
	@echo "$@ not found. Generating now..."
	@sudo python $(gen_config)

moguio: $(SYNTAX_PY)
	@echo "Building the moguio Python library"
	@sudo pip install $(MOGUIO_DIR)

$(DBCONF_DIR)/dbconfig.conf:
	@echo "Creating and updating $@"
	@mkdir -p $(DBCONF_DIR)
	@tail -n +3 $(CURDIR)/dbconfig.conf > $@
	@sed -i "s|<DBCONF_DIR>|$(DBCONF_DIR)|g" $@

check: all.check
	@echo "Removing unwarranted entries from all.check"
	@sed -i "/\(information\)/d" all.check
	@sed -i "/\(portability\)/d" all.check
	@sed -i "/syntax.h/d" all.check # Squelch erros caused by cppcheck bugs with enum classes
	@sed -i "s|src/||g" all.check	# Remove the 'src/' from every line for easier reading.
	@echo "Check complete. You can view the results by opening \"all.check\""
	@sort all.check -o all.check

all.check: syntax
	@echo "Running cppcheck on project. This could take a while..."
ifeq ($(CHECK_UNUSED),off)
	@cppcheck --std=c++11 -j$(MAX_JOBS) --enable=all --max-configs=1 -I$(CURDIR)/src  $(header_files) $(cpp_files) 2> all.check
else
	@cppcheck --std=c++11 --enable=all --max-configs=1 -I$(CURDIR)/src $(header_files) $(cpp_files) 2> all.check
endif

help:
	@echo "[path/to/]*.o            | Compile a specific source file without linkage"
	@echo "build                    | Create build directory"
	@echo "upgrade                  | Uninstall, clean, build, and reinstall Mogu"
	@echo "syntax                   | Create system-specific syntax tables"
	@echo "purge                    | Clean, uninstall, and fully remove Mogu from the system."
	@echo "mogu.conf                | Generate a configuration file with prompts"
	@echo "moguio                   | Create and install the python mogu library"
	@echo "[/path/to]dbconfig.conf  | Create the database config file in the DBCONF_DIR directry"
	@echo "check                    | Use cppcheck to statically check the source code for errors"
	@echo "compile-examine          | Use with 'EXAMINE=path/to/source.cpp' to attempt to compile \
	                                  \n\t\tand show errors for a source file." 
	@echo "compile-ignore-errors    | Try and compile all object files, logging errors for later examination"
	@echo "all                      | Compile, link, and create an executable for Mogu"
