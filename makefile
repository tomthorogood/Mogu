
source_files := src
branch_subs := Events Factories Moldable Redis FiniteAutomaton Parsers Parsers/NodeValueParser Types Perspectives crypt Sessions Validators Security Config

includes := -I$(CURDIR)/src -I/usr/local/include -I/usr/include -L/usr/local/lib
executable := mogu-server
o:=0
devel_libs := -lwt -lwthttp -lboost_signals -lhiredis -lturnleft -lcrypto -lcityhash -lwt
gen_config := cli/src/config_generator.py

turnleft := /usr/local/include/TurnLeftLib/TurnLeft.h
sources := $(source_files) $(foreach s, $(branch_subs), $(source_files)/$s)
cpp_files := $(foreach source, $(sources), $(wildcard $(source)/*.cpp))
objects := $(patsubst %.cpp, %.o, $(cpp_files))
objects_no_exe := $(patsubst src/main.o, , $(objects))
flags := -std=c++0x
command := g++ $(flags) -Wall -O$(o) 


all: $(objects) | $(turnleft) 
ifeq ($(dbg),on)
	g++ $(flags) -Wall -DDEBUG -DTERM_ENABLED -g -pg -o $(executable) $(objects) $(devel_libs)
else
	g++ $(flags) -Wall -DNDEBUG -o $(executable) $(objects) $(devel_libs)
endif


upgrade-database: db_upgrade.cpp $(objects) | $(turnleft)
	g++ $(flags) -Wall $(includes) -o $@ $(objects_no_exe) db_upgrade.cpp $(devel_libs)

install: mogu.conf
	$(MAKE) uninstall
	mkdir -p /etc/mogu
	cp $< /etc/mogu
	cp -r resources/ /etc/mogu
	ln -s $(CURDIR)/mogu-server /usr/bin/mogu-server
	ln -s /etc/mogu/mogu /usr/bin/mogu

uninstall:
	rm -f /usr/bin/mogu-server
	rm -f /usr/bin/mogu
	rm -rf /etc/mogu

%.o:
ifeq ($(dbg),on)
	$(command) -c -DDEBUG -DTERM_ENABLED -g -pg $(includes) -o $@ $(patsubst %.o, %.cpp, $@) 
else
	$(command) -DNDEBUG -c $(includes) -o $@ $(patsubst %.o, %.cpp, $@) 
endif

$(turnleft):
	git clone git://www.github.com/tomthorogood/TurnLeftLib.git
	cd TurnLeftLib && $(MAKE) && sudo $(MAKE) install
	rm -rf TurnLeftLib


upgrade:
	$(MAKE) uninstall
	$(MAKE)
	$(MAKE) install

clean:
	rm -rf $(objects) $(syntax)
	rm -rf *.pyc
	cd syntax && $(MAKE) clean

purge: clean
	rm -rf mogu-server
	rm -f mogu.conf

mogu.conf:
	sudo python $(gen_config)

