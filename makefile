source_files := src
branch_subs := Events Core Redis FiniteAutomaton Parsers Parsers/NodeValueParser Types Perspectives crypt Sessions Validators Security Core/FactoryStations Config

includes := -I$(CURDIR)/src -I/usr/local/include -I/usr/include -L/usr/local/lib
executable := mogu-server
o:=0
devel_libs := -lwt -lwthttp -lboost_signals -lhiredis -lturnleft -lcrypto -lcityhash -lwt
gen_config := cli/src/config_generator.py

turnleft := /usr/local/include/TurnLeftLib/TurnLeft.h
sources := $(source_files) $(foreach s, $(branch_subs), $(source_files)/$s)
cpp_files := $(foreach source, $(sources), $(wildcard $(source)/*.cpp))
objects := $(patsubst %.cpp, %.o, $(cpp_files))
command := g++ -Wall -O$(o) 



all: $(objects) | $(turnleft) 
ifeq ($(dbg),on)
	g++ -Wall -DDEBUG -DTERM_ENABLED -g -pg -o $(executable) $(objects) $(devel_libs)
else
	g++ -Wall -DNDEBUG -o $(executable) $(objects) $(devel_libs)
endif


production: $(objects) | $(turnleft)
	$(command) -o mogu-server $(objects) $(production_libs)

install: mogu.conf
	$(MAKE) uninstall
	mkdir -p /etc/mogu
	cp $< /etc/mogu
	cd cli/c && $(MAKE)
	cp -r cli/* /etc/mogu
	cp -r resources/ /etc/mogu
	ln -s $(CURDIR)/mogu-server /usr/bin/mogu-server
	ln -s /etc/mogu/mogu /usr/bin/mogu

install-cli: mogu.conf
	cd cli/c && $(MAKE)
	cp -r cli/* /etc/mogu
	cp $< /etc/mogu

uninstall:
	rm -f /usr/bin/mogu-server
	rm -f /usr/bin/mogu
	rm -rf /etc/mogu

%.o:
ifeq ($(dbg),on)
	$(command) -c -DDEBUG -DTERM_ENABLED -g -pg $(includes) -o $@ $(patsubst %.o, %.cpp, $@) $(devel_libs)
else
	$(command) -DNDEBUG -c $(includes) -o $@ $(patsubst %.o, %.cpp, $@) $(devel_libs)
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
	rm -rf $(objects)
	rm -rf *.pyc

purge: clean
	rm -rf mogu-server

mogu.conf:
	sudo python $(gen_config)

