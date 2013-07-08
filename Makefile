#= These options will be configured when using the included configure script =# 

#== This is where the server executable and command line executable will live.
#== Default: /usr/bin
#INSTALL_DIR=%(INSTALL_DIR)s
INSTALL_DIR=/usr/bin
#== This is where the configuration files will live.
#== Default: /usr/share/Mogu
#CONFIG_DIR=%(CONFIG_DIR)s
CONFIG_DIR=/usr/share/Mogu

EXECUTABLE=bin/mogu-server


#= Make-time Flags -- set with make [target] flag=value #
#= e.g.: make foo=bar

#== Whether you are currently operating in a python virtual environment.
#== set to true/yes in order to use pip correctly when needed.
VIRTUALENV ?= no

#== Use this flag to compile with debug information built in. This is useful
#== for pretty much everything, except in production.
DEBUG ?= no


# For custom-defined PATHs to search for includes, set this as an make option
XPATH :=

#= BUILD THE COMPILE COMMAND =#
GCC := g++ -std=c++0x -Wall -DDBCONF_DIR=$(CONFIG_DIR)

ifneq ($(DEBUG), no)
	GCC += -DDEBUG
else
	GCC += -DNDEBUG
endif

COMPILE := $(GCC) -c

# Find all subdirectories of of the source directory. #
#
DIRS := ${shell find ${ROOT_DIRECTORY} -type d -print | sed "s:\.::g" | sed "s:^/::g" | grep ^src}

SYNTAX := $(CURDIR)/syntax/syntax.h

TEMP_DIRS := $(CURDIR)/build $(CURDIR)/log $(patsubst src/%,$(CURDIR)/build/%,$(patsubst src,build,$(DIRS)))

# Finds all cpp files in the subdirectories
SOURCE_FILES_ := $(foreach DIR, $(DIRS), $(wildcard $(DIR)/*.cpp))
SOURCE_FILES := $(patsubst src/%, %, $(SOURCE_FILES_))

# All output object files that have to be created.
OBJECT_FILES := $(patsubst %.cpp, %.o, $(SOURCE_FILES))

# The location of all object files
ARTIFACTS := $(patsubst %, build/%, $(OBJECT_FILES))

LIBS := -lwt -lwthttp -lboost_signals -lhiredis -lturnleft -lcrypto -lcityhash -lwt

INCLUDES := -I$(CURDIR)/src $(patsubst %, -I%, $(XPATH)) 

#= Color Escape Sequences.
C_OK=\033[92m
C_INFO=\033[94m
C_WARN=\033[93m
C_FAIL=\033[91m
C_END=\033[0m

#= Compile Recipes

$(EXECUTABLE) : | $(ARTIFACTS)
	@echo "Creating bin/ directory."
	@mkdir -pv bin/
	@echo -n "Linking objects and creating executable..."
	@$(GCC) -o $@ $(ARTIFACTS) $(LIBS) && echo "$(C_OK)DONE!$(C_END)"


build/%.o: src/%.cpp | $(TEMP_DIRS) $(SYNTAX)
	@echo -n "Creating $(notdir $@)..."
	@($(COMPILE) $(INCLUDES) -o $@ $< 2> log/$(notdir $<).log && echo "$(C_OK)DONE!$(C_END)")\
		|| echo "$(C_FAIL)FAILED.$(C_END) See $(C_INFO)log/$(notdir $<).log$(C_END) for details"

#= Generic Recipes

$(SYNTAX):
	@echo "$(C_INFO)Creating syntax files...$(C_END)"
	@cd syntax && $(MAKE)

$(TEMP_DIRS):
	mkdir -pv $@

test: $(TEMP_DIRS)
	@echo "ABSOLUTE CPP FILES: $(SOURCE_FILES_)\n"
	@echo "COMPILE COMMAND $(COMPILE)"
	@echo "CPP FILES: $(SOURCE_FILES)\n"
	@echo "ARTIFACTS: $(ARTIFACTS)\n"
	@echo "INSTALL_DIR: $(INSTALL_DIR)\n"
	@echo "SOURCE DIRECTORIES:  $(DIRS)\n"
	@echo "BUILD DIRECTORIES: $(TEMP_DIRS)\n"

#= Cleaning house

clean:
	@echo "$(C_INFO)Removing generated syntax files...$(C_END)"
	@cd syntax && ($(MAKE) clean || echo "$(C_INFO)Syntax files not created, so not removed.$(C_END)")
	@echo "$(C_INFO)Uninstall moguio python module$(C_END)"
	@sudo pip uninstall moguio --yes || echo "$(C_INFO)moguio not installed, so not removed.$(C_END)"
	@echo "$(C_INFO)Removing temporary files...$(C_END)"
	@rm -rf $(TEMP_DIRS)
